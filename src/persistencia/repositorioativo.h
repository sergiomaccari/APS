#pragma once

// Repositorio da tabela "ativo" (padrao Repository).
//
// A tabela guarda a hierarquia Ativo -> (Acao | FundoImobiliario) em uma unica
// relacao, com a coluna "tipo" como discriminador ('ACAO' ou 'FII'). Por isso o
// repositorio devolve std::unique_ptr<Ativo>: o objeto concreto e escolhido em
// tempo de execucao por montarAtivo(), e o restante do sistema trabalha apenas
// com a interface polimorfica de Ativo.

#include <memory>
#include <vector>

#include <QString>

#include "dominio/ativo.h"

class QSqlQuery;

namespace analisador
{

class RepositorioAtivo
{
public:
    RepositorioAtivo();

    // Insere o ativo (gravando 'ACAO' ou 'FII' conforme a subclasse concreta)
    // e preenche o id gerado pelo banco.
    bool salvar(Ativo& ativo);

    bool atualizar(const Ativo& ativo);
    bool remover(qint64 id);

    std::unique_ptr<Ativo> buscarPorId(qint64 id) const;
    std::unique_ptr<Ativo> buscarPorTicker(const QString& ticker) const;

    // Todos os ativos, em ordem de ticker.
    std::vector<std::unique_ptr<Ativo>> listarTodos() const;

    // Busca parcial (LIKE) por ticker OU nome da empresa.
    std::vector<std::unique_ptr<Ativo>> buscar(const QString& termo) const;

    // Verdadeiro quando o ticker ja pertence a outro ativo. O parametro
    // idIgnorado permite editar um ativo sem que ele colida consigo mesmo.
    bool tickerEmUso(const QString& ticker, qint64 idIgnorado = 0) const;

    QString ultimoErro() const;

private:
    // Colunas selecionadas por todas as consultas de leitura, na ordem esperada
    // por montarAtivo().
    static QString colunas();

    // Cria a subclasse concreta correspondente a coluna "tipo" da linha corrente.
    // Ordem das colunas: id, ticker, nome_empresa, setor, tipo, preco_lucro,
    // valor_mercado, dividend_yield, valor_patrimonial_por_cota, segmento.
    static std::unique_ptr<Ativo> montarAtivo(const QSqlQuery& consulta);

    // Vincula os campos comuns e os especificos do tipo, gravando NULL nas
    // colunas que nao pertencem a subclasse do ativo informado.
    static void vincularCampos(QSqlQuery& consulta, const Ativo& ativo);

    // Le todas as linhas ja executadas da consulta.
    static std::vector<std::unique_ptr<Ativo>> lerTodos(QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
