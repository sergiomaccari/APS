#pragma once

// Dados fixos do protótipo de telas (entrega de 13/10, 1º bimestre).
//
// O alvo analisador-b3-prototipo não compila os .cpp reais de persistencia/,
// servicos/ e analise/: no lugar deles entram os stubs de src/prototipo/, que
// leem e escrevem neste estado em memória. Nada é gravado em disco; ao fechar o
// programa tudo volta à semente.
//
// A semente transcreve os dados de demonstração do sistema completo (usuários,
// ativos, carteira e regras), as cotações de dados/*.csv (embutidas em
// recursos/prototipo.qrc como :/dados/<TICKER>.csv) e o que o sistema completo
// produz em ferramentas/capturar-telas.cpp (importações, recomendações e
// alertas), de modo que as telas mostrem os mesmos dados das figuras do
// documento do 1º bimestre.
//
// Identificadores da semente:
//   usuários   1 admin@analisadorb3.com (Administrador)
//              2 investidor@analisadorb3.com (Investidor)
//   ativos     1 PETR4 · 2 VALE3 · 3 ITUB4 · 4 HGLG11 (FII) · 5 MGLU3
//   regras     1 Cruzamento de Medias Moveis · 2 Indice de Forca Relativa
//              3 Dividend Yield · 4 Preco sobre Lucro (todas ativas, peso 1,0)
//   carteira   1 "Carteira Principal" do investidor; posições 1 PETR4, 2 VALE3, 3 HGLG11
//   alertas    1 PETR4 acima de 97% do último fechamento (estado Ativo: dispara na
//              primeira avaliação) · 2 VALE3 abaixo de 90% (segue monitorando)
//   recomend.  1 HGLG11 · 2 ITUB4 · 3 MGLU3 · 4 PETR4 · 5 VALE3 (todas Vigentes)
//   importação 1 HGLG11 · 2 ITUB4 · 3 MGLU3 · 4 PETR4 · 5 VALE3 (Concluídas)
// Novos registros devem usar proximoId() (começa em 1000, acima de toda a semente).
//
// Ponteiros devolvidos pelas funções de consulta apontam para dentro dos
// vetores: valem até a próxima inserção ou remoção no vetor correspondente.

#include <memory>
#include <vector>

#include <QDateTime>
#include <QHash>
#include <QString>
#include <QVector>

#include "dominio/alerta.h"
#include "dominio/ativo.h"
#include "dominio/carteira.h"
#include "dominio/cotacao.h"
#include "dominio/importacao.h"
#include "dominio/recomendacao.h"
#include "dominio/regraconfigurada.h"
#include "dominio/usuario.h"

namespace analisador::prototipo
{

struct DadosFixos
{
    // Usuários em ordem de id. senhaHash() guarda o mesmo SHA-256 de
    // ("aps-b3-2026" + senha) da semente SQL; ServicoAutenticacao::gerarHash()
    // do stub deve calcular igual (QCryptographicHash, só Qt Core).
    QVector<Usuario> usuarios;

    // Ativos em ordem de id; cada elemento é um Acao ou FundoImobiliario.
    std::vector<std::unique_ptr<Ativo>> ativos;

    // Série histórica por ativoId, em ordem crescente de data (130 pregões por
    // ativo da semente, de 16/02/2026 a 14/08/2026).
    QHash<qint64, QVector<Cotacao>> cotacoes;

    // Regras de análise em ordem de id.
    QVector<RegraConfigurada> regras;

    // Carteiras (a semente tem só a do investidor, já com as 3 posições).
    QVector<Carteira> carteiras;

    // Alertas em ordem de id (todos do investidor na semente).
    QVector<Alerta> alertas;

    // Recomendações consolidadas vigentes, em ordem de id (= ordem de geração).
    // O repositório real lista as vigentes da mais recente para a mais antiga.
    QVector<Recomendacao> recomendacoes;

    // Pareceres individuais por ativoId (o que detalharPorRegra() devolve):
    // estado Gerada, id 0, na ordem de registro das regras. A regra IFR se
    // abstém nos 5 ativos e o FII não tem P/L, por isso há 2 ou 3 pareceres.
    QHash<qint64, QVector<Recomendacao>> pareceresPorRegra;

    // Histórico de importações em ordem de id. O real lista do mais recente
    // para o mais antigo.
    QVector<Importacao> importacoes;

    // Contador usado por proximoId().
    qint64 ultimoId = 999;
};

// Singleton de função: semeia tudo na primeira chamada (inclusive a leitura
// dos CSVs de :/dados) e devolve sempre a mesma instância.
DadosFixos& dados();

// Próximo identificador livre para qualquer entidade nova (1000, 1001, ...).
qint64 proximoId();

// Ativo pelo id ou nullptr.
Ativo* ativoPorId(qint64 id);

// Ativo pelo ticker (comparação sem diferenciar maiúsculas) ou nullptr.
Ativo* ativoPorTicker(const QString& ticker);

// Cópia polimórfica do ativo (Acao ou FundoImobiliario), para devolver
// std::unique_ptr<Ativo> sem expor o objeto interno.
std::unique_ptr<Ativo> clonarAtivo(const Ativo& ativo);

// Usuário pelo id ou nullptr.
Usuario* usuarioPorId(qint64 id);

// Usuário pelo e-mail (sem diferenciar maiúsculas) ou nullptr.
Usuario* usuarioPorEmail(const QString& email);

// Série do ativo em ordem crescente de data; vetor vazio quando não há cotações.
const QVector<Cotacao>& cotacoesDoAtivo(qint64 ativoId);

// Regra pelo id ou nullptr.
RegraConfigurada* regraPorId(qint64 id);

// Carteira pelo id ou nullptr.
Carteira* carteiraPorId(qint64 id);

// Primeira carteira do usuário (a "principal") ou nullptr.
Carteira* carteiraDoUsuario(qint64 usuarioId);

// Alerta pelo id ou nullptr.
Alerta* alertaPorId(qint64 id);

// Recomendação vigente do ativo ou nullptr.
Recomendacao* recomendacaoVigenteDoAtivo(qint64 ativoId);

// Pareceres regra a regra do ativo; vetor vazio quando o ativo não tem.
const QVector<Recomendacao>& pareceresDoAtivo(qint64 ativoId);

// Momento fixo em que a semente foi "gerada" (23/08/2026 12:20, o mesmo das
// figuras do documento): criação dos alertas e execução das importações.
QDateTime momentoDaSemente();

// Mensagem fixa do resumo de importação, para usar literalmente.
QString mensagemImportacaoFixa();

// Mensagem fixa da sincronização online, para usar literalmente.
QString mensagemSincronizacaoFixa();

}
