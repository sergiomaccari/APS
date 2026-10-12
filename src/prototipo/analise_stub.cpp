// Implementacao alternativa das classes de src/analise/ para o protótipo de
// telas (entrega de 13/10).
//
// Este arquivo substitui, nos alvos do protótipo, os .cpp reais de análise.
// Os headers de src/analise/ continuam sendo o contrato: aqui só se definem os
// membros que a interface efetivamente chama.
//
// - CalculadoraIndicadores: cálculo real e simples de MMS, IFR, variação e
//   volatilidade. É a ÚNICA exceção de cálculo aprovada para o protótipo
//   e existe apenas para o gráfico e os cartões do painel.
// - MotorAnalise::criarRegra: devolve uma RegraFixa, que só sabe descrever o
//   critério de cada uma das 4 regras; o parecer é sempre Neutro.
// - Backtester: devolve uma tabela pré-pronta, uma linha por regra,
//   sem percorrer as cotações.

#include <cmath>

#include <QDate>
#include <QLocale>

#include "analise/backtester.h"
#include "analise/calculadoraindicadores.h"
#include "analise/motoranalise.h"
#include "analise/regraanalise.h"

namespace analisador
{

// ---------------------------------------------------------------------------
// CalculadoraIndicadores
// ---------------------------------------------------------------------------

const int CalculadoraIndicadores::PREGOES_POR_ANO = 252;

QVector<Indicador> CalculadoraIndicadores::mediaMovelSimples(const QVector<Cotacao>& historico, int periodo)
{
    QVector<Indicador> serie;
    const int total = static_cast<int>(historico.size());
    if (periodo <= 0 || total < periodo)
    {
        return serie;
    }

    const QString nome = QStringLiteral("MMS %1").arg(periodo);
    serie.reserve(total - periodo + 1);

    // Janela deslizante: soma o fechamento que entra e subtrai o que sai.
    // Cada ponto leva a data da cotação que fecha a janela, para o gráfico
    // posicionar a linha sobre a vela correta.
    double soma = 0.0;
    for (int i = 0; i < total; ++i)
    {
        soma += historico.at(i).fechamento();
        if (i >= periodo)
        {
            soma -= historico.at(i - periodo).fechamento();
        }
        if (i >= periodo - 1)
        {
            serie.append(Indicador(nome, historico.at(i).data(), soma / periodo));
        }
    }
    return serie;
}

// IFR de Wilder (o mesmo método documentado no header): as médias iniciais de
// ganhos e perdas são simples sobre os primeiros "periodo" pregões e as
// seguintes são suavizadas por media(t) = (media(t-1) * (N - 1) + valor(t)) / N.
// O primeiro valor sai na data da cotação de índice "periodo" (são necessárias
// periodo + 1 cotações para formar periodo variações).
QVector<Indicador> CalculadoraIndicadores::indiceForcaRelativa(const QVector<Cotacao>& historico, int periodo)
{
    QVector<Indicador> serie;
    const int total = static_cast<int>(historico.size());
    if (periodo <= 0 || total <= periodo)
    {
        return serie;
    }

    const auto calcular = [](double mediaGanhos, double mediaPerdas) {
        if (mediaPerdas <= 0.0)
        {
            // Sem perdas na janela: força máxima (ou 50 se a série ficou parada).
            return mediaGanhos <= 0.0 ? 50.0 : 100.0;
        }
        const double rs = mediaGanhos / mediaPerdas;
        return 100.0 - 100.0 / (1.0 + rs);
    };

    double mediaGanhos = 0.0;
    double mediaPerdas = 0.0;
    for (int i = 1; i <= periodo; ++i)
    {
        const double variacao = historico.at(i).fechamento() - historico.at(i - 1).fechamento();
        if (variacao > 0.0)
        {
            mediaGanhos += variacao;
        }
        else
        {
            mediaPerdas -= variacao;
        }
    }
    mediaGanhos /= periodo;
    mediaPerdas /= periodo;

    const QString nome = QStringLiteral("RSI %1").arg(periodo);
    serie.reserve(total - periodo);
    serie.append(Indicador(nome, historico.at(periodo).data(), calcular(mediaGanhos, mediaPerdas)));

    for (int i = periodo + 1; i < total; ++i)
    {
        const double variacao = historico.at(i).fechamento() - historico.at(i - 1).fechamento();
        const double ganho = variacao > 0.0 ? variacao : 0.0;
        const double perda = variacao < 0.0 ? -variacao : 0.0;
        mediaGanhos = (mediaGanhos * (periodo - 1) + ganho) / periodo;
        mediaPerdas = (mediaPerdas * (periodo - 1) + perda) / periodo;
        serie.append(Indicador(nome, historico.at(i).data(), calcular(mediaGanhos, mediaPerdas)));
    }
    return serie;
}

std::optional<double> CalculadoraIndicadores::variacaoPercentual(const QVector<Cotacao>& historico, int dias)
{
    const int total = static_cast<int>(historico.size());
    if (dias <= 0 || total < 2)
    {
        return std::nullopt;
    }

    // Compara o último fechamento com o de "dias" pregões antes; se a série
    // for mais curta, usa o primeiro fechamento disponível.
    const int indiceInicial = qMax(0, total - 1 - dias);
    const double inicial = historico.at(indiceInicial).fechamento();
    if (inicial <= 0.0)
    {
        return std::nullopt;
    }
    return (historico.last().fechamento() - inicial) / inicial * 100.0;
}

std::optional<double> CalculadoraIndicadores::volatilidadeAnualizada(const QVector<Cotacao>& historico, int janela)
{
    const int total = static_cast<int>(historico.size());
    if (janela < 2 || total < 3)
    {
        return std::nullopt;
    }

    // Retornos diários simples da janela: os últimos "janela" retornos, ou
    // seja, as últimas janela + 1 cotações.
    const int inicio = qMax(1, total - janela);
    QVector<double> retornos;
    retornos.reserve(total - inicio);
    for (int i = inicio; i < total; ++i)
    {
        const double anterior = historico.at(i - 1).fechamento();
        if (anterior > 0.0)
        {
            retornos.append(historico.at(i).fechamento() / anterior - 1.0);
        }
    }
    if (retornos.size() < 2)
    {
        return std::nullopt;
    }

    double media = 0.0;
    for (const double retorno : retornos)
    {
        media += retorno;
    }
    media /= retornos.size();

    // Desvio-padrão amostral (n - 1) dos retornos diários, anualizado por raiz de 252.
    double somaQuadrados = 0.0;
    for (const double retorno : retornos)
    {
        somaQuadrados += (retorno - media) * (retorno - media);
    }
    const double desvio = std::sqrt(somaQuadrados / (retornos.size() - 1));
    return desvio * std::sqrt(static_cast<double>(PREGOES_POR_ANO)) * 100.0;
}

std::optional<double> CalculadoraIndicadores::ultimoFechamento(const QVector<Cotacao>& historico)
{
    if (historico.isEmpty())
    {
        return std::nullopt;
    }
    return historico.last().fechamento();
}

std::optional<double> CalculadoraIndicadores::ultimoValor(const QVector<Indicador>& serie)
{
    if (serie.isEmpty())
    {
        return std::nullopt;
    }
    return serie.last().valor;
}

// ---------------------------------------------------------------------------
// RegraFixa e MotorAnalise::criarRegra
// ---------------------------------------------------------------------------

namespace
{

enum class TipoRegraFixa
{
    CruzamentoMedias,
    IndiceForcaRelativa,
    DividendYield,
    PrecoLucro
};

// Estratégia concreta do protótipo. Guarda os parâmetros só para que a
// descrição exibida nas telas de regras seja igual à das regras reais; não
// analisa nada (o parecer é sempre Neutro).
class RegraFixa final : public RegraAnalise
{
public:
    explicit RegraFixa(TipoRegraFixa tipo)
        : m_tipo(tipo)
    {
        // Mesmos valores padrão das regras reais.
        switch (m_tipo)
        {
        case TipoRegraFixa::CruzamentoMedias:
            configurar(9, 21);
            break;
        case TipoRegraFixa::IndiceForcaRelativa:
            configurar(30, 70);
            break;
        case TipoRegraFixa::DividendYield:
            configurar(6, 10);
            break;
        case TipoRegraFixa::PrecoLucro:
            configurar(8, 20);
            break;
        }
    }

    QString nome() const override
    {
        switch (m_tipo)
        {
        case TipoRegraFixa::CruzamentoMedias:
            return QStringLiteral("Cruzamento de Medias Moveis");
        case TipoRegraFixa::IndiceForcaRelativa:
            return QStringLiteral("Indice de Forca Relativa");
        case TipoRegraFixa::DividendYield:
            return QStringLiteral("Dividend Yield");
        case TipoRegraFixa::PrecoLucro:
            return QStringLiteral("Preco sobre Lucro");
        }
        return QString();
    }

    QString descricao() const override
    {
        const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
        switch (m_tipo)
        {
        case TipoRegraFixa::CruzamentoMedias:
            return QString::fromUtf8(
                "Compara a média móvel simples de %1 dias com a de %2 dias. O cruzamento "
                "da média curta para cima da longa sinaliza compra; para baixo, venda.")
                .arg(static_cast<int>(m_principal))
                .arg(static_cast<int>(m_secundario));
        case TipoRegraFixa::IndiceForcaRelativa:
            return QString::fromUtf8(
                "Calcula o RSI de %1 períodos. Abaixo de %2 o ativo é considerado sobrevendido "
                "(compra); acima de %3, sobrecomprado (venda).")
                .arg(PERIODO_IFR)
                .arg(brasil.toString(m_principal, 'f', 0), brasil.toString(m_secundario, 'f', 0));
        case TipoRegraFixa::DividendYield:
            return QString::fromUtf8(
                "Avalia os proventos distribuídos nos últimos doze meses. A partir de %1% o ativo "
                "é considerado bom pagador; a partir de %2%, excelente pagador.")
                .arg(brasil.toString(m_principal, 'f', 1), brasil.toString(m_secundario, 'f', 1));
        case TipoRegraFixa::PrecoLucro:
            return QString::fromUtf8(
                "Compara o índice Preço/Lucro da ação com as faixas de referência: até %1 é "
                "considerado atrativo; acima de %2, elevado. Não se aplica a fundos imobiliários.")
                .arg(brasil.toString(m_principal, 'f', 1), brasil.toString(m_secundario, 'f', 1));
        }
        return QString();
    }

    // Normaliza os parâmetros com os mesmos limites das regras reais, para que
    // a descrição mostre exatamente o critério que vigoraria.
    void configurar(double parametroPrincipal, double parametroSecundario) override
    {
        switch (m_tipo)
        {
        case TipoRegraFixa::CruzamentoMedias:
        {
            const int curto = static_cast<int>(parametroPrincipal);
            const int longo = static_cast<int>(parametroSecundario);
            const int curtoEfetivo = curto > 1 ? curto : 9;
            m_principal = curtoEfetivo;
            m_secundario = longo > curtoEfetivo ? longo : curtoEfetivo + 12;
            break;
        }
        case TipoRegraFixa::IndiceForcaRelativa:
            m_principal = (parametroPrincipal > 0.0 && parametroPrincipal < 50.0) ? parametroPrincipal : 30.0;
            m_secundario = (parametroSecundario > 50.0 && parametroSecundario < 100.0) ? parametroSecundario : 70.0;
            break;
        case TipoRegraFixa::DividendYield:
            m_principal = parametroPrincipal > 0.0 ? parametroPrincipal : 6.0;
            m_secundario = parametroSecundario > m_principal ? parametroSecundario : m_principal + 4.0;
            break;
        case TipoRegraFixa::PrecoLucro:
            m_principal = parametroPrincipal > 0.0 ? parametroPrincipal : 8.0;
            m_secundario = parametroSecundario > m_principal ? parametroSecundario : m_principal * 2.5;
            break;
        }
    }

    // Parecer fixo: Neutro, pontuação zero. A análise real chega no 2º bimestre.
    std::optional<Recomendacao> avaliar(const Ativo& ativo, const QVector<Cotacao>& historico) const override
    {
        const QDate dataReferencia = historico.isEmpty() ? QDate::currentDate() : historico.last().data();
        return Recomendacao(0,
                            ativo.id(),
                            TipoRecomendacao::Neutro,
                            QString::fromUtf8("Parecer ilustrativo do protótipo: a avaliação desta regra "
                                              "está prevista para a versão final (2º bimestre)."),
                            nome(),
                            dataReferencia,
                            0.0);
    }

private:
    static constexpr int PERIODO_IFR = 14;

    TipoRegraFixa m_tipo;
    double m_principal = 0.0;
    double m_secundario = 0.0;
};

}

std::unique_ptr<RegraAnalise> MotorAnalise::criarRegra(const QString& nomeRegra)
{
    // Mesmos nomes canônicos aceitos pela fábrica real (regra_configurada.nome_regra).
    const QString nome = nomeRegra.trimmed();
    if (nome.compare(QStringLiteral("Cruzamento de Medias Moveis"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraFixa>(TipoRegraFixa::CruzamentoMedias);
    }
    if (nome.compare(QStringLiteral("Indice de Forca Relativa"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraFixa>(TipoRegraFixa::IndiceForcaRelativa);
    }
    if (nome.compare(QStringLiteral("Dividend Yield"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraFixa>(TipoRegraFixa::DividendYield);
    }
    if (nome.compare(QStringLiteral("Preco sobre Lucro"), Qt::CaseInsensitive) == 0)
    {
        return std::make_unique<RegraFixa>(TipoRegraFixa::PrecoLucro);
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Backtester (tabela fixa)
// ---------------------------------------------------------------------------

const QString Backtester::NOME_CONSOLIDADO = QString::fromUtf8("Consolidado (média ponderada)");

double ResultadoBacktestRegra::taxaDeAcerto() const
{
    if (sinais <= 0)
    {
        return 0.0;
    }
    return static_cast<double>(acertos) / sinais;
}

double ResultadoBacktestRegra::retornoMedio() const
{
    if (sinais <= 0)
    {
        return 0.0;
    }
    return somaDosRetornos / sinais;
}

Backtester::Backtester(const QVector<RegraConfigurada>& regras)
    : m_regras(regras)
{
}

namespace
{

// Linha pré-pronta da tabela de backtesting.
struct LinhaFixa
{
    int sinais;
    int acertos;
    double retornoMedio; // fração (0,0135 = 1,35 %)
};

// Uma linha por posição de regra: taxas de 58 %, 51 %, 63 % e 47 %.
constexpr LinhaFixa LINHAS_FIXAS[] = {
    {19, 11, 0.0135},   // 57,9 % de acerto, +1,35 % por sinal
    {35, 18, 0.0021},   // 51,4 %, +0,21 %
    {27, 17, 0.0187},   // 63,0 %, +1,87 %
    {17, 8, -0.0064},   // 47,1 %, -0,64 %
};
constexpr int QUANTIDADE_LINHAS_FIXAS = static_cast<int>(sizeof(LINHAS_FIXAS) / sizeof(LINHAS_FIXAS[0]));

constexpr LinhaFixa LINHA_CONSOLIDADA = {46, 27, 0.0098}; // 58,7 %, +0,98 %

// Pregões avaliados na tabela fixa (130 cotações - 30 de formação - 10 de horizonte + 1).
constexpr int PREGOES_AVALIADOS_FIXOS = 91;

// Monta a apuração de uma linha. A pequena variação pelo id do ativo (-1, 0
// ou +1) só evita que, ao escolher "todos os ativos", os blocos da tabela
// fiquem idênticos; nenhuma cotação é lida.
ResultadoBacktestRegra montarLinha(const QString& regra, const LinhaFixa& base, qint64 ativoId)
{
    const int ajuste = static_cast<int>(ativoId % 3) - 1;
    ResultadoBacktestRegra linha;
    linha.regra = regra;
    linha.sinais = base.sinais + 2 * ajuste;
    linha.acertos = base.acertos + ajuste;
    linha.somaDosRetornos = (base.retornoMedio + 0.0015 * ajuste) * linha.sinais;
    return linha;
}

}

ResultadoBacktest Backtester::executar(const Ativo& ativo,
                                       const QVector<Cotacao>& historico,
                                       int horizonte) const
{
    ResultadoBacktest resultado;
    resultado.ticker = ativo.ticker();
    resultado.horizonte = horizonte > 0 ? horizonte : HORIZONTE_PADRAO;

    // Única verificação sobre o histórico: sem cotações não há o que mostrar.
    if (historico.isEmpty())
    {
        resultado.consolidado.regra = NOME_CONSOLIDADO;
        resultado.observacao = QString::fromUtf8(
            "Ativo sem cotações importadas: o backtesting precisa do histórico de preços.");
        return resultado;
    }

    resultado.pregoesAvaliados = PREGOES_AVALIADOS_FIXOS;

    int posicao = 0;
    for (const RegraConfigurada& regra : m_regras)
    {
        // Como no sistema real, só regras ativas entram no backtesting.
        if (!regra.ativa())
        {
            continue;
        }
        const LinhaFixa& base = LINHAS_FIXAS[posicao % QUANTIDADE_LINHAS_FIXAS];
        resultado.porRegra.append(montarLinha(regra.nomeRegra(), base, ativo.id()));
        ++posicao;
    }
    resultado.consolidado = montarLinha(NOME_CONSOLIDADO, LINHA_CONSOLIDADA, ativo.id());
    return resultado;
}

}
