#pragma once

// Importacao de cotacoes a partir de arquivos CSV no padrao brasileiro.
//
// Formato esperado (cabecalho obrigatorio):
//     Data;Abertura;Maxima;Minima;Fechamento;Volume
//     14/08/2026;38,42;38,90;38,10;38,75;41250300
//
// A importacao e TRANSACIONAL: todo o arquivo e lido e validado em memoria
// antes de qualquer escrita. Havendo um unico erro estrutural, nada e gravado
// e a importacao fica registrada no estado Rejeitada. Cotacoes repetidas
// (mesmo ativo e mesma data) sao ignoradas, o que torna a operacao idempotente.

#include <optional>

#include <QDate>
#include <QString>
#include <QStringList>
#include <QVector>

#include <memory>

#include "dominio/cotacao.h"
#include "dominio/importacao.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocotacao.h"
#include "persistencia/repositorioimportacao.h"

namespace analisador
{

struct ResultadoImportacao
{
    bool sucesso = false;
    int linhasLidas = 0;
    int linhasInseridas = 0;
    int linhasIgnoradas = 0;
    QStringList erros;

    // Resumo pronto para exibir na tela de importacao.
    QString resumo() const;
};

class ServicoImportacao
{
public:
    // Colunas esperadas no arquivo.
    static const int COLUNAS_ESPERADAS;

    // Separador de campos do padrao brasileiro exportado pela B3.
    static const QChar SEPARADOR;

    ServicoImportacao(RepositorioCotacao& repositorioCotacao,
                      RepositorioAtivo& repositorioAtivo,
                      RepositorioImportacao& repositorioImportacao);

    // Importa um arquivo para o ativo informado.
    ResultadoImportacao importarCsv(const QString& caminho, qint64 ativoId);

    // Importa todos os arquivos TICKER.csv de um diretorio, casando o nome do
    // arquivo com o ticker cadastrado. Arquivos sem ativo correspondente sao
    // relatados como erro, sem interromper os demais.
    ResultadoImportacao importarDiretorio(const QString& caminho);

    QVector<Importacao> historico(int limite = 20) const;

    QString ultimoErro() const;

    // Converte um numero no padrao brasileiro ("1.234,56" ou "38,42") para double.
    static std::optional<double> converterNumero(const QString& texto);

private:
    // Converte uma linha do arquivo em Cotacao. Em caso de problema devolve
    // std::nullopt e descreve o erro (com o numero da linha) em erro.
    static std::optional<Cotacao> converterLinha(const QString& linha,
                                                 qint64 ativoId,
                                                 int numeroLinha,
                                                 QString* erro);

    // Verifica se o cabecalho tem as seis colunas esperadas.
    static bool cabecalhoValido(const QString& linha);

    RepositorioCotacao& m_repositorioCotacao;
    RepositorioAtivo& m_repositorioAtivo;
    RepositorioImportacao& m_repositorioImportacao;
    QString m_ultimoErro;
};

}
