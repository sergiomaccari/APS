#pragma once

// Estrategia fundamentalista: Preco sobre Lucro (P/L).
//
// Aplica-se SOMENTE a acoes. Fundos imobiliarios nao possuem P/L, portanto a
// regra se abstem quando o dynamic_cast para Acao falha - o caminho didatico
// mais claro de polimorfismo com verificacao de tipo em tempo de execucao.

#include "analise/regraanalise.h"

namespace analisador
{

class RegraPrecoLucro : public RegraAnalise
{
public:
    RegraPrecoLucro();
    ~RegraPrecoLucro() override;

    QString nome() const override;
    QString descricao() const override;
    void configurar(double parametroPrincipal, double parametroSecundario) override;
    std::optional<Recomendacao> avaliar(const Ativo& ativo,
                                        const QVector<Cotacao>& historico) const override;

    double precoLucroAtrativo() const;
    double precoLucroElevado() const;

private:
    double m_precoLucroAtrativo = 8.0;
    double m_precoLucroElevado = 20.0;
};

}
