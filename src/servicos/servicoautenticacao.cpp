#include "servicos/servicoautenticacao.h"

#include <QCryptographicHash>
#include <QRegularExpression>

namespace analisador
{

const QString ServicoAutenticacao::SAL = QStringLiteral("aps-b3-2026");
const int ServicoAutenticacao::TAMANHO_MINIMO_SENHA = 6;

ServicoAutenticacao::ServicoAutenticacao(RepositorioUsuario& repositorio)
    : m_repositorio(repositorio)
{
}

QString ServicoAutenticacao::gerarHash(const QString& senha)
{
    const QByteArray entrada = (SAL + senha).toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(entrada, QCryptographicHash::Sha256).toHex());
}

bool ServicoAutenticacao::emailPareceValido(const QString& email)
{
    static const QRegularExpression padrao(QStringLiteral("^[^@\\s]+@[^@\\s]+\\.[A-Za-z]{2,}$"));
    return padrao.match(email.trimmed()).hasMatch();
}

std::optional<Usuario> ServicoAutenticacao::autenticar(const QString& email, const QString& senha)
{
    m_ultimoErro.clear();
    const QString emailNormalizado = email.trimmed().toLower();
    if (emailNormalizado.isEmpty() || senha.isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe e-mail e senha.");
        return std::nullopt;
    }

    const std::optional<Usuario> encontrado = m_repositorio.buscarPorEmail(emailNormalizado);
    if (!encontrado.has_value())
    {
        // Mensagem generica de proposito: nao revela se o e-mail existe.
        m_ultimoErro = QString::fromUtf8("E-mail ou senha inválidos.");
        return std::nullopt;
    }

    if (encontrado.value().senhaHash() != gerarHash(senha))
    {
        m_ultimoErro = QString::fromUtf8("E-mail ou senha inválidos.");
        return std::nullopt;
    }
    return encontrado;
}

bool ServicoAutenticacao::cadastrarUsuario(const QString& nome,
                                           const QString& email,
                                           const QString& senha,
                                           PapelUsuario papel,
                                           Usuario* criado)
{
    m_ultimoErro.clear();
    const QString nomeNormalizado = nome.trimmed();
    const QString emailNormalizado = email.trimmed().toLower();

    if (nomeNormalizado.isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o nome do usuário.");
        return false;
    }
    if (!emailPareceValido(emailNormalizado))
    {
        m_ultimoErro = QString::fromUtf8("Informe um e-mail válido.");
        return false;
    }
    if (senha.size() < TAMANHO_MINIMO_SENHA)
    {
        m_ultimoErro = QString::fromUtf8("A senha deve ter ao menos %1 caracteres.")
                           .arg(TAMANHO_MINIMO_SENHA);
        return false;
    }
    if (m_repositorio.buscarPorEmail(emailNormalizado).has_value())
    {
        m_ultimoErro = QString::fromUtf8("Já existe um usuário com o e-mail %1.").arg(emailNormalizado);
        return false;
    }

    Usuario usuario;
    usuario.definirNome(nomeNormalizado);
    usuario.definirEmail(emailNormalizado);
    usuario.definirSenhaHash(gerarHash(senha));
    usuario.definirPapel(papel);

    if (!m_repositorio.salvar(usuario))
    {
        m_ultimoErro = m_repositorio.ultimoErro();
        return false;
    }
    if (criado != nullptr)
    {
        *criado = usuario;
    }
    return true;
}

bool ServicoAutenticacao::atualizarUsuario(const Usuario& usuario)
{
    m_ultimoErro.clear();
    if (usuario.id() <= 0)
    {
        m_ultimoErro = QString::fromUtf8("Usuário sem identificador não pode ser atualizado.");
        return false;
    }
    if (usuario.nome().trimmed().isEmpty())
    {
        m_ultimoErro = QString::fromUtf8("Informe o nome do usuário.");
        return false;
    }
    if (!emailPareceValido(usuario.email()))
    {
        m_ultimoErro = QString::fromUtf8("Informe um e-mail válido.");
        return false;
    }

    const std::optional<Usuario> mesmoEmail = m_repositorio.buscarPorEmail(usuario.email().trimmed().toLower());
    if (mesmoEmail.has_value() && mesmoEmail.value().id() != usuario.id())
    {
        m_ultimoErro = QString::fromUtf8("O e-mail %1 já pertence a outro usuário.").arg(usuario.email());
        return false;
    }

    // Rebaixar o ultimo administrador deixaria o sistema sem administracao.
    const std::optional<Usuario> atual = m_repositorio.buscarPorId(usuario.id());
    if (atual.has_value() && atual.value().ehAdministrador() && !usuario.ehAdministrador()
        && m_repositorio.contarAdministradores() <= 1)
    {
        m_ultimoErro = QString::fromUtf8("Este é o único administrador: o papel não pode ser alterado.");
        return false;
    }

    if (!m_repositorio.atualizar(usuario))
    {
        m_ultimoErro = m_repositorio.ultimoErro();
        return false;
    }
    return true;
}

bool ServicoAutenticacao::alterarSenha(qint64 usuarioId, const QString& novaSenha)
{
    m_ultimoErro.clear();
    if (novaSenha.size() < TAMANHO_MINIMO_SENHA)
    {
        m_ultimoErro = QString::fromUtf8("A senha deve ter ao menos %1 caracteres.")
                           .arg(TAMANHO_MINIMO_SENHA);
        return false;
    }

    std::optional<Usuario> encontrado = m_repositorio.buscarPorId(usuarioId);
    if (!encontrado.has_value())
    {
        m_ultimoErro = QString::fromUtf8("Usuário não encontrado.");
        return false;
    }

    Usuario usuario = encontrado.value();
    usuario.definirSenhaHash(gerarHash(novaSenha));
    if (!m_repositorio.atualizar(usuario))
    {
        m_ultimoErro = m_repositorio.ultimoErro();
        return false;
    }
    return true;
}

bool ServicoAutenticacao::removerUsuario(qint64 usuarioId)
{
    m_ultimoErro.clear();
    const std::optional<Usuario> encontrado = m_repositorio.buscarPorId(usuarioId);
    if (!encontrado.has_value())
    {
        m_ultimoErro = QString::fromUtf8("Usuário não encontrado.");
        return false;
    }
    if (encontrado.value().ehAdministrador() && m_repositorio.contarAdministradores() <= 1)
    {
        m_ultimoErro = QString::fromUtf8("O último administrador do sistema não pode ser removido.");
        return false;
    }
    if (!m_repositorio.remover(usuarioId))
    {
        m_ultimoErro = m_repositorio.ultimoErro();
        return false;
    }
    return true;
}

QVector<Usuario> ServicoAutenticacao::listarUsuarios() const
{
    return m_repositorio.listarTodos();
}

QString ServicoAutenticacao::ultimoErro() const
{
    return m_ultimoErro;
}

}
