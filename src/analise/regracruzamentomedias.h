#pragma once

// Estrategia tecnica: cruzamento de medias moveis.
//
// Compra quando a media curta cruza para cima da media longa (sinal classico de
// reversao de tendencia para alta) e venda no cruzamento inverso. Sem
// cruzamento recente, a regra ainda opina com menos convicção apenas indicando
// a tendencia vigente.

#include "analise/regraanalise.h"

namespace analisador
{

class RegraCruzamentoMedias : public RegraAnalise
{
public:
    RegraCruzamentoMedias();
    ~RegraCruzamentoMedias() override;

    QString nome() const override;
    QString descricao() const override;
    void configurar(double parametroPrincipal, double parametroSecundario) override;
    std::optional<Recomendacao> avaliar(const Ativo& ativo,
                                        const QVector<Cotacao>& historico) const override;

    int periodoCurto() const;
    int periodoLongo() const;

private:
    int m_periodoCurto = 9;
    int m_periodoLongo = 21;
};

}
