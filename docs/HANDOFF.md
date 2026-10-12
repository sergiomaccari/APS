# HANDOFF — retomada entre sessões

> Atualizado em **2026-10-12**. Leia junto com [`../MARCOS.md`](../MARCOS.md) e [`../LOG.md`](../LOG.md).

## Estado atual (resumo de 1 parágrafo)

Véspera da entrega do 1º bimestre (13/10). O **sistema completo** está em `main`
(5 alvos de teste, 88 casos; inclui a revisão do Thales de 11/10, incorporada por
fast-forward). O **protótipo de telas** foi feito na branch `feat/prototipo-stubs`
(ainda não mesclada em `main`): alvo `analisador-b3-prototipo` com as mesmas telas,
dados fixos e estado em memória, via dublês em `src/prototipo/` (spec 0002, design
0001, ADR 0004, plano 0003). A entrega vai na branch órfã **`entrega-13-10`**,
gerada por `ferramentas/exportar-entrega.sh` e publicada no GitHub; ela substitui
a `13/10-prototipo`, que deve ser apagada local e no GitHub. O **documento do 1º
bimestre** está na versão 0.9: conteúdo até a Aula 10 (sem UCP), sem "Tempo
realizado", Capítulo 4 e conclusões alinhados ao protótipo. O documento do 2º
bimestre não mudou.

## Próximos passos

1. Conferir a publicação da `entrega-13-10` (1 commit, sem `merge-base` com `main`,
   compila do zero) e a remoção da `13/10-prototipo` (local e `origin`).
2. Ensaiar o roteiro (`docs/processos/roteiro-apresentacao-1.md`) com o protótipo e
   dividir os blocos A/B/C entre a equipe; entregar no Moodle/na forma que o
   professor pede até 13/10. Testes do protótipo em 20/10.
3. Decidir as pendências restantes do documento (lista abaixo) — se algo mudar,
   regenerar o documento do 1º bimestre e a branch da entrega.
4. Mesclar `feat/prototipo-stubs` em `main` quando o Sergio aprovar.
5. Depois de 13/10, voltar ao 2º bimestre em `main` (plano 0002; entrega 17/11,
   validação 24/11), incorporando o feedback do professor.

## Pendências 🟦 (input humano)

0. **⚠️ Limpar o histórico público** (pendência antiga, de 23/08):
   `docs/analises/metodologia-hub.md`, `perfil-github.md` e
   `veterano-teste-driva.md` (material da Driva + nome de um candidato) ficaram no
   histórico já publicado de `main` e `teste-de-desenvolvimento`. Já saíram do versionamento (`2aaf4c9`), mas a
   reescrita exige aprovação do Sergio. Sugestão no terminal:
   `! pip install git-filter-repo && git filter-repo --invert-paths --path docs/analises/metodologia-hub.md --path docs/analises/perfil-github.md --path docs/analises/veterano-teste-driva.md && git push --force origin main teste-de-desenvolvimento`
   (guardar um clone de backup antes; os arquivos locais são preservados por
   estarem no .gitignore — conferir com `git status` após o filter-repo). A
   `entrega-13-10` é órfã e não carrega esse histórico.
1. **Cliente "própria equipe"** — o documento trocou o Clube de Investimentos
   Neoville pela própria equipe, sem ADR. Confirmar e registrar em `docs/decisoes/`.
2. **Figura 1 do Planning Poker** — é imagem gerada, e o texto afirma sessão
   online. Fazer a sessão e trocar a figura, ou ajustar o texto.
3. **Histórico de Modificações** — datas/autores de 19/08 a conferir por cada um.
4. **Tabela de nível de conhecimento** — preenchida por uma pessoa só; cada um
   confirma a sua linha.
5. **R$ 30/h** na viabilidade — sem fonte; citar referência ou justificar.
6. **TLS** — aparece no quadro do UC020, mas não está na lista de abreviaturas.
7. **`docs/entrega/secoes/dicionario-parte2.md:123`** — cita a `FonteYahooFinance`
   como implementação existente; avaliar uma variante `-1bim`, como foi feito com
   `quadros-uc-01-09` e `dicionario-parte1`.
8. Preferências de custo: subagentes pesados em **Opus**; avisar Sergio ao cruzar
   50% do orçamento de tokens da sessão.

Resolvidas em 12/10: estimativa por UCP (fora do 1º bimestre, é da Aula 12),
coluna "Tempo realizado"/143 h e R$ 4.290,00, Capítulo 4 e conclusões que
afirmavam SQLite e indicadores reais no protótipo, quadros de UC e dicionário que
afirmavam implementação.

## Como rodar o protótipo

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release   # em main/feat: gera os 2 executáveis
cmake --build build -j
./build/analisador-b3-prototipo                    # protótipo (nada gravado em disco)
QT_QPA_PLATFORM=offscreen ./build/capturar-telas-prototipo   # 12 PNGs em capturas-prototipo/
ferramentas/exportar-entrega.sh --dry-run          # lista o que iria para a entrega-13-10
```

Na `entrega-13-10`, o `CMakeLists.txt` já vem com `APENAS_PROTOTIPO=ON` e só
precisa de Qt Widgets e Charts.

## Como regenerar os documentos

```bash
cmake --build build --target gerar-diagramas
QT_QPA_PLATFORM=offscreen ./build/gerar-diagramas --lote docs/entrega/diagramas docs/entrega/figuras
python3 ferramentas/gerar-secoes-diagramas.py
python3 ferramentas/montar-documento.py ambos --docx <caminho-do-pandoc>
```

Editou um diagrama? Mude o `.json` e rode as 2 últimas linhas. Editou texto? Mude a
seção em `docs/entrega/secoes/` e rode só a última.

## Histórico da implementação do sistema (até 19/08, branch `teste-de-desenvolvimento`, hoje em `main`)

**Registro histórico (a fase de execução de agosto terminou).**
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

11. ✅ **COMPILADO, TESTADO E EXECUTANDO** (19/08). Qt 6.4.2 instalado; build sem erros
    na primeira tentativa; 67 casos de teste passando; app cria o banco, aplica as 3
    migrações, importa 650 cotações e gera as 5 recomendações previstas (incluindo o
    cenário de Venda). Figuras das telas em `capturas/`, geradas pelo alvo
    `capturar-telas` (`QT_QPA_PLATFORM=offscreen ./build/capturar-telas capturas`).
    Defeitos que só a execução revelou já corrigidos — ver `LOG.md` de 19/08
    (gráfico vazio por falta de timestamp nos candles, estilo preso no main.cpp,
    interior do gráfico sem tema, eixo de datas ilegível, cabeçalhos cortados).

12. ✅ **DOCUMENTOS ENTREGÁVEIS GERADOS** (19/08). `docs/entrega/` contém o modelo
    canônico, 123 diagramas (JSON + PNG verificado), todas as seções e os dois
    documentos montados em `.md` + `.docx`. Ver "Como regenerar" no topo.

**Próximos passos:** ver a seção "Próximos passos" no topo (atualizada em 12/10).

**Restrições da execução:** convenções em `CLAUDE.md`; commit/push na branch a cada
item concluído; nunca versionar `inicio.txt`, `APS/` nem material do
`claude-automacoes`.

**Toolchain (já instalada em 19/08):** gcc 12, CMake, Qt 6.4.2 (base + charts),
`libqt6sql6-sqlite`; pandoc estático em `~/bin/pandoc` (persistente)
(re-baixar se o /tmp for limpo: https://github.com/jgm/pandoc/releases).

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
