#pragma once

// Usuario do sistema: quem faz login, mantem carteiras e cadastra alertas.

#include <QString>

#include "dominio/papelusuario.h"

namespace analisador
{

class Usuario
{
public:
    Usuario();
    Usuario(qint64 id,
            const QString& nome,
            const QString& email,
            const QString& senhaHash,
            PapelUsuario papel);

    qint64 id() const;
    QString nome() const;
    QString email() const;
    QString senhaHash() const;
    PapelUsuario papel() const;

    void definirId(qint64 id);
    void definirNome(const QString& nome);
    void definirEmail(const QString& email);
    void definirSenhaHash(const QString& senhaHash);
    void definirPapel(PapelUsuario papel);

    // Verdadeiro quando o usuario pode administrar cadastros e regras.
    bool ehAdministrador() const;

    // Verdadeiro quando nome e e-mail estao preenchidos e o e-mail tem formato aceitavel.
    bool valido() const;

    // Rotulo curto para exibicao, no formato "Nome (Papel)".
    QString rotulo() const;

private:
    qint64 m_id = 0;
    QString m_nome;
    QString m_email;
    QString m_senhaHash;
    PapelUsuario m_papel = PapelUsuario::Investidor;
};

}
