#include "analise/backtester.h"

#include <memory>

#include <QHash>
#include <QStringList>

#include "analise/motoranalise.h"
#include "dominio/recomendacao.h"

namespace analisador
{

const QString Backtester::NOME_CONSOLIDADO = QString::fromUtf8("Consolidado (média ponderada)");

// Aplica a RN024 a um parecer: pareceres Neutro nao viram sinal; Compra acerta
// em retorno positivo e Venda em retorno negativo. O retorno acumulado e o de
// quem seguiu o sinal (vendido ganha na queda).
static void contabilizar(ResultadoBacktestRegra& apuracao, TipoRecomendacao tipo, double retorno)
{
    if (tipo == TipoRecomendacao::Neutro)
    {
        return;
    }

    ++apuracao.sinais;
    if (tipo == TipoRecomendacao::Compra)
    {
        apuracao.somaDosRetornos += retorno;
        if (retorno > 0.0)
        {
            ++apuracao.acertos;
        }
        return;
    }

    apuracao.somaDosRetornos -= retorno;
    if (retorno < 0.0)
    {
        ++apuracao.acertos;
    }
}

double ResultadoBacktestRegra::taxaDeAcerto() const
{
    if (sinais <= 0)
    {
        return 0.0;
    }
    return static_cast<double>(acertos) / static_cast<double>(sinais);
}

double ResultadoBacktestRegra::retornoMedio() const
{
    if (sinais <= 0)
    {
        return 0.0;
    }
    return somaDosRetornos / static_cast<double>(sinais);
}

Backtester::Backtester(const QVector<RegraConfigurada>& regras)
    : m_regras(regras)
{
}

ResultadoBacktest Backtester::executar(const Ativo& ativo,
                                       const QVector<Cotacao>& historico,
                                       int horizonte) const
{
    const int horizonteEfetivo = horizonte > 0 ? horizonte : HORIZONTE_PADRAO;

    ResultadoBacktest resultado;
    resultado.ticker = ativo.ticker();
    resultado.horizonte = horizonteEfetivo;
    resultado.consolidado.regra = NOME_CONSOLIDADO;

    // O motor e montado uma unica vez: as estrategias sao sem estado e podem
    // ser reaplicadas a cada recorte do historico.
    const std::unique_ptr<MotorAnalise> motor = MotorAnalise::comRegrasPadrao(m_regras);

    // Uma linha por regra registrada, preservando a ordem de registro.
    QHash<QString, int> posicaoPorRegra;
    const QStringList nomes = motor->nomesDasRegras();
    for (const QString& nome : nomes)
    {
        if (posicaoPorRegra.contains(nome))
        {
            continue;
        }
        posicaoPorRegra.insert(nome, static_cast<int>(resultado.porRegra.size()));
        ResultadoBacktestRegra apuracao;
        apuracao.regra = nome;
        resultado.porRegra.append(apuracao);
    }

    const int total = static_cast<int>(historico.size());
    const int minimoNecessario = MINIMO_DE_PREGOES + horizonteEfetivo;
    if (total < minimoNecessario)
    {
        resultado.observacao = QString::fromUtf8(
            "Histórico com apenas %1 pregão(ões). O backtesting exige %2: %3 para as regras se "
            "formarem e mais %4 para medir o resultado de cada sinal.")
            .arg(total)
            .arg(minimoNecessario)
            .arg(MINIMO_DE_PREGOES)
            .arg(horizonteEfetivo);
        return resultado;
    }

    const int ultimoPonto = total - 1 - horizonteEfetivo;
    for (int t = MINIMO_DE_PREGOES - 1; t <= ultimoPonto; ++t)
    {
        const double fechamentoAtual = historico.at(t).fechamento();
        if (fechamentoAtual <= 0.0)
        {
            // Cotacao invalida: nao ha como calcular retorno a partir dela.
            continue;
        }

        // Retorno realizado entre o dia do sinal e o fim do horizonte.
        const double retorno = historico.at(t + horizonteEfetivo).fechamento() / fechamentoAtual - 1.0;

        // Recorte com tudo que estava disponivel naquele pregao: nada do futuro
        // entra na decisao das regras.
        const QVector<Cotacao> recorte = historico.mid(0, t + 1);
        ++resultado.pregoesAvaliados;

        const Recomendacao consolidada = motor->analisar(ativo, recorte);
        contabilizar(resultado.consolidado, consolidada.tipo(), retorno);

        const QVector<Recomendacao> pareceres = motor->avaliarRegrasIndividualmente(ativo, recorte);
        for (const Recomendacao& parecer : pareceres)
        {
            const auto posicao = posicaoPorRegra.constFind(parecer.regraAplicada());
            if (posicao == posicaoPorRegra.constEnd())
            {
                continue;
            }
            contabilizar(resultado.porRegra[posicao.value()], parecer.tipo(), retorno);
        }
    }

    if (resultado.pregoesAvaliados == 0)
    {
        resultado.observacao = QString::fromUtf8(
            "Nenhum pregão pôde ser avaliado: as cotações do período não têm preços válidos.");
    }
    return resultado;
}

}
