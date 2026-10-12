#pragma once

// Sincronizacao online das cotacoes (RF020) - complemento da importacao CSV.
//
// Para cada ativo cadastrado o servico calcula a JANELA FALTANTE (do dia
// seguinte a ultima cotacao local ate hoje), pede so esse intervalo a fonte
// remota e grava o resultado de forma transacional, reaproveitando o mesmo
// INSERT OR IGNORE da importacao de arquivo - portanto clicar duas vezes nao
// duplica nada.
//
// Regras de robustez (RNF008): a falha de um ativo NUNCA interrompe os demais;
// cada ativo vira uma linha de mensagem e, quando houve leitura ou erro, um
// registro na tabela "importacao", que assim guarda a auditoria unica de CSV e
// de sincronizacao online.

#include <optional>

#include <QDate>
#include <QString>
#include <QStringList>
#include <QVector>

#include "dominio/cotacao.h"
#include "persistencia/repositorioativo.h"
#include "persistencia/repositoriocotacao.h"
#include "persistencia/repositorioimportacao.h"
#include "servicos/fontecotacoesremota.h"

namespace analisador
{

struct ResultadoSincronizacao
{
    int ativosProcessados = 0;
    int ativosComFalha = 0;
    int cotacoesInseridas = 0;
    int cotacoesIgnoradas = 0;

    // Uma linha por ativo: "PETR4: 12 novas, 0 ignoradas".
    QStringList mensagens;

    // Resumo pronto para exibir na tela de importacao.
    QString resumo() const;
};

class ServicoSincronizacao
{
public:
    // Quanto historico buscar quando o ativo ainda nao tem nenhuma cotacao local.
    static const int JANELA_INICIAL_DIAS;

    ServicoSincronizacao(RepositorioAtivo& repositorioAtivo,
                         RepositorioCotacao& repositorioCotacao,
                         RepositorioImportacao& repositorioImportacao,
                         FonteCotacoesRemota& fonte);

    // Percorre todos os ativos cadastrados e atualiza o que estiver faltando.
    ResultadoSincronizacao sincronizarTodos();

    // Primeiro dia a pedir para a fonte remota: dia seguinte a ultima cotacao
    // local ou, quando nao ha nenhuma, JANELA_INICIAL_DIAS antes de hoje.
    // Publica e pura para poder ser testada sem rede e sem banco.
    static QDate inicioDaJanela(const std::optional<Cotacao>& ultima, const QDate& hoje);

private:
    // Grava as cotacoes de um ativo em transacao unica e registra a auditoria.
    // Devolve false quando nada foi gravado, descrevendo o problema em *erro.
    bool gravar(qint64 ativoId,
                const QString& ticker,
                const QVector<Cotacao>& cotacoes,
                int* inseridas,
                QString* erro);

    // Registra a tentativa na tabela "importacao", seguindo a mesma maquina de
    // estados usada pelo ServicoImportacao: Concluida quando motivoDaRejeicao
    // vem vazio, Rejeitada caso contrario. Devolve false quando a gravacao do
    // proprio registro falha.
    bool registrarAuditoria(qint64 ativoId,
                            const QString& ticker,
                            int lidas,
                            int inseridas,
                            const QString& motivoDaRejeicao);

    RepositorioAtivo& m_repositorioAtivo;
    RepositorioCotacao& m_repositorioCotacao;
    RepositorioImportacao& m_repositorioImportacao;
    FonteCotacoesRemota& m_fonte;
};

}
