#include "dominio/acao.h"

#include <algorithm>

namespace analisador
{

const double Acao::PRECO_LUCRO_REFERENCIA = 10.0;
const double Acao::DIVIDEND_YIELD_MAXIMO = 8.0;

Acao::Acao()
{
}

Acao::Acao(qint64 id,
           const QString& ticker,
           const QString& nomeEmpresa,
           const QString& setor,
           double precoLucro,
           double dividendYield,
           double valorMercado)
    : Ativo(id, ticker, nomeEmpresa, setor)
    , m_precoLucro(precoLucro)
    , m_dividendYield(dividendYield)
    , m_valorMercado(valorMercado)
{
}

Acao::~Acao()
{
}

QString Acao::tipo() const
{
    return QString::fromUtf8("Ação");
}

double Acao::indiceValor() const
{
    // Criterio de preco: P/L igual a referencia vale 50 pontos, P/L menor pontua mais.
    double pontuacaoPrecoLucro = 0.0;
    if (m_precoLucro > 0.0)
    {
        pontuacaoPrecoLucro =
            100.0 * (PRECO_LUCRO_REFERENCIA / (m_precoLucro + PRECO_LUCRO_REFERENCIA));
    }

    // Criterio de proventos: cresce linearmente ate o teto configurado.
    double pontuacaoDividendos = 0.0;
    if (m_dividendYield > 0.0 && DIVIDEND_YIELD_MAXIMO > 0.0)
    {
        pontuacaoDividendos = 100.0 * (m_dividendYield / DIVIDEND_YIELD_MAXIMO);
    }
    pontuacaoDividendos = std::min(100.0, pontuacaoDividendos);

    const double indice = (0.6 * pontuacaoPrecoLucro) + (0.4 * pontuacaoDividendos);
    return std::clamp(indice, 0.0, 100.0);
}

QString Acao::descricaoFundamentos() const
{
    return QString::fromUtf8("%1 - %2 (setor %3). P/L: %4 | Dividend Yield: %5 por cento | "
                             "Valor de mercado: R$ %6 | Índice de valor: %7 (%8).")
        .arg(m_ticker,
             tipo(),
             m_setor.isEmpty() ? QString::fromUtf8("não informado") : m_setor,
             QString::number(m_precoLucro, 'f', 2),
             QString::number(m_dividendYield, 'f', 2),
             QString::number(m_valorMercado, 'f', 2),
             QString::number(indiceValor(), 'f', 1),
             classificarIndice(indiceValor()));
}

double Acao::precoLucro() const
{
    return m_precoLucro;
}

double Acao::dividendYield() const
{
    return m_dividendYield;
}

double Acao::valorMercado() const
{
    return m_valorMercado;
}

void Acao::definirPrecoLucro(double precoLucro)
{
    m_precoLucro = precoLucro;
}

void Acao::definirDividendYield(double dividendYield)
{
    m_dividendYield = dividendYield;
}

void Acao::definirValorMercado(double valorMercado)
{
    m_valorMercado = valorMercado;
}

}
