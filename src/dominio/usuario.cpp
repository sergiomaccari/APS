#include "dominio/usuario.h"

namespace analisador
{

Usuario::Usuario()
{
}

Usuario::Usuario(qint64 id,
                 const QString& nome,
                 const QString& email,
                 const QString& senhaHash,
                 PapelUsuario papel)
    : m_id(id)
    , m_nome(nome)
    , m_email(email)
    , m_senhaHash(senhaHash)
    , m_papel(papel)
{
}

qint64 Usuario::id() const
{
    return m_id;
}

QString Usuario::nome() const
{
    return m_nome;
}

QString Usuario::email() const
{
    return m_email;
}

QString Usuario::senhaHash() const
{
    return m_senhaHash;
}

PapelUsuario Usuario::papel() const
{
    return m_papel;
}

void Usuario::definirId(qint64 id)
{
    m_id = id;
}

void Usuario::definirNome(const QString& nome)
{
    m_nome = nome;
}

void Usuario::definirEmail(const QString& email)
{
    m_email = email.trimmed().toLower();
}

void Usuario::definirSenhaHash(const QString& senhaHash)
{
    m_senhaHash = senhaHash;
}

void Usuario::definirPapel(PapelUsuario papel)
{
    m_papel = papel;
}

bool Usuario::ehAdministrador() const
{
    return m_papel == PapelUsuario::Administrador;
}

bool Usuario::valido() const
{
    if (m_nome.trimmed().isEmpty())
    {
        return false;
    }

    const QString email = m_email.trimmed();
    if (email.isEmpty())
    {
        return false;
    }

    const int posicaoArroba = email.indexOf(QLatin1Char('@'));
    if (posicaoArroba <= 0 || posicaoArroba == email.size() - 1)
    {
        return false;
    }

    return email.indexOf(QLatin1Char('.'), posicaoArroba) > posicaoArroba + 1;
}

QString Usuario::rotulo() const
{
    return QStringLiteral("%1 (%2)").arg(m_nome, paraTexto(m_papel));
}

}
