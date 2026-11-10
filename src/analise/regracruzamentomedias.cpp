#include "analise/regracruzamentomedias.h"

#include <QDate>
#include <QLocale>

#include "analise/calculadoraindicadores.h"

namespace analisador
{

RegraCruzamentoMedias::RegraCruzamentoMedias()
{
}

RegraCruzamentoMedias::~RegraCruzamentoMedias()
{
}

QString RegraCruzamentoMedias::nome() const
{
    // Precisa coincidir com regra_configurada.nome_regra (ver 002_dados_semente.sql).
    return QStringLiteral("Cruzamento de Medias Moveis");
}

QString RegraCruzamentoMedias::descricao() const
{
    return QString::fromUtf8(
        "Compara a média móvel simples de %1 dias com a de %2 dias. O cruzamento "
        "da média curta para cima da longa sinaliza compra; para baixo, venda.")
        .arg(m_periodoCurto)
        .arg(m_periodoLongo);
}

void RegraCruzamentoMedias::configurar(double parametroPrincipal, double parametroSecundario)
{
    const int curto = static_cast<int>(parametroPrincipal);
    const int longo = static_cast<int>(parametroSecundario);
    m_periodoCurto = curto > 1 ? curto : 9;
    m_periodoLongo = longo > m_periodoCurto ? longo : m_periodoCurto + 12;
}

int RegraCruzamentoMedias::periodoCurto() const
{
    return m_periodoCurto;
}

int RegraCruzamentoMedias::periodoLongo() const
{
    return m_periodoLongo;
}

std::optional<Recomendacao> RegraCruzamentoMedias::avaliar(const Ativo& ativo,
                                                          const QVector<Cotacao>& historico) const
{
    const QVector<Indicador> curta = CalculadoraIndicadores::mediaMovelSimples(historico, m_periodoCurto);
    const QVector<Indicador> longa = CalculadoraIndicadores::mediaMovelSimples(historico, m_periodoLongo);

    // Sao necessarios dois pontos de cada media para detectar cruzamento.
    if (curta.size() < 2 || longa.size() < 2)
    {
        return std::nullopt;
    }

    // As duas series terminam na mesma data, portanto o alinhamento e feito
    // contando a partir do fim.
    const double curtaAtual = curta.last().valor;
    const double curtaAnterior = curta.at(curta.size() - 2).valor;
    const double longaAtual = longa.last().valor;
    const double longaAnterior = longa.at(longa.size() - 2).valor;
    const QDate dataAtual = longa.last().data;

    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QString textoCurta = brasil.toString(curtaAtual, 'f', 2);
    const QString textoLonga = brasil.toString(longaAtual, 'f', 2);
    const QString dataTexto = dataAtual.toString(QStringLiteral("dd/MM/yyyy"));

    double pontuacao = 0.0;
    QString justificativa;

    const bool cruzouParaCima = curtaAnterior <= longaAnterior && curtaAtual > longaAtual;
    const bool cruzouParaBaixo = curtaAnterior >= longaAnterior && curtaAtual < longaAtual;

    if (cruzouParaCima)
    {
        pontuacao = 0.80;
        justificativa = QString::fromUtf8(
            "A média de %1 dias (R$ %2) cruzou acima da média de %3 dias (R$ %4) em %5, "
            "sinal de reversão para tendência de alta.")
            .arg(m_periodoCurto)
            .arg(textoCurta)
            .arg(m_periodoLongo)
            .arg(textoLonga, dataTexto);
    }
    else if (cruzouParaBaixo)
    {
        pontuacao = -0.80;
        justificativa = QString::fromUtf8(
            "A média de %1 dias (R$ %2) cruzou abaixo da média de %3 dias (R$ %4) em %5, "
            "sinal de reversão para tendência de baixa.")
            .arg(m_periodoCurto)
            .arg(textoCurta)
            .arg(m_periodoLongo)
            .arg(textoLonga, dataTexto);
    }
    else if (curtaAtual > longaAtual)
    {
        pontuacao = 0.35;
        justificativa = QString::fromUtf8(
            "A média de %1 dias (R$ %2) segue acima da média de %3 dias (R$ %4) em %5: "
            "tendência de alta em curso, sem cruzamento recente.")
            .arg(m_periodoCurto)
            .arg(textoCurta)
            .arg(m_periodoLongo)
            .arg(textoLonga, dataTexto);
    }
    else if (curtaAtual < longaAtual)
    {
        pontuacao = -0.35;
        justificativa = QString::fromUtf8(
            "A média de %1 dias (R$ %2) segue abaixo da média de %3 dias (R$ %4) em %5: "
            "tendência de baixa em curso, sem cruzamento recente.")
            .arg(m_periodoCurto)
            .arg(textoCurta)
            .arg(m_periodoLongo)
            .arg(textoLonga, dataTexto);
    }
    else
    {
        // Medias praticamente coladas: a regra prefere nao opinar.
        return std::nullopt;
    }

    Recomendacao recomendacao;
    recomendacao.definirAtivoId(ativo.id());
    recomendacao.definirPontuacao(pontuacao);
    recomendacao.definirTipo(Recomendacao::tipoPorPontuacao(pontuacao));
    recomendacao.definirJustificativa(justificativa);
    recomendacao.definirRegraAplicada(nome());
    recomendacao.definirGeradaEm(dataAtual.isValid() ? dataAtual : QDate::currentDate());
    return recomendacao;
}

}
