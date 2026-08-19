#pragma once

// Acao negociada na B3 (subclasse concreta de Ativo).
// Fundamentos considerados: Preco/Lucro, Dividend Yield e Valor de Mercado.

#include "dominio/ativo.h"

namespace analisador
{

class Acao : public Ativo
{
public:
    Acao();
    Acao(qint64 id,
         const QString& ticker,
         const QString& nomeEmpresa,
         const QString& setor,
         double precoLucro,
         double dividendYield,
         double valorMercado);
    ~Acao() override;

    // "Acao" (com cedilha e til) - identifica o tipo polimorficamente.
    QString tipo() const override;

    // Combina Preco/Lucro (peso 60%) e Dividend Yield (peso 40%) em [0, 100].
    double indiceValor() const override;

    QString descricaoFundamentos() const override;

    // Preco dividido pelo lucro por acao. Valores menores tendem a indicar acao mais barata.
    double precoLucro() const;

    // Percentual de proventos distribuidos nos ultimos doze meses (ex.: 8.5 significa 8,5%).
    double dividendYield() const;

    // Valor de mercado da companhia em reais.
    double valorMercado() const;

    void definirPrecoLucro(double precoLucro);
    void definirDividendYield(double dividendYield);
    void definirValorMercado(double valorMercado);

private:
    // Preco/Lucro de referencia: acima disso a pontuacao fundamentalista cai pela metade.
    static const double PRECO_LUCRO_REFERENCIA;

    // Dividend Yield (em %) que ja garante pontuacao maxima no criterio de proventos.
    static const double DIVIDEND_YIELD_MAXIMO;

    double m_precoLucro = 0.0;
    double m_dividendYield = 0.0;
    double m_valorMercado = 0.0;
};

}
