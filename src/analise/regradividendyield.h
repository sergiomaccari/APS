#pragma once

// Estrategia fundamentalista: Dividend Yield.
//
// Aplica-se a acoes e a fundos imobiliarios (as duas subclasses expoem o DY).
// A leitura do DY e feita por dynamic_cast, exemplo direto de polimorfismo em
// tempo de execucao: a regra nao precisa saber de antemao qual e o tipo do ativo.

#include "analise/regraanalise.h"

namespace analisador
{

class RegraDividendYield : public RegraAnalise
{
public:
    RegraDividendYield();
    ~RegraDividendYield() override;

    QString nome() const override;
    QString descricao() const override;
    void configurar(double parametroPrincipal, double parametroSecundario) override;
    std::optional<Recomendacao> avaliar(const Ativo& ativo,
                                        const QVector<Cotacao>& historico) const override;

    double dividendYieldMinimo() const;
    double dividendYieldExcelente() const;

private:
    // Le o dividend yield do ativo, qualquer que seja a subclasse concreta.
    // Devolve std::nullopt quando o tipo nao possui esse fundamento.
    static std::optional<double> lerDividendYield(const Ativo& ativo);

    double m_dividendYieldMinimo = 6.0;
    double m_dividendYieldExcelente = 10.0;
};

}
