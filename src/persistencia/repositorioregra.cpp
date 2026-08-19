#include "persistencia/repositorioregra.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioRegra::RepositorioRegra()
{
}

QString RepositorioRegra::colunas()
{
    return QStringLiteral("id, nome_regra, ativa, parametro_principal, parametro_secundario");
}

RegraConfigurada RepositorioRegra::montarRegra(const QSqlQuery& consulta)
{
    RegraConfigurada regra;
    regra.definirId(consulta.value(0).toLongLong());
    regra.definirNomeRegra(consulta.value(1).toString());
    // A coluna "ativa" e um inteiro 0/1 (SQLite nao tem tipo booleano).
    regra.definirAtiva(consulta.value(2).toInt() != 0);
    regra.definirParametroPrincipal(consulta.value(3).toDouble());
    regra.definirParametroSecundario(consulta.value(4).toDouble());
    return regra;
}

QVector<RegraConfigurada> RepositorioRegra::lerTodas(QSqlQuery& consulta)
{
    QVector<RegraConfigurada> regras;
    while (consulta.next())
    {
        regras.append(montarRegra(consulta));
    }
    return regras;
}

bool RepositorioRegra::salvar(RegraConfigurada& regra)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO regra_configurada "
        "(nome_regra, ativa, parametro_principal, parametro_secundario) "
        "VALUES (:nome_regra, :ativa, :parametro_principal, :parametro_secundario)"));
    consulta.bindValue(QStringLiteral(":nome_regra"), regra.nomeRegra());
    consulta.bindValue(QStringLiteral(":ativa"), regra.ativa() ? 1 : 0);
    consulta.bindValue(QStringLiteral(":parametro_principal"), regra.parametroPrincipal());
    consulta.bindValue(QStringLiteral(":parametro_secundario"), regra.parametroSecundario());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    regra.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

bool RepositorioRegra::atualizar(const RegraConfigurada& regra)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE regra_configurada SET nome_regra = :nome_regra, ativa = :ativa, "
        "parametro_principal = :parametro_principal, "
        "parametro_secundario = :parametro_secundario WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":nome_regra"), regra.nomeRegra());
    consulta.bindValue(QStringLiteral(":ativa"), regra.ativa() ? 1 : 0);
    consulta.bindValue(QStringLiteral(":parametro_principal"), regra.parametroPrincipal());
    consulta.bindValue(QStringLiteral(":parametro_secundario"), regra.parametroSecundario());
    consulta.bindValue(QStringLiteral(":id"), regra.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

std::optional<RegraConfigurada> RepositorioRegra::buscarPorId(qint64 id) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(
        QStringLiteral("SELECT %1 FROM regra_configurada WHERE id = :id").arg(colunas()));
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
    return montarRegra(consulta);
}

std::optional<RegraConfigurada> RepositorioRegra::buscarPorNome(const QString& nomeRegra) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM regra_configurada "
                                    "WHERE nome_regra = :nome_regra")
                         .arg(colunas()));
    consulta.bindValue(QStringLiteral(":nome_regra"), nomeRegra.trimmed());

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
    return montarRegra(consulta);
}

QVector<RegraConfigurada> RepositorioRegra::listarTodas() const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());

    if (!consulta.exec(
            QStringLiteral("SELECT %1 FROM regra_configurada ORDER BY nome_regra").arg(colunas())))
    {
        m_ultimoErro = consulta.lastError().text();
        return QVector<RegraConfigurada>();
    }

    m_ultimoErro.clear();
    return lerTodas(consulta);
}

QVector<RegraConfigurada> RepositorioRegra::listarAtivas() const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT %1 FROM regra_configurada WHERE ativa = 1 "
                                    "ORDER BY nome_regra")
                         .arg(colunas()));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return QVector<RegraConfigurada>();
    }

    m_ultimoErro.clear();
    return lerTodas(consulta);
}

QString RepositorioRegra::ultimoErro() const
{
    return m_ultimoErro;
}

}
