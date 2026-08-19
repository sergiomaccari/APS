# HANDOFF — retomada entre sessões

> Atualizado em **2026-08-18**. Leia junto com [`../MARCOS.md`](../MARCOS.md) e [`../LOG.md`](../LOG.md).

## Alerta ativo

- **Fase atual: PLANEJAMENTO.** A spec `docs/specs/0001-tema-e-escopo.md` está em
  `rascunho` — **implementação proibida** até o Sergio (e a equipe) aprovarem tema
  (ADR 0001) e stack (ADR 0002).
- `inicio.txt` é privado — está no `.gitignore` e **nunca** entra em commit/push.

## Pendências 🟦 (input humano)

1. Aprovar tema: A (Analisador B3, recomendado) / B (gestão p/ cliente real) / C (paper trading).
2. Aprovar stack: Qt 6 + Qt Charts + SQLite + CMake (C++ POO é restrição fixa, item 7 do briefing).
3. Definir o "cliente" da entrevista de requisitos.
4. Senha dos 14 PDFs de aula (`APS/Aulas/` — criptografados) ou cópias sem proteção.
5. Preferências de custo: subagentes pesados em **Opus**; avisar Sergio ao cruzar 50% do orçamento de tokens da sessão.

## Próximo item da implementação (loop autônomo — atualizar a cada iteração)

**Fase atual: EXECUÇÃO do sistema na branch `teste-de-desenvolvimento`.**
Ordem dos itens; marque ✅ ao concluir e commitar.

1. ✅ Domínio (`src/dominio/`) — 12 classes, com máquinas de estado.
2. ✅ Schema SQL (9 tabelas) + semente idempotente + `recursos.qrc`.
3. ✅ Análise (`src/analise/`) — indicadores, Strategy com 4 regras, MotorAnalise.
4. ✅ Persistência (`src/persistencia/`) — BancoDeDados + 8 repositórios.
5. ✅ Serviços (`src/servicos/`) — autenticação, ativos, importação transacional,
   carteira, alertas (Observer), recomendações.
6. ✅ `CMakeLists.txt` (núcleo + executável + testes no ctest) e CSVs em `dados/`.
7. ✅ **INTERFACE GRÁFICA** — concluída. Feito: `ui/contexto.h/.cpp`
   (injeção de dependência), `ui/widgets/cartaoindicador`, `ui/widgets/graficocandlestick`,
   `ui/widgets/graficolinha`, `ui/telalogin`, `ui/dialogos/dialogoativo`.
   Feito também: `ui/telaativos` (CRUD com busca, confirmação de remoção e contagem
   de cotações) e `ui/telaimportacao` (arquivo ou pasta, relatório de erros linha a
   linha e histórico de importações).
   Feito também: `ui/teladashboard` (seletor de ativo/período, candlestick com
   MMS 9/21, cartões de fechamento, variação, RSI e volatilidade, selo colorido da
   recomendação vigente com justificativa, botão "Analisar agora") e
   `ui/teladetalheativo` (fundamentos, gráfico de linha, tabela das 60 últimas
   cotações e pareceres regra por regra).
   Feito também: `ui/telacarteira` (posições com preço médio/atual, lucro colorido,
   resumo consolidado, compra e venda via `dialogos/dialogooperacao` — arquivo extra,
   fora do contrato original, que atende as duas operações), `ui/telaalertas`
   (cadastro, arquivamento, remoção e "Avaliar agora") e `dialogos/dialogoalerta`.
   Feito também: `ui/telarecomendacoes` (lista das vigentes com cor por tipo e
   reavaliação regra por regra no painel lateral), `ui/telaregras` +
   `dialogos/dialogoregra` (ativar/desativar e reparametrizar as estratégias, com o
   critério descrito pela própria regra) e `ui/telausuarios` +
   `dialogos/dialogousuario` (CRUD com senha opcional na edição).
   Feito também: `ui/janelaprincipal` (navegação lateral filtrada por papel,
   QStackedWidget, recarga da tela ao entrar, navegação cruzada entre telas, menu
   Sistema/Ajuda, barra de status e implementação de `ObservadorAlerta` com
   avaliação automática dos alertas na abertura) e `src/main.cpp` (estilo QSS
   completo, banco em QStandardPaths::AppDataLocation, migrações, laço de sessão
   que permite "Trocar usuário" para demonstrar os dois perfis).
   **A interface está completa: 9 telas + 4 diálogos + 3 widgets.**
7b. ✅ **Ajuste da demonstração:** resolvido pela migração
   `recursos/sql/003_ativo_demonstracao.sql` (MGLU3, P/L 28, DY 0,4%) + `dados/MGLU3.csv`
   calibrado (MMS9 9,82 < MMS21 10,16, RSI 36,6 na faixa neutra) → parecer −0,367 = **Venda**.
   Contexto original: com os fundamentos atuais nenhum ativo fecha em
   **Venda** consolidada (ITUB4 dá −0,80 no cruzamento, mas DY 6,3 e P/L 9,1 puxam a
   média para Neutro). Acrescentar em `002_dados_semente.sql` um ativo caro e sem
   proventos (ex.: MGLU3, Comércio, P/L 28, DY 0,4) e gerar `dados/MGLU3.csv` em
   queda consistente — assim a apresentação mostra Compra, Neutro e Venda.
8. ✅ Testes completos: `testeindicadores.cpp` (27 casos), `testeregras.cpp` (26 casos) e
   `testeimportacao.cpp` (8 casos de integração com banco SQLite temporário, migrações
   reais e prova da atomicidade). Também corrigido no CMake: o `.qrc` passou a ser
   compilado em cada alvo executável (em biblioteca estática o registro do recurso é
   descartado pelo linker) e padronizado o `#include "<arquivo>.moc"` nos três testes.
9. ✅ `README.md` (visão geral, diagrama da arquitetura em camadas, tabela de padrões
   de projeto, tecnologias, build, credenciais e **roteiro de demonstração em 7 passos**)
   e `docs/processos/como-compilar-e-rodar.md` (pré-requisitos por sistema, build,
   testes, reset do banco, tabela de 7 problemas prováveis e checklist de apresentação).
10. ✅ **Revisão adversarial** — concluída (1ª rodada por agentes + 2ª rodada focada em Qt Charts). Os 4 agentes Opus (domínio+análise,
    persistência+SQL, serviços, interface+build) corrigiram 18 arquivos antes de
    morrerem no limite de sessão, **sem entregar relatório**. Correções mais relevantes:
    divisor de scripts SQL reescrito caractere a caractere (comentário `--` no fim da
    linha grudava no comando seguinte e quebraria a migração); idempotência da carteira
    na semente via `NOT EXISTS` (não havia UNIQUE em (usuario_id, nome));
    `qsizetype` × `int` em `qMin/qMax` e `QCOMPARE` (erro de compilação no Qt 6);
    `inserirEmLote` devolvendo −1 (contrato que o serviço já esperava); `desfazer()`
    nos caminhos de commit recusado; cópia do vetor de observadores antes de notificar;
    normalização de ticker/e-mail; includes faltando (`<QtGlobal>`, `<QStringList>`,
    `<QString>`, `<QVector>`, `<QStringConverter>`).
    **Verificações automáticas executadas depois (todas passaram):** toda chamada
    UI→serviços e serviços→repositórios existe nos headers; todo método declarado tem
    definição (erro de link); `#pragma once` e `namespace analisador` em todos os
    arquivos; chaves balanceadas; `Q_OBJECT` em todas as classes com signals/slots;
    ponteiros de membro em `connect` válidos; e simulação do mapeamento
    navegação→página nos dois perfis (com e sem o item separador).
    **2ª rodada (Qt Charts), feita:** `QChartView::setChart` **não destrói** o gráfico
    anterior (só libera a posse) — cada redesenho do painel vazava um `QChart` inteiro;
    criado `trocarGrafico()` nos dois widgets para apagar o antigo. Também acrescentado
    `#include <QPainter>` (usado por `QPainter::Antialiasing`, antes vindo só por
    dependência transitiva), `<QtCharts/QAbstractSeries>` e
    `QT_FORWARD_DECLARE_CLASS(QChart)` nos headers. Casos de borda conferidos: série de
    1 ponto, série vazia e todas as cotações iguais (a folga mínima de 0,5 mantém o
    eixo válido).

    **IMPLEMENTAÇÃO COMPLETA.** O que resta depende de máquina com toolchain:
    1. `sudo apt install build-essential cmake qt6-base-dev qt6-charts-dev libqt6sql6-sqlite`
    2. `cmake -S . -B build && cmake --build build -j` → corrigir o que o compilador apontar
    3. `ctest --test-dir build --output-on-failure` (61 casos)
    4. Executar o roteiro de 7 passos do README e conferir as telas.

**Restrições da execução:** não compilar (sem gcc/cmake/Qt e sem sudo);
convenções em `CLAUDE.md`; commit/push na branch a cada item concluído;
nunca versionar `inicio.txt`, `APS/` nem material do `claude-automacoes`.

**Para compilar quando houver toolchain:**
`sudo apt install build-essential cmake qt6-base-dev qt6-charts-dev libqt6sql6-sqlite`

## Fatos a não re-descobrir

- Régua da disciplina extraída dos modelos do professor (PDFs de `APS/Documentos
  disponibilizados no site/`, lidos integralmente) e do `txt do site.txt`:
  estrutura obrigatória, notas (8+2 por bimestre), prazos 13/10 · 20/10 · 17/11 ·
  24/11/2026, banco ≥5 tabelas, dicionário volumoso (meta 30+ atributos),
  1 diagrama de sequência por UC; no 2º bim: comunicação, DER, estados, atividades
  e sistema rodando (sem ele a parte escrita não é corrigida).
- Análises prontas em `docs/analises/`: `veterano-sol.md` (régua + fraquezas a
  superar: estados de domínio, rastreabilidade, bibliografia real, backend de
  verdade), `veterano-teste-driva.md` (é teste técnico da Driva, não trabalho de
  APS), `metodologia-hub.md` (manual operacional do método do Sergio),
  `perfil-github.md`, `aulas-01-07.md`/`aulas-08-14.md` (relato do bloqueio).
- Equipe: Sergio Roncato Maccari, Arthur Dal Bem Nunes, Leonardo Pereira Shibata,
  Thales Romagna Fabrowski. Doc de transparência: `Leiam esse doc Arthur Leo
  Thales.md` (manter sempre em dia).

## Onde está cada coisa

- Briefing: `inicio.txt` (privado). Regras de trabalho: `CLAUDE.md`.
- Material da disciplina: `APS/` (aulas, modelos, veteranos, site).
- Specs/ADRs/planos: `docs/specs/`, `docs/decisoes/`, `docs/planos/`.
- Roadmap: `MARCOS.md`. Diário: `LOG.md`.
