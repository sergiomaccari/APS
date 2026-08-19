#pragma once

// Fachada de acesso ao banco SQLite (padrao Singleton).
//
// Responsabilidades: abrir a conexao, habilitar integridade referencial,
// aplicar as migracoes versionadas embutidas nos recursos Qt e oferecer
// controle explicito de transacao para as operacoes que precisam ser atomicas
// (a importacao de cotacoes, principalmente).

#include <QSqlDatabase>
#include <QString>
#include <QStringList>

namespace analisador
{

class BancoDeDados
{
public:
    // Instancia unica compartilhada pela aplicacao.
    static BancoDeDados& instancia();

    // Abre (ou cria) o arquivo de banco no caminho informado.
    // Aplica PRAGMA foreign_keys = ON. Devolve false em caso de falha.
    bool abrir(const QString& caminho);

    // Fecha a conexao, se estiver aberta.
    void fechar();

    bool estaAberto() const;
    QString caminhoBanco() const;

    // Conexao viva para os repositorios montarem suas consultas.
    QSqlDatabase& conexao();

    // Aplica as migracoes ainda nao registradas em schema_versao.
    // Idempotente: pode ser chamada a cada inicializacao.
    bool executarMigracoes();

    // Versao de esquema atualmente aplicada (0 quando nenhuma).
    int versaoAplicada() const;

    bool iniciarTransacao();
    bool confirmar();
    bool desfazer();

    QString ultimoErro() const;

private:
    BancoDeDados();
    ~BancoDeDados();

    BancoDeDados(const BancoDeDados&) = delete;
    BancoDeDados& operator=(const BancoDeDados&) = delete;

    // Cria a tabela de controle de versao de esquema, se necessario.
    bool garantirTabelaDeVersao();

    // Le um script do recurso qrc e o divide em comandos executaveis,
    // descartando comentarios ("--" ate o fim da linha) e comandos vazios.
    // O ponto-e-virgula so separa comandos quando esta fora de literal de texto.
    static QStringList comandosDoRecurso(const QString& caminhoRecurso, QString* erro);

    // Executa um script inteiro; espera-se estar dentro de uma transacao.
    bool aplicarScript(const QString& caminhoRecurso);

    // Registra a versao aplicada em schema_versao.
    bool registrarVersao(int versao, const QString& descricao);

    static const QString NOME_CONEXAO;

    QSqlDatabase m_conexao;
    QString m_caminhoBanco;
    QString m_ultimoErro;
};

}
