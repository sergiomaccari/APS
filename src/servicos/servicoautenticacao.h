#pragma once

// Regras de negocio de autenticacao e de gestao de usuarios.
//
// A senha nunca e armazenada em texto puro: guarda-se o SHA-256 de (sal + senha),
// com sal fixo documentado. O mesmo esquema e usado no script de dados de
// semente (recursos/sql/002_dados_semente.sql).

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/papelusuario.h"
#include "dominio/usuario.h"
#include "persistencia/repositoriousuario.h"

namespace analisador
{

class ServicoAutenticacao
{
public:
    // Sal fixo da aplicacao. Em um sistema de producao seria por usuario;
    // aqui e fixo para manter o script de semente reproduzivel.
    static const QString SAL;

    // Tamanho minimo aceito para a senha.
    static const int TAMANHO_MINIMO_SENHA;

    explicit ServicoAutenticacao(RepositorioUsuario& repositorio);

    // Devolve o usuario quando e-mail e senha conferem; std::nullopt caso contrario
    // (a mensagem fica em ultimoErro(), sem revelar qual dos dois campos falhou).
    std::optional<Usuario> autenticar(const QString& email, const QString& senha);

    // SHA-256 de (SAL + senha) em hexadecimal minusculo.
    static QString gerarHash(const QString& senha);

    // Cadastra um usuario novo. Valida nome, formato e unicidade do e-mail e
    // tamanho minimo da senha. Quando informado, criado recebe o usuario gravado.
    bool cadastrarUsuario(const QString& nome,
                          const QString& email,
                          const QString& senha,
                          PapelUsuario papel,
                          Usuario* criado = nullptr);

    // Atualiza nome, e-mail e papel. Nao altera a senha.
    bool atualizarUsuario(const Usuario& usuario);

    bool alterarSenha(qint64 usuarioId, const QString& novaSenha);

    // Impede a remocao do ultimo administrador do sistema.
    bool removerUsuario(qint64 usuarioId);

    QVector<Usuario> listarUsuarios() const;

    QString ultimoErro() const;

private:
    static bool emailPareceValido(const QString& email);

    RepositorioUsuario& m_repositorio;
    QString m_ultimoErro;
};

}
