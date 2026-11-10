#pragma once

// Estrategia tecnica: Indice de Forca Relativa (RSI) de Wilder.
//
// RSI abaixo do limite de sobrevenda indica ativo descontado (compra); acima do
// limite de sobrecompra indica ativo esticado (venda). Na faixa intermediaria a
// regra nao opina.

#include "analise/regraanalise.h"

namespace analisador
{

class RegraRsi : public RegraAnalise
{
public:
    RegraRsi();
    ~RegraRsi() override;

    QString nome() const override;
    QString descricao() const override;
    void configurar(double parametroPrincipal, double parametroSecundario) override;
    std::optional<Recomendacao> avaliar(const Ativo& ativo,
                                        const QVector<Cotacao>& historico) const override;

    double limiteSobrevenda() const;
    double limiteSobrecompra() const;
    int periodo() const;

private:
    double m_limiteSobrevenda = 30.0;
    double m_limiteSobrecompra = 70.0;
    int m_periodo = 14;
};

}
