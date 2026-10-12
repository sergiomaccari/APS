#pragma once

// Repositorio da tabela "usuario" (padrao Repository): converte objetos Usuario
// em linhas do banco e vice-versa, isolando o restante do sistema do SQL.
//
// Todas as consultas usam prepare() + bindValue(): nenhuma string de SQL e
// montada por concatenacao com dados do usuario.

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/usuario.h"

QT_FORWARD_DECLARE_CLASS(QSqlQuery)

namespace analisador
{

class RepositorioUsuario
{
public:
    explicit RepositorioUsuario();

    // Insere o usuario e preenche o id gerado pelo banco.
    bool salvar(Usuario& usuario);

    // Atualiza nome, e-mail, senha e papel do usuario ja persistido.
    bool atualizar(const Usuario& usuario);

    bool remover(qint64 id);

    std::optional<Usuario> buscarPorId(qint64 id) const;

    // Busca pelo e-mail (chave natural usada no login).
    std::optional<Usuario> buscarPorEmail(const QString& email) const;

    // Todos os usuarios em ordem alfabetica de nome.
    QVector<Usuario> listarTodos() const;

    // Quantidade de administradores cadastrados; devolve -1 quando a consulta falha.
    int contarAdministradores() const;

    // Mensagem do ultimo erro de banco ocorrido neste repositorio.
    QString ultimoErro() const;

private:
    // Monta um Usuario a partir da linha corrente da consulta.
    // Ordem esperada das colunas: id, nome, email, senha_hash, papel.
    static Usuario montarUsuario(const QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
