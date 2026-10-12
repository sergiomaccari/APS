# Design — Protótipo de telas por substituição em tempo de link

- **Spec:** [`../specs/0002-prototipo-de-telas.md`](../specs/0002-prototipo-de-telas.md)
- **Status:** aprovado (Sergio, 12/10/2026)

## Solução

### Por que substituição em tempo de link

A interface recebe um único `Contexto` (`src/ui/contexto.h`) com 8 repositórios,
7 serviços e `FonteYahooFinance` **por valor**, sem interfaces abstratas. Trocar
por interfaces exigiria mexer em ~60 chamadas da UI. `#ifdef` espalharia código
de protótipo pelo sistema real. A saída de menor risco é compilar, no alvo do
protótipo, **outras implementações dos mesmos headers**: `src/prototipo/*.cpp`
define as mesmas classes de `servicos/`, `persistencia/` e `analise/` e o alvo
simplesmente não compila os `.cpp` reais. Símbolo duplicado ou faltando vira
erro de link imediato.

Os headers ficam congelados (não podem ganhar membros), então o estado em
memória vive fora deles, num singleton de arquivo.

### Componentes

```
src/prototipo/
  dadosfixos.h / .cpp      estado em memória + semente (usuários, ativos, regras,
                           carteira/posições, alertas, recomendações, importações,
                           cotações lidas de :/dados/*.csv)
  repositorios_stub.cpp    8 repositórios: construtores + 10 métodos que a UI chama
  servicos_stub.cpp        7 serviços + FonteYahooFinance (vtable) + resumo()/vazia()
  analise_stub.cpp         CalculadoraIndicadores (MMS/IFR/variação/volatilidade),
                           MotorAnalise::criarRegra → RegraFixa, Backtester fixo
  main.cpp                 cópia de src/main.cpp sem BancoDeDados
recursos/prototipo.qrc     dados/*.csv com prefixo /dados
```

**`DadosFixos`** (contrato definido pela Tarefa 1 do plano e consumido pelas
Tarefas 2 e 3): `DadosFixos& dados()` devolve o singleton. Guarda vetores de
`Usuario`, `std::vector<std::unique_ptr<Ativo>>` (Acao/FundoImobiliario),
`RegraConfigurada`, uma `Carteira` com `Posicao`s do investidor, `Alerta`,
`Recomendacao` (vigentes e pareceres por regra), `Importacao` e
`QHash<qint64, QVector<Cotacao>>`. Expõe `proximoId()` para inserções e
funções de consulta por id/ticker/usuário. Semente com os valores de
`recursos/sql/002_dados_semente.sql`, `003_ativo_demonstracao.sql` e o que
`capturar-telas.cpp` monta (alertas PETR4 acima de 97 % do último fechamento e
VALE3 abaixo de 90 %; recomendações PETR4 Compra 0,717 · VALE3 Compra 0,450 ·
HGLG11 Compra 0,375 · ITUB4 Neutro −0,079 · MGLU3 Venda −0,367).

**Comportamentos** (spec RP-03 a RP-08): repositórios e serviços leem/escrevem
em `dados()`; `ServicoImportacao` devolve resumo fixo e acrescenta `Importacao`;
`ServicoRecomendacao::gerar*` devolve as recomendações da semente;
`ServicoSincronizacao::sincronizarTodos()` devolve resultado com a mensagem
"previsto para a versão final"; `Backtester::executar` devolve tabela fixa;
`ServicoAlerta::avaliarAlertas` usa `Alerta::condicaoSatisfeita` do domínio e
notifica os observadores já declarados no header.

### Build

`CMakeLists.txt` de `main` ganha `option(APENAS_PROTOTIPO "..." OFF)`:

- OFF (padrão): tudo como hoje **mais** os alvos `analisador-b3-prototipo` e
  `capturar-telas-prototipo` (UI + domínio + `src/prototipo` + `prototipo.qrc`,
  `target_compile_definitions(... ANALISADOR_PROTOTIPO)`, linka só Core, Gui,
  Widgets, Charts).
- ON (na branch da entrega): `find_package` só com Core Gui Widgets Charts;
  apenas os dois alvos do protótipo são definidos.

`src/persistencia/repositorioimportacao.h` troca `#include <QSqlQuery>` por
`QT_FORWARD_DECLARE_CLASS(QSqlQuery)` para o protótipo não depender de Qt Sql
nem nos headers. `janelaprincipal.cpp` usa `#ifdef ANALISADOR_PROTOTIPO` nos
dois pontos de marcação; `main.cpp` real volta à versão `1.0.0`.
`ferramentas/capturar-telas.cpp` usa o mesmo `#ifdef` para pular o banco.

### Branch da entrega

`ferramentas/exportar-entrega.sh` (idempotente, roda em `main` limpa):

1. Cria worktree temporário com `git worktree add --detach` e `git checkout
   --orphan entrega-13-10`, limpa o índice.
2. Copia a whitelist: `CMakeLists.txt` (reduzido aos alvos do protótipo:
   cabeçalho reescrito, `APENAS_PROTOTIPO` ON e nada após o `return()`),
   `.gitignore` gerado (sem comentários descritivos; ignora também `APS/`,
   `docs/analises/`, `.superpowers/`, `.claude/`, `.agents/`, `padroes/`,
   `design-system/`, `cofre*`, `inicio.txt`, `build*/` e `capturas*/`, que
   continuam no disco após `git checkout entrega-13-10`), `src/ui/**`, `src/dominio/**`, `src/servicos/*.h`,
   `src/persistencia/repositorio*.h`, `src/analise/{regraanalise,motoranalise,
   calculadoraindicadores,backtester}.h`, `src/prototipo/**`, `dados/*.csv`,
   `recursos/prototipo.qrc`, `ferramentas/capturar-telas.cpp`,
   `docs/processos/roteiro-apresentacao-1.md`, `docs/entrega/documento-1-bimestre.{md,docx,pdf}`,
   as seções e figuras referenciadas por `documento-1-bimestre.md` (lista obtida
   do manifesto em `ferramentas/montar-documento.py` e dos `![](figuras/...)`
   do próprio `.md`), e `docs/entrega/README-entrega.md` renomeado para
   `README.md` na raiz.
3. Verifica que nada proibido entrou (`inicio.txt`, `APS/`, `.claude/`,
   `.agents/`, `.superpowers/`, `padroes/`, `design-system/`, `cofre*`,
   `docs/analises/`, `CLAUDE.md`, `LOG.md`, `MARCOS.md`, `docs/HANDOFF.md`,
   specs/design/planos/decisões, `*.cpp` de analise/persistencia/servicos,
   `testes/`, `documento-2-bimestre*`) e varre o conteúdo (texto e XML do
   `.docx`, sem diferenciar maiúsculas) atrás de termos de autoria/empresa:
   ocorrência aborta o modo real, salvo `--aceitar-avisos`.
4. Commit único "Entrega 13/10 — Plano do Projeto e protótipo de telas" com
   autor Sergio; remove o worktree. Push e exclusão da branch antiga são passos
   manuais do plano (autorizados pelo Sergio).

## Riscos e tratamento

- Header real evolui em `main` e quebra o stub → o alvo do protótipo faz parte
  do build padrão; a quebra aparece na hora. Na branch da entrega está congelado.
- Símbolo real vazar para o protótipo (ex.: `BancoDeDados`) → link falha; além
  disso `ldd` sem Sql/Network é critério de aceite.
- Membros referência nos construtores dos serviços → repetir a lista de
  inicialização dos headers.
- Gráficos exigem ≥ 60 pregões → os CSVs têm 130.
- Thales já puxou `13/10-prototipo` → doc da equipe avisa para `git fetch -p &&
  git checkout entrega-13-10`.
- `RegraFixa` precisa implementar `nome()`, `avaliar()`, `configurar()`,
  `descricao()` de `RegraAnalise` → ler o header antes de escrever.

## Verificação técnica

- Build Release de `analisador-b3`, `analisador-b3-prototipo`,
  `capturar-telas-prototipo` e dos 5 testes sem avisos; `ctest` 88 casos.
- `ldd build/analisador-b3-prototipo | grep -E 'Sql|Network'` vazio.
- `QT_QPA_PLATFORM=offscreen timeout 10 ./analisador-b3-prototipo` sem crash e
  sem `.sqlite` em `XDG_DATA_HOME` isolado.
- `capturar-telas-prototipo` gera 12 PNGs; conferência visual de 3 (login,
  painel do investidor, janela admin) contra `docs/entrega/figuras/tela-*.png`.
- Branch órfã: clonar o worktree em diretório limpo e compilar do zero.
