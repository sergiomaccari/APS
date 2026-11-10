#include "persistencia/repositorioalerta.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioAlerta::RepositorioAlerta()
{
}

QString RepositorioAlerta::colunas()
{
    return QStringLiteral("id, usuario_id, ativo_id, condicao, valor_referencia, estado, "
                          "criado_em, disparado_em");
}

Alerta RepositorioAlerta::montarAlerta(const QSqlQuery& consulta)
{
    Alerta alerta;
    alerta.definirId(consulta.value(0).toLongLong());
    alerta.definirUsuarioId(consulta.value(1).toLongLong());
    alerta.definirAtivoId(consulta.value(2).toLongLong());
    alerta.definirCondicao(tipoCondicaoAlertaDeTexto(consulta.value(3).toString()));
    alerta.definirValorReferencia(consulta.value(4).toDouble());
    alerta.definirEstado(estadoAlertaDeTexto(consulta.value(5).toString()));
    alerta.definirCriadoEm(QDate::fromString(consulta.value(6).toString(), Qt::ISODate));

    // disparado_em e NULL enquanto o alerta nao dispara.
    const QVariant disparadoEm = consulta.value(7);
    if (!disparadoEm.isNull())
    {
        alerta.definirDisparadoEm(QDateTime::fromString(disparadoEm.toString(), Qt::ISODate));
    }
    return alerta;
}

QVector<Alerta> RepositorioAlerta::lerTodos(QSqlQuery& consulta)
{
    QVector<Alerta> alertas;
    while (consulta.next())
    {
        alertas.append(montarAlerta(consulta));
    }
    return alertas;
}

bool RepositorioAlerta::salvar(Alerta& alerta)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO alerta (usuario_id, ativo_id, condicao, valor_referencia, estado, "
        "criado_em, disparado_em) "
        "VALUES (:usuario_id, :ativo_id, :condicao, :valor_referencia, :estado, "
        ":criado_em, :disparado_em)"));
    consulta.bindValue(QStringLiteral(":usuario_id"), alerta.usuarioId());
    consulta.bindValue(QStringLiteral(":ativo_id"), alerta.ativoId());
    consulta.bindValue(QStringLiteral(":condicao"), paraTexto(alerta.condicao()));
    consulta.bindValue(QStringLiteral(":valor_referencia"), alerta.valorReferencia());
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(alerta.estado()));
    consulta.bindValue(QStringLiteral(":criado_em"), alerta.criadoEm().toString(Qt::ISODate));
    consulta.bindValue(QStringLiteral(":disparado_em"),
                       alerta.disparadoEm().isValid()
                           ? QVariant(alerta.disparadoEm().toString(Qt::ISODate))
                           : QVariant());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    alerta.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

bool RepositorioAlerta::atualizar(const Alerta& alerta)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE alerta SET usuario_id = :usuario_id, ativo_id = :ativo_id, "
        "condicao = :condicao, valor_referencia = :valor_referencia, estado = :estado, "
        "criado_em = :criado_em, disparado_em = :disparado_em WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":usuario_id"), alerta.usuarioId());
    consulta.bindValue(QStringLiteral(":ativo_id"), alerta.ativoId());
    consulta.bindValue(QStringLiteral(":condicao"), paraTexto(alerta.condicao()));
    consulta.bindValue(QStringLiteral(":valor_referencia"), alerta.valorReferencia());
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(alerta.estado()));
    consulta.bindValue(QStringLiteral(":criado_em"), alerta.criadoEm().toString(Qt::ISODate));
    consulta.bindValue(QStringLiteral(":disparado_em"),
                       alerta.disparadoEm().isValid()
                           ? QVariant(alerta.disparadoEm().toString(Qt::ISODate))
                           : QVariant());
    consulta.bindValue(QStringLiteral(":id"), alerta.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

bool RepositorioAlerta::remover(qint64 id)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("DELETE FROM alerta WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":id"), id);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

std::optional<Alerta> RepositorioAlerta::buscarPorId(qint64 id) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM alerta WHERE id = :id").arg(colunas()));
    consulta.bindValue(QStringLiteral(":id"), id);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return std::nullopt;
    }

    m_ultimoErro.clear();
    if (!consulta.next())
    {
        return std::nullopt;
    }
    return montarAlerta(consulta);
}

QVector<Alerta> RepositorioAlerta::listarPorUsuario(qint64 usuarioId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM alerta WHERE usuario_id = :usuario_id "
                                    "ORDER BY criado_em DESC, id DESC")
                         .arg(colunas()));
    consulta.bindValue(QStringLiteral(":usuario_id"), usuarioId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return QVector<Alerta>();
    }

    m_ultimoErro.clear();
    return lerTodos(consulta);
}

QVector<Alerta> RepositorioAlerta::listarMonitoradosPorUsuario(qint64 usuarioId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM alerta "
                                    "WHERE usuario_id = :usuario_id AND estado = :estado "
                                    "ORDER BY criado_em DESC, id DESC")
                         .arg(colunas()));
    consulta.bindValue(QStringLiteral(":usuario_id"), usuarioId);
    consulta.bindValue(QStringLiteral(":estado"), paraTexto(EstadoAlerta::Ativo));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return QVector<Alerta>();
    }

    m_ultimoErro.clear();
    return lerTodos(consulta);
}

QVector<Alerta> RepositorioAlerta::listarPorAtivo(qint64 ativoId) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM alerta WHERE ativo_id = :ativo_id "
                                    "ORDER BY criado_em DESC, id DESC")
                         .arg(colunas()));
    consulta.bindValue(QStringLiteral(":ativo_id"), ativoId);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return QVector<Alerta>();
    }

    m_ultimoErro.clear();
    return lerTodos(consulta);
}

QString RepositorioAlerta::ultimoErro() const
{
    return m_ultimoErro;
}

}
