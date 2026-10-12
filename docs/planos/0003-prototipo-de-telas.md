# Plano — Protótipo de telas da entrega de 13/10

- **Spec:** [`../specs/0002-prototipo-de-telas.md`](../specs/0002-prototipo-de-telas.md)
- **Design:** [`../design/0001-prototipo-de-telas-stubs.md`](../design/0001-prototipo-de-telas-stubs.md)
- **Status:** em andamento (12/10/2026): T1–T6 e T8 feitas; T7 (publicação da branch `entrega-13-10`) em andamento
- **Branch de trabalho:** `feat/prototipo-stubs` (a partir de `main` em `d7335be`). Nada é commitado em `main`.

> Para agentes executores: cada tarefa abaixo é autocontida. Leia a spec, o design
> e os headers citados antes de escrever. Os stubs implementam **exatamente** as
> classes declaradas nos headers existentes (mesmos nomes, assinaturas e
> namespace `analisador`); não é permitido alterar esses headers, salvo as duas
> exceções da Tarefa 1. Idioma do código e comentários: PT-BR, sem acentos em
> identificadores (padrão do projeto). Sem `Co-Authored-By` em commits.

## Contrato compartilhado

- **`src/prototipo/dadosfixos.h`** (Tarefa 1 produz; 2 e 3 consomem): namespace
  `analisador::prototipo`, struct `DadosFixos` com os vetores do design e
  `DadosFixos& dados();` (singleton de função, semeado na primeira chamada).
  Funções auxiliares mínimas: `qint64 proximoId()`, `Ativo* ativoPorId(qint64)`,
  `Ativo* ativoPorTicker(const QString&)`, `Usuario* usuarioPorId(qint64)`,
  `Usuario* usuarioPorEmail(const QString&)`, `const QVector<Cotacao>&
  cotacoesDoAtivo(qint64)`. A Tarefa 1 pode acrescentar o que precisar, mas
  **não renomear** esses.
- **Definição de compilação:** `ANALISADOR_PROTOTIPO` (só nos alvos do protótipo).
- **Opção CMake:** `APENAS_PROTOTIPO` (OFF em `main`; ON na branch de entrega).
- **Alvos:** `analisador-b3-prototipo`, `capturar-telas-prototipo`.
- **Recurso:** `recursos/prototipo.qrc` com `dados/*.csv` sob o prefixo `/dados`
  (alias igual ao nome do arquivo, ex. `:/dados/PETR4.csv`).
- **Mensagens fixas** (RP-05/RP-06), usar literalmente:
  - importação: `"Importação concluída: 650 cotações em 5 arquivos; 1 arquivo rejeitado (INVALIDO-exemplo.csv)."`
  - sincronização: `"Sincronização online prevista para a versão final (2º bimestre). Nenhuma cotação alterada."`
- **Branch de entrega:** `entrega-13-10`; README dela vive em `main` como
  `docs/entrega/README-entrega.md`.

## Etapas

### Fase A — paralela

- [x] **T1 — Dados fixos, repositórios, CMake e main do protótipo**
  (arquivos: `src/prototipo/dadosfixos.{h,cpp}`, `src/prototipo/repositorios_stub.cpp`,
  `src/prototipo/main.cpp`, `recursos/prototipo.qrc`, `CMakeLists.txt`,
  `src/persistencia/repositorioimportacao.h` (só o include → forward declare),
  `src/ui/janelaprincipal.cpp` (2 `#ifdef ANALISADOR_PROTOTIPO`), `src/main.cpp`
  (versão volta a `1.0.0`), `ferramentas/capturar-telas.cpp` (`#ifdef` para pular
  banco e população).
  Verificação: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build
  build --target analisador-b3 -j` sem avisos; objetos de `src/prototipo/*.cpp`
  compilam (`cmake --build build --target analisador-b3-prototipo` pode falhar
  **apenas** por símbolos indefinidos de serviços/análise, que T2/T3 fornecem).
  Commit: `feat(prototipo): dados fixos, repositórios em memória e alvo de build`.
  ✅ Feita em 12/10: commit `0197a6f`.
- [x] **T4 — Documentos da entrega**: reescrever
  `docs/processos/roteiro-apresentacao-1.md` para o protótipo (sem ctest, sem
  importação real, sem rede; o que cada bloco demonstra e o que diz "previsto
  para a versão final"); criar `docs/entrega/README-entrega.md` (o que é, como
  compilar só com Qt Widgets/Charts, credenciais demo, mapa tela → UC);
  atualizar `Leiam esse doc Arthur Leo Thales.md` (seção nova: por que o
  protótipo mudou, branch `entrega-13-10`, como trocar de branch, pendências do
  documento para decidir em equipe). Commit: `docs: roteiro e README do protótipo de telas; aviso à equipe`.
  ✅ Feita em 12/10: commit `4c27bfe` (junto com `docs/processos/prototipo-vs-versao-final.md`).
- [x] **T5 — Script de exportação** `ferramentas/exportar-entrega.sh` conforme o
  design (whitelist, verificação de proibidos, commit único, `--dry-run` que só
  lista). Deriva seções e figuras do 1º bim do manifesto em
  `ferramentas/montar-documento.py` e dos `![](figuras/...)` de
  `documento-1-bimestre.md`. Testar com `--dry-run` em `feat/prototipo-stubs`.
  Commit: `feat(ferramentas): script que gera a branch órfã da entrega`.
  ✅ Feita em 12/10: commit `2d2a8fa`, com ajustes da revisão em `c060412`.

### Fase B — paralela, após T1

- [x] **T2 — Serviços stub** `src/prototipo/servicos_stub.cpp`: 7 serviços +
  `FonteYahooFinance` + `ResultadoImportacao::resumo()`, `ResumoCarteira::vazia()`,
  `ResultadoSincronizacao::resumo()` (se declarados nos headers e não definidos
  inline). Comportamentos RP-03 a RP-06 e RP-08. Verificação: alvo
  `analisador-b3-prototipo` linka quando T3 também estiver pronto; até lá,
  compilar o objeto. Commit: `feat(prototipo): serviços em memória`.
  ✅ Feita em 12/10: commit `ee62c00`.
- [x] **T3 — Análise stub** `src/prototipo/analise_stub.cpp`:
  `CalculadoraIndicadores` (MMS, IFR, último fechamento, variação, volatilidade,
  último valor), `MotorAnalise::criarRegra` devolvendo `RegraFixa` (subclasse
  concreta de `RegraAnalise`), `Backtester` com tabela fixa (RP-07, RP-05).
  Commit: `feat(prototipo): indicadores para o gráfico e backtesting fixo`.
  ✅ Feita em 12/10: commit `2d2a8fa`.

### Fase C — integração

- [x] **T6 — Build, testes e capturas**: build Release completo (todos os alvos),
  `ctest` (88 casos), `ldd` do protótipo, smoke offscreen 10 s com
  `XDG_DATA_HOME` isolado, `capturar-telas-prototipo` → 12 PNGs em
  `capturas-prototipo/` (ignorado no git), conferência visual de 3 telas.
  Corrigir defeitos. Commit: `fix(prototipo): ajustes revelados na execução` (se houver).
  ✅ Feita em 12/10, junto com a revisão (Critical 0, Important 3, Minor 11, todos
  tratados): commit `c060412`. Build sem avisos, 88 casos passando, `ldd` sem
  Sql/Network, smoke saiu com 124 sem `.sqlite`, 12 capturas (10 idênticas byte a
  byte às figuras do documento; 01 e 07 diferem só no cursor de texto).
- [ ] 🟡 **T7 — Branch da entrega**: rodar `exportar-entrega.sh`; compilar a branch
  órfã em worktree limpo com `-DAPENAS_PROTOTIPO=ON`; `git push -u origin
  entrega-13-10`; `git push origin --delete 13/10-prototipo`; `git branch -D
  13/10-prototipo`.
  Em andamento (12/10): geração e publicação pelo controlador.
- [x] **T8 — Estado vivo**: `MARCOS.md` (marco 4, tabela de implementação),
  `LOG.md` (entrada 12/10), `docs/HANDOFF.md`, spec/plano com status; commit
  `docs: estado vivo após o protótipo de telas`; `git push -u origin feat/prototipo-stubs`.
  ✅ Arquivos atualizados em 12/10 (commit e push a cargo do controlador).

## Validação

- [x] `cmake --build build -j` sem avisos (alvos: analisador-b3, analisador-b3-prototipo, capturar-telas, capturar-telas-prototipo, gerar-diagramas, 5 testes).
- [x] `ctest --test-dir build` → 5/5, 88 casos.
- [x] `ldd build/analisador-b3-prototipo | grep -E 'Qt6Sql|Qt6Network'` vazio.
- [x] `XDG_DATA_HOME=$TMP QT_QPA_PLATFORM=offscreen timeout 10 build/analisador-b3-prototipo; echo $?` → 124 e nenhum `*.sqlite` em `$TMP`.
- [x] `ls capturas-prototipo/*.png | wc -l` → 12.
- [ ] `git rev-list --count entrega-13-10` → 1; `git merge-base main entrega-13-10` → vazio (código 1).
- [ ] Worktree da `entrega-13-10` compila do zero.
- [ ] `git ls-remote --heads origin` lista `entrega-13-10` e `feat/prototipo-stubs`, não lista `13/10-prototipo`.
