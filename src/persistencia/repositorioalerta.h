#pragma once

// Repositorio da tabela "alerta" (padrao Repository).
//
// A coluna estado guarda exatamente o texto devolvido por paraTexto(EstadoAlerta)
// e a coluna disparado_em aceita NULL enquanto o alerta nao dispara.

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/alerta.h"

class QSqlQuery;

namespace analisador
{

class RepositorioAlerta
{
public:
    RepositorioAlerta();

    // Insere o alerta e preenche o id gerado pelo banco.
    bool salvar(Alerta& alerta);

    bool atualizar(const Alerta& alerta);
    bool remover(qint64 id);

    std::optional<Alerta> buscarPorId(qint64 id) const;

    // Todos os alertas do usuario, do mais recente para o mais antigo.
    QVector<Alerta> listarPorUsuario(qint64 usuarioId) const;

    // Somente os alertas ainda monitorados (estado 'Ativo').
    QVector<Alerta> listarMonitoradosPorUsuario(qint64 usuarioId) const;

    // Alertas de todos os usuarios sobre um ativo.
    QVector<Alerta> listarPorAtivo(qint64 ativoId) const;

    QString ultimoErro() const;

private:
    // Colunas na ordem esperada por montarAlerta(): id, usuario_id, ativo_id,
    // condicao, valor_referencia, estado, criado_em, disparado_em.
    static QString colunas();

    static Alerta montarAlerta(const QSqlQuery& consulta);

    // Le todas as linhas de uma consulta ja executada.
    static QVector<Alerta> lerTodos(QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
