#include "dominio/fundoimobiliario.h"

#include <algorithm>

namespace analisador
{

const double FundoImobiliario::DIVIDEND_YIELD_MAXIMO = 12.0;
const double FundoImobiliario::VALOR_PATRIMONIAL_REFERENCIA = 200.0;

FundoImobiliario::FundoImobiliario()
{
}

FundoImobiliario::FundoImobiliario(qint64 id,
                                   const QString& ticker,
                                   const QString& nomeEmpresa,
                                   const QString& setor,
                                   double dividendYield,
                                   double valorPatrimonialPorCota,
                                   const QString& segmento)
    : Ativo(id, ticker, nomeEmpresa, setor)
    , m_dividendYield(dividendYield)
    , m_valorPatrimonialPorCota(valorPatrimonialPorCota)
    , m_segmento(segmento)
{
}

FundoImobiliario::~FundoImobiliario()
{
}

QString FundoImobiliario::tipo() const
{
    return QString::fromUtf8("Fundo Imobiliário");
}

double FundoImobiliario::indiceValor() const
{
    // Criterio de rendimentos: cresce linearmente ate o teto configurado.
    double pontuacaoRendimentos = 0.0;
    if (m_dividendYield > 0.0 && DIVIDEND_YIELD_MAXIMO > 0.0)
    {
        pontuacaoRendimentos = 100.0 * (m_dividendYield / DIVIDEND_YIELD_MAXIMO);
    }
    pontuacaoRendimentos = std::min(100.0, pontuacaoRendimentos);

    // Criterio de solidez patrimonial: cotas com patrimonio maior pontuam mais.
    double pontuacaoPatrimonio = 0.0;
    if (m_valorPatrimonialPorCota > 0.0 && VALOR_PATRIMONIAL_REFERENCIA > 0.0)
    {
        pontuacaoPatrimonio = 100.0 * (m_valorPatrimonialPorCota / VALOR_PATRIMONIAL_REFERENCIA);
    }
    pontuacaoPatrimonio = std::min(100.0, pontuacaoPatrimonio);

    const double indice = (0.7 * pontuacaoRendimentos) + (0.3 * pontuacaoPatrimonio);
    return std::clamp(indice, 0.0, 100.0);
}

QString FundoImobiliario::descricaoFundamentos() const
{
    return QString::fromUtf8("%1 - %2 (segmento %3). Dividend Yield: %4 por cento | "
                             "Valor patrimonial por cota: R$ %5 | Índice de valor: %6 (%7).")
        .arg(m_ticker,
             tipo(),
             m_segmento.isEmpty() ? QString::fromUtf8("não informado") : m_segmento,
             QString::number(m_dividendYield, 'f', 2),
             QString::number(m_valorPatrimonialPorCota, 'f', 2),
             QString::number(indiceValor(), 'f', 1),
             classificarIndice(indiceValor()));
}

double FundoImobiliario::dividendYield() const
{
    return m_dividendYield;
}

double FundoImobiliario::valorPatrimonialPorCota() const
{
    return m_valorPatrimonialPorCota;
}

QString FundoImobiliario::segmento() const
{
    return m_segmento;
}

void FundoImobiliario::definirDividendYield(double dividendYield)
{
    m_dividendYield = dividendYield;
}

void FundoImobiliario::definirValorPatrimonialPorCota(double valorPatrimonialPorCota)
{
    m_valorPatrimonialPorCota = valorPatrimonialPorCota;
}

void FundoImobiliario::definirSegmento(const QString& segmento)
{
    m_segmento = segmento;
}

}
