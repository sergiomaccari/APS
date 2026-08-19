#pragma once

// Repositorio da tabela "recomendacao" (padrao Repository).
//
// Guarda a saida do MotorAnalise. As colunas tipo e estado recebem exatamente
// os textos de paraTexto(TipoRecomendacao) e paraTexto(EstadoRecomendacao),
// de modo que a maquina de estados do dominio continue valida no banco.

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/recomendacao.h"

class QSqlQuery;

namespace analisador
{

class RepositorioRecomendacao
{
public:
    RepositorioRecomendacao();

    // Insere a recomendacao e preenche o id gerado pelo banco.
    bool salvar(Recomendacao& recomendacao);

    bool atualizar(const Recomendacao& recomendacao);

    // Recomendacao publicada (estado 'Vigente') mais recente do ativo.
    std::optional<Recomendacao> buscarVigenteDoAtivo(qint64 ativoId) const;

    // Todas as recomendacoes vigentes, das mais recentes para as mais antigas.
    QVector<Recomendacao> listarVigentes() const;

    // Historico do ativo, das mais recentes para as mais antigas.
    // Com limite maior que zero devolve apenas as N primeiras.
    QVector<Recomendacao> listarPorAtivo(qint64 ativoId, int limite = 0) const;

    // Invalida as recomendacoes do ativo que ainda podem mudar de estado
    // ('Gerada' ou 'Vigente'), como manda a maquina de estados do dominio.
    // Devolve quantas linhas foram alteradas, ou -1 quando a atualizacao falha.
    int invalidarVigentesDoAtivo(qint64 ativoId) const;

    QString ultimoErro() const;

private:
    // Colunas na ordem esperada por montarRecomendacao(): id, ativo_id, tipo,
    // justificativa, regra_aplicada, gerada_em, estado, pontuacao.
    static QString colunas();

    static Recomendacao montarRecomendacao(const QSqlQuery& consulta);

    // Le todas as linhas de uma consulta ja executada.
    static QVector<Recomendacao> lerTodas(QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
