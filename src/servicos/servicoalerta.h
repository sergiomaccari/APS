#pragma once

// Monitoramento de alertas de preco - implementa o padrao OBSERVER.
//
// A interface grafica registra-se como ObservadorAlerta e e notificada sempre
// que uma condicao cadastrada e satisfeita durante a avaliacao, sem que o
// servico conheca a janela concreta que exibira a notificacao.

#include <optional>
#include <vector>

#include <QString>
#include <QVector>

#include "dominio/alerta.h"
#include "dominio/ativo.h"
#include "persistencia/repositorioalerta.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocotacao.h"

namespace analisador
{

class ObservadorAlerta
{
public:
    virtual ~ObservadorAlerta() = default;

    // Chamado uma vez por alerta disparado, com o valor que satisfez a condicao.
    virtual void alertaDisparado(const Alerta& alerta, const Ativo& ativo, double valorObservado) = 0;
};

class ServicoAlerta
{
public:
    ServicoAlerta(RepositorioAlerta& repositorioAlerta,
                  RepositorioCotacao& repositorioCotacao,
                  RepositorioAtivo& repositorioAtivo);

    void registrarObservador(ObservadorAlerta* observador);
    void removerObservador(ObservadorAlerta* observador);

    bool criarAlerta(qint64 usuarioId,
                     qint64 ativoId,
                     TipoCondicaoAlerta condicao,
                     double valorReferencia,
                     Alerta* criado = nullptr);

    bool arquivarAlerta(qint64 alertaId);
    bool removerAlerta(qint64 alertaId);

    QVector<Alerta> listarPorUsuario(qint64 usuarioId) const;

    // Percorre os alertas monitorados do usuario, compara com a ultima cotacao
    // de cada ativo e dispara os que satisfazem a condicao. Devolve a
    // quantidade de alertas disparados nesta avaliacao.
    int avaliarAlertas(qint64 usuarioId);

    QString ultimoErro() const;

private:
    void notificar(const Alerta& alerta, const Ativo& ativo, double valorObservado);

    RepositorioAlerta& m_repositorioAlerta;
    RepositorioCotacao& m_repositorioCotacao;
    RepositorioAtivo& m_repositorioAtivo;
    std::vector<ObservadorAlerta*> m_observadores;
    QString m_ultimoErro;
};

}
