#pragma once

// Repositorio da tabela "cotacao" (serie historica diaria OHLCV).
//
// O indice UNIQUE (ativo_id, data) garante que nao existam dois candles do
// mesmo ativo no mesmo dia; inserirEmLote() se apoia nele com INSERT OR IGNORE
// para tornar a importacao de CSV idempotente.

#include <optional>

#include <QDate>
#include <QString>
#include <QVector>

#include "dominio/cotacao.h"

QT_FORWARD_DECLARE_CLASS(QSqlQuery)

namespace analisador
{

class RepositorioCotacao
{
public:
    RepositorioCotacao();

    // Insere uma cotacao e preenche o id gerado pelo banco.
    bool salvar(Cotacao& cotacao);

    // Insere varias cotacoes com INSERT OR IGNORE e devolve quantas foram de
    // fato gravadas (as duplicadas sao silenciosamente ignoradas).
    // A transacao NAO e aberta aqui: quem chama controla a atomicidade.
    int inserirEmLote(const QVector<Cotacao>& cotacoes);

    // Serie do ativo em ordem crescente de data. Com limite maior que zero,
    // devolve apenas as ultimas N cotacoes (ainda em ordem crescente).
    QVector<Cotacao> listarPorAtivo(qint64 ativoId, int limite = 0) const;

    // Cotacao mais recente do ativo.
    std::optional<Cotacao> buscarUltima(qint64 ativoId) const;

    // Verdadeiro quando ja existe candle do ativo naquela data.
    bool existe(qint64 ativoId, const QDate& data) const;

    // Quantidade de cotacoes do ativo; devolve -1 quando a consulta falha.
    int contarPorAtivo(qint64 ativoId) const;

    // Apaga toda a serie historica do ativo.
    bool removerPorAtivo(qint64 ativoId);

    QString ultimoErro() const;

private:
    // Colunas na ordem esperada por montarCotacao().
    static QString colunas();

    // Monta uma Cotacao a partir da linha corrente da consulta.
    static Cotacao montarCotacao(const QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
