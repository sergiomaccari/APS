#pragma once

// Orquestracao do motor de analise.
//
// Monta o MotorAnalise com as regras ativas do banco, gera a recomendacao do
// ativo e cuida do ciclo de vida: a recomendacao nova nasce Gerada, e publicada
// como Vigente e as anteriores do mesmo ativo sao Invalidadas.

#include <optional>

#include <QString>
#include <QVector>

#include "dominio/recomendacao.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocotacao.h"
#include "persistencia/repositoriorecomendacao.h"
#include "persistencia/repositorioregra.h"

namespace analisador
{

class ServicoRecomendacao
{
public:
    ServicoRecomendacao(RepositorioRecomendacao& repositorioRecomendacao,
                        RepositorioCotacao& repositorioCotacao,
                        RepositorioAtivo& repositorioAtivo,
                        RepositorioRegra& repositorioRegra);

    // Gera, publica e persiste a recomendacao do ativo.
    std::optional<Recomendacao> gerarParaAtivo(qint64 ativoId);

    // Gera para todos os ativos cadastrados; devolve as recomendacoes publicadas.
    QVector<Recomendacao> gerarParaTodos();

    QVector<Recomendacao> listarVigentes() const;

    std::optional<Recomendacao> vigenteDoAtivo(qint64 ativoId) const;

    // Pareceres regra por regra, sem persistir - alimenta o detalhamento da tela.
    QVector<Recomendacao> detalharPorRegra(qint64 ativoId) const;

    // Quantidade minima de cotacoes para o motor conseguir opinar tecnicamente.
    static const int COTACOES_MINIMAS;

    QString ultimoErro() const;

private:
    RepositorioRecomendacao& m_repositorioRecomendacao;
    RepositorioCotacao& m_repositorioCotacao;
    RepositorioAtivo& m_repositorioAtivo;
    RepositorioRegra& m_repositorioRegra;
    QString m_ultimoErro;
};

}
