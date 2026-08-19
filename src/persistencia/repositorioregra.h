#pragma once

// Repositorio da tabela "regra_configurada" (padrao Repository).
//
// E a fonte dos parametros que o MotorAnalise usa para montar as estrategias
// (Strategy): ligar, desligar ou reparametrizar uma regra e uma operacao de
// banco, sem recompilar o sistema.

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/regraconfigurada.h"

class QSqlQuery;

namespace analisador
{

class RepositorioRegra
{
public:
    RepositorioRegra();

    // Insere a regra e preenche o id gerado pelo banco.
    bool salvar(RegraConfigurada& regra);

    bool atualizar(const RegraConfigurada& regra);

    std::optional<RegraConfigurada> buscarPorId(qint64 id) const;

    // Busca pelo nome canonico da estrategia (o mesmo devolvido por RegraAnalise::nome()).
    std::optional<RegraConfigurada> buscarPorNome(const QString& nomeRegra) const;

    // Todas as regras cadastradas, em ordem de nome.
    QVector<RegraConfigurada> listarTodas() const;

    // Somente as regras ligadas (coluna ativa = 1).
    QVector<RegraConfigurada> listarAtivas() const;

    QString ultimoErro() const;

private:
    // Colunas na ordem esperada por montarRegra(): id, nome_regra, ativa,
    // parametro_principal, parametro_secundario.
    static QString colunas();

    static RegraConfigurada montarRegra(const QSqlQuery& consulta);

    // Le todas as linhas de uma consulta ja executada.
    static QVector<RegraConfigurada> lerTodas(QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
