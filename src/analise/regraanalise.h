#pragma once

// Interface do padrao de projeto STRATEGY do sistema.
//
// Cada regra de analise (cruzamento de medias, RSI, dividend yield, preco/lucro)
// e uma estrategia intercambiavel que recebe o ativo e sua serie historica e
// devolve - ou nao - uma recomendacao justificada. O MotorAnalise combina as
// estrategias ativas sem conhecer nenhuma delas concretamente, o que permite
// acrescentar novas regras sem alterar o motor nem a interface grafica.

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/ativo.h"
#include "dominio/cotacao.h"
#include "dominio/recomendacao.h"

namespace analisador
{

class RegraAnalise
{
public:
    virtual ~RegraAnalise() = default;

    // Nome canonico da regra. Precisa ser igual ao gravado em
    // regra_configurada.nome_regra para a fabrica do motor encontrar a estrategia.
    virtual QString nome() const = 0;

    // Explicacao do criterio da regra, exibida na tela de regras.
    virtual QString descricao() const = 0;

    // Ajusta os parametros vindos de RegraConfigurada.
    virtual void configurar(double parametroPrincipal, double parametroSecundario) = 0;

    // Avalia o ativo. Devolve std::nullopt quando a regra nao tem opiniao
    // (dados insuficientes ou criterio nao aplicavel ao tipo de ativo).
    // A recomendacao devolvida traz pontuacao em [-1, 1] e justificativa com
    // os numeros concretos que levaram a conclusao.
    virtual std::optional<Recomendacao> avaliar(const Ativo& ativo,
                                                const QVector<Cotacao>& historico) const = 0;
};

}
