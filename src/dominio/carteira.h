#pragma once

// Carteira de investimentos de um usuario. Agrega objetos Posicao (composicao).

#include <QDate>
#include <QHash>
#include <QString>
#include <QVector>

#include "dominio/posicao.h"

namespace analisador
{

class Carteira
{
public:
    Carteira();
    Carteira(qint64 id, qint64 usuarioId, const QString& nome, const QDate& criadaEm);

    qint64 id() const;
    qint64 usuarioId() const;
    QString nome() const;
    QDate criadaEm() const;
    const QVector<Posicao>& posicoes() const;

    void definirId(qint64 id);
    void definirUsuarioId(qint64 usuarioId);
    void definirNome(const QString& nome);
    void definirCriadaEm(const QDate& criadaEm);
    void definirPosicoes(const QVector<Posicao>& posicoes);

    // Acrescenta a posicao. Se ja existir posicao do mesmo ativo, incorpora a compra
    // recalculando o preco medio ponderado.
    void adicionarPosicao(const Posicao& posicao);

    // Remove a posicao pelo identificador. Retorna false quando nao encontra.
    bool removerPosicao(qint64 posicaoId);

    // Remove a posicao do ativo informado. Retorna false quando nao encontra.
    bool removerPosicaoDoAtivo(qint64 ativoId);

    // Ponteiro para a posicao do ativo ou nullptr quando a carteira nao possui o ativo.
    Posicao* posicaoDoAtivo(qint64 ativoId);
    const Posicao* posicaoDoAtivo(qint64 ativoId) const;

    // Quantidade de ativos diferentes na carteira.
    int quantidadeDeAtivos() const;

    // Verdadeiro quando nao ha nenhuma posicao cadastrada.
    bool vazia() const;

    // Soma do capital investido em todas as posicoes.
    double custoTotal() const;

    // Soma do valor de mercado das posicoes, dado o mapa ativoId -> preco atual.
    // Ativos ausentes no mapa sao avaliados pelo proprio preco medio.
    double valorDeMercado(const QHash<qint64, double>& precosAtuais) const;

    // Lucro (positivo) ou prejuizo (negativo) total em reais.
    double lucroPrejuizo(const QHash<qint64, double>& precosAtuais) const;

    // Rentabilidade percentual da carteira sobre o custo total.
    double rentabilidadePercentual(const QHash<qint64, double>& precosAtuais) const;

private:
    qint64 m_id = 0;
    qint64 m_usuarioId = 0;
    QString m_nome;
    QDate m_criadaEm;
    QVector<Posicao> m_posicoes;
};

}
