#pragma once

// Fundo de Investimento Imobiliario negociado na B3 (subclasse concreta de Ativo).
// Fundamentos considerados: Dividend Yield, Valor Patrimonial por Cota e segmento.

#include "dominio/ativo.h"

namespace analisador
{

class FundoImobiliario : public Ativo
{
public:
    FundoImobiliario();
    FundoImobiliario(qint64 id,
                     const QString& ticker,
                     const QString& nomeEmpresa,
                     const QString& setor,
                     double dividendYield,
                     double valorPatrimonialPorCota,
                     const QString& segmento);
    ~FundoImobiliario() override;

    // "Fundo Imobiliario" (com acento) - identifica o tipo polimorficamente.
    QString tipo() const override;

    // Combina Dividend Yield (peso 70%) e Valor Patrimonial por Cota (peso 30%) em [0, 100].
    double indiceValor() const override;

    QString descricaoFundamentos() const override;

    // Percentual de rendimentos distribuidos nos ultimos doze meses (ex.: 9.2 significa 9,2%).
    double dividendYield() const;

    // Valor patrimonial de cada cota, em reais.
    double valorPatrimonialPorCota() const;

    // Segmento de atuacao do fundo (Logistica, Lajes Corporativas, Papel, etc.).
    QString segmento() const;

    void definirDividendYield(double dividendYield);
    void definirValorPatrimonialPorCota(double valorPatrimonialPorCota);
    void definirSegmento(const QString& segmento);

private:
    // Dividend Yield (em %) que ja garante pontuacao maxima no criterio de rendimentos.
    static const double DIVIDEND_YIELD_MAXIMO;

    // Valor patrimonial por cota de referencia para pontuacao maxima de solidez.
    static const double VALOR_PATRIMONIAL_REFERENCIA;

    double m_dividendYield = 0.0;
    double m_valorPatrimonialPorCota = 0.0;
    QString m_segmento;
};

}
