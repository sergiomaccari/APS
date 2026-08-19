#pragma once

// Repositorio das tabelas "carteira" e "posicao" (padrao Repository).
//
// Carteira e Posicao formam uma composicao no dominio, por isso as duas tabelas
// sao tratadas pelo mesmo repositorio: toda carteira lida ja vem com suas
// posicoes carregadas, e o indice UNIQUE (carteira_id, ativo_id) e respeitado
// por salvarPosicao(), que atualiza a posicao existente em vez de duplicar.

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/carteira.h"
#include "dominio/posicao.h"

class QSqlQuery;

namespace analisador
{

class RepositorioCarteira
{
public:
    RepositorioCarteira();

    // Insere a carteira (sem as posicoes) e preenche o id gerado pelo banco.
    // As posicoes sao gravadas uma a uma por salvarPosicao().
    bool salvar(Carteira& carteira);

    bool atualizar(const Carteira& carteira);

    // Remove a carteira; as posicoes caem junto por ON DELETE CASCADE.
    bool remover(qint64 id);

    // Carteira com as posicoes ja carregadas.
    std::optional<Carteira> buscarPorId(qint64 id) const;

    // Carteira principal do usuario: a mais antiga que ele possui.
    std::optional<Carteira> buscarPrincipalDoUsuario(qint64 usuarioId) const;

    // Todas as carteiras do usuario, cada uma com suas posicoes.
    QVector<Carteira> listarPorUsuario(qint64 usuarioId) const;

    // Grava a posicao: quando o par (carteira, ativo) ja existe, atualiza
    // quantidade e preco medio; caso contrario insere e preenche o id gerado.
    bool salvarPosicao(Posicao& posicao);

    bool atualizarPosicao(const Posicao& posicao);
    bool removerPosicao(qint64 posicaoId);

    // Posicoes da carteira, em ordem de ativo.
    QVector<Posicao> listarPosicoes(qint64 carteiraId) const;

    std::optional<Posicao> buscarPosicao(qint64 carteiraId, qint64 ativoId) const;

    QString ultimoErro() const;

private:
    // Colunas na ordem esperada por montarCarteira(): id, usuario_id, nome, criada_em.
    static QString colunasCarteira();

    // Colunas na ordem esperada por montarPosicao(): id, carteira_id, ativo_id,
    // quantidade, preco_medio, comprada_em.
    static QString colunasPosicao();

    static Carteira montarCarteira(const QSqlQuery& consulta);
    static Posicao montarPosicao(const QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
