#include "persistencia/repositoriousuario.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "persistencia/bancodedados.h"

namespace analisador
{

RepositorioUsuario::RepositorioUsuario()
{
}

Usuario RepositorioUsuario::montarUsuario(const QSqlQuery& consulta)
{
    Usuario usuario;
    usuario.definirId(consulta.value(0).toLongLong());
    usuario.definirNome(consulta.value(1).toString());
    usuario.definirEmail(consulta.value(2).toString());
    usuario.definirSenhaHash(consulta.value(3).toString());
    usuario.definirPapel(papelUsuarioDeTexto(consulta.value(4).toString()));
    return usuario;
}

bool RepositorioUsuario::salvar(Usuario& usuario)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "INSERT INTO usuario (nome, email, senha_hash, papel) "
        "VALUES (:nome, :email, :senha_hash, :papel)"));
    consulta.bindValue(QStringLiteral(":nome"), usuario.nome());
    consulta.bindValue(QStringLiteral(":email"), usuario.email());
    consulta.bindValue(QStringLiteral(":senha_hash"), usuario.senhaHash());
    consulta.bindValue(QStringLiteral(":papel"), paraTexto(usuario.papel()));

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    usuario.definirId(consulta.lastInsertId().toLongLong());
    m_ultimoErro.clear();
    return true;
}

bool RepositorioUsuario::atualizar(const Usuario& usuario)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "UPDATE usuario SET nome = :nome, email = :email, senha_hash = :senha_hash, "
        "papel = :papel WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":nome"), usuario.nome());
    consulta.bindValue(QStringLiteral(":email"), usuario.email());
    consulta.bindValue(QStringLiteral(":senha_hash"), usuario.senhaHash());
    consulta.bindValue(QStringLiteral(":papel"), paraTexto(usuario.papel()));
    consulta.bindValue(QStringLiteral(":id"), usuario.id());

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

bool RepositorioUsuario::remover(qint64 id)
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("DELETE FROM usuario WHERE id = :id"));
    consulta.bindValue(QStringLiteral(":id"), id);

    if (!consulta.exec())
    {
        m_ultimoErro = consulta.lastError().text();
        return false;
    }

    m_ultimoErro.clear();
    return true;
}

std::optional<Usuario> RepositorioUsuario::buscarPorId(qint64 id) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "SELECT id, nome, email, senha_hash, papel FROM usuario WHERE id = :id"));
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
    return montarUsuario(consulta);
}

std::optional<Usuario> RepositorioUsuario::buscarPorEmail(const QString& email) const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral(
        "SELECT id, nome, email, senha_hash, papel FROM usuario "
        "WHERE LOWER(email) = LOWER(:email)"));
    consulta.bindValue(QStringLiteral(":email"), email.trimmed());

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
    return montarUsuario(consulta);
}

QVector<Usuario> RepositorioUsuario::listarTodos() const
{
    QVector<Usuario> usuarios;
    QSqlQuery consulta(BancoDeDados::instancia().conexao());

    if (!consulta.exec(QStringLiteral(
            "SELECT id, nome, email, senha_hash, papel FROM usuario ORDER BY nome")))
    {
        m_ultimoErro = consulta.lastError().text();
        return usuarios;
    }

    while (consulta.next())
    {
        usuarios.append(montarUsuario(consulta));
    }

    m_ultimoErro.clear();
    return usuarios;
}

int RepositorioUsuario::contarAdministradores() const
{
    QSqlQuery consulta(BancoDeDados::instancia().conexao());
    consulta.prepare(QStringLiteral("SELECT COUNT(*) FROM usuario WHERE papel = :papel"));
    consulta.bindValue(QStringLiteral(":papel"), paraTexto(PapelUsuario::Administrador));

    if (!consulta.exec() || !consulta.next())
    {
        m_ultimoErro = consulta.lastError().text();
        return -1;
    }

    m_ultimoErro.clear();
    return consulta.value(0).toInt();
}

QString RepositorioUsuario::ultimoErro() const
{
    return m_ultimoErro;
}

}
