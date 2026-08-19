#include "analise/regradividendyield.h"

#include <QDate>
#include <QLocale>

#include "dominio/acao.h"
#include "dominio/fundoimobiliario.h"

namespace analisador
{

RegraDividendYield::RegraDividendYield()
{
}

RegraDividendYield::~RegraDividendYield()
{
}

QString RegraDividendYield::nome() const
{
    return QStringLiteral("Dividend Yield");
}

QString RegraDividendYield::descricao() const
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    return QString::fromUtf8(
        "Avalia os proventos distribuídos nos últimos doze meses. A partir de %1% o ativo "
        "é considerado bom pagador; a partir de %2%, excelente pagador.")
        .arg(brasil.toString(m_dividendYieldMinimo, 'f', 1),
             brasil.toString(m_dividendYieldExcelente, 'f', 1));
}

void RegraDividendYield::configurar(double parametroPrincipal, double parametroSecundario)
{
    m_dividendYieldMinimo = parametroPrincipal > 0.0 ? parametroPrincipal : 6.0;
    m_dividendYieldExcelente = parametroSecundario > m_dividendYieldMinimo
        ? parametroSecundario
        : m_dividendYieldMinimo + 4.0;
}

double RegraDividendYield::dividendYieldMinimo() const
{
    return m_dividendYieldMinimo;
}

double RegraDividendYield::dividendYieldExcelente() const
{
    return m_dividendYieldExcelente;
}

std::optional<double> RegraDividendYield::lerDividendYield(const Ativo& ativo)
{
    if (const auto* acao = dynamic_cast<const Acao*>(&ativo))
    {
        return acao->dividendYield();
    }
    if (const auto* fundo = dynamic_cast<const FundoImobiliario*>(&ativo))
    {
        return fundo->dividendYield();
    }
    return std::nullopt;
}

std::optional<Recomendacao> RegraDividendYield::avaliar(const Ativo& ativo,
                                                        const QVector<Cotacao>& historico) const
{
    const std::optional<double> leitura = lerDividendYield(ativo);
    if (!leitura.has_value() || leitura.value() <= 0.0)
    {
        // Ativo sem dividend yield cadastrado: a regra nao tem base para opinar.
        return std::nullopt;
    }

    const double dividendYield = leitura.value();
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const QString dyTexto = brasil.toString(dividendYield, 'f', 2);

    double pontuacao = 0.0;
    QString justificativa;

    if (dividendYield >= m_dividendYieldExcelente)
    {
        pontuacao = 0.75;
        justificativa = QString::fromUtf8(
            "Dividend Yield de %1% supera o patamar de excelência (%2%): "
            "distribuição de proventos muito acima da média para %3.")
            .arg(dyTexto, brasil.toString(m_dividendYieldExcelente, 'f', 1), ativo.tipo());
    }
    else if (dividendYield >= m_dividendYieldMinimo)
    {
        pontuacao = 0.40;
        justificativa = QString::fromUtf8(
            "Dividend Yield de %1% atende o mínimo exigido (%2%): bom pagador de proventos.")
            .arg(dyTexto, brasil.toString(m_dividendYieldMinimo, 'f', 1));
    }
    else
    {
        pontuacao = -0.15;
        justificativa = QString::fromUtf8(
            "Dividend Yield de %1% está abaixo do mínimo exigido (%2%): "
            "retorno em proventos pouco atrativo.")
            .arg(dyTexto, brasil.toString(m_dividendYieldMinimo, 'f', 1));
    }

    Recomendacao recomendacao;
    recomendacao.definirAtivoId(ativo.id());
    recomendacao.definirPontuacao(pontuacao);
    recomendacao.definirTipo(Recomendacao::tipoPorPontuacao(pontuacao));
    recomendacao.definirJustificativa(justificativa);
    recomendacao.definirRegraAplicada(nome());
    const QDate dataReferencia = historico.isEmpty() ? QDate::currentDate() : historico.last().data();
    recomendacao.definirGeradaEm(dataReferencia);
    return recomendacao;
}

}
