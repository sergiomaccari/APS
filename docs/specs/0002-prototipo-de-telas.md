# Spec — Protótipo de telas da entrega de 13/10 (1º bimestre)

- **Status:** aprovada
- **Dono:** Sergio
- **Aprovada por:** Sergio (em sessão, 12/10/2026)
- **Aprovada em:** 2026-10-12
- **Origem:** [`../../inicio.txt`](../../inicio.txt) · regras do site (`APS/txt do site.txt`)
- **Substitui:** nenhuma (complementa a spec 0001 para o 1º bimestre)

## Problema e resultado esperado

A branch `13/10-prototipo` entregava o **sistema completo** de `main` (SQLite,
motor de análise, sincronização online, backtesting, 88 testes) com apenas três
textos marcando "protótipo". O 1º bimestre pede um **protótipo de telas**:
"telas funcionais em modo gráfico, representando os casos de uso" (2,0 pts em
20/10). O sistema funcionando é o entregável do 2º bimestre (24/11).

Resultado esperado: um executável Qt com **as mesmas telas** do documento do 1º
bimestre, navegável nos 20 casos de uso, porém **sem lógica de negócio real** —
dados de demonstração fixos e estado apenas em memória. Ele vive numa branch
própria da entrega, sem o histórico que mostra o sistema completo.

## Escopo

### Inclui

- Alvo de build `analisador-b3-prototipo` em `main`, que compila `src/ui` +
  `src/dominio` + `src/prototipo` (stubs) e **nenhum** `.cpp` de `analise/`,
  `persistencia/` e `servicos/`.
- Stubs em `src/prototipo/` implementando as mesmas classes e headers de
  `servicos/`, `persistencia/` e `analise/` que a interface usa, sobre um estado
  em memória (`DadosFixos`).
- Dados de demonstração iguais aos das 12 capturas do documento: 2 usuários,
  5 ativos, 4 regras, carteira do investidor, 2 alertas, 5 recomendações e as
  cotações dos CSVs de `dados/`.
- Branch órfã `entrega-13-10`, gerada por script a partir de `main`, contendo só
  o protótipo e os documentos do 1º bimestre.
- Roteiro de apresentação reescrito para o protótipo; README da branch.
- Remoção da branch `13/10-prototipo` (local e GitHub) após a nova existir.

### Não inclui

- Alterar o conteúdo do documento do 1º bimestre (texto, figuras, estimativas).
  As pendências apontadas na revisão de 12/10 ficam para decisão do Sergio.
- Qualquer mudança funcional no sistema completo (`analisador-b3`, testes).
- Reescrita do histórico público de `main` (pendência 0 do HANDOFF, segue
  bloqueada).

## Requisitos e regras de negócio

1. **RP-01 Mesmas telas.** As 44 fontes de `src/ui/` são compiladas sem
   alteração, exceto a marcação "(protótipo)" condicionada à definição de
   compilação `ANALISADOR_PROTOTIPO` em `janelaprincipal.cpp` (título e Sobre).
2. **RP-02 Sem camadas reais.** O executável do protótipo não contém símbolo
   algum dos `.cpp` de `analise/`, `persistencia/` e `servicos/` e não linka
   `Qt6::Sql` nem `Qt6::Network`.
3. **RP-03 Login.** Aceita apenas `admin@analisadorb3.com/admin123` e
   `investidor@analisadorb3.com/investidor123`; outra combinação mostra o mesmo
   erro do sistema real ("credenciais inválidas", via `ultimoErro()`).
4. **RP-04 Estado em memória na sessão.** Cadastrar/editar/remover ativo,
   usuário e regra; comprar e vender; criar e arquivar alerta alteram o estado
   em memória e as telas refletem a mudança até fechar o programa. Nada é
   gravado em disco.
5. **RP-05 Resultado pré-pronto.** Importar CSV/pasta devolve um resumo fixo de
   sucesso (650 cotações, 5 arquivos aceitos, `INVALIDO-exemplo.csv` rejeitado)
   e acrescenta uma linha ao histórico de importações. Gerar recomendações
   devolve as 5 recomendações fixas. Backtesting devolve uma tabela fixa por
   regra.
6. **RP-06 Sincronização online.** Devolve resultado sem rede cuja mensagem
   informa que a sincronização online está prevista para a versão final
   (2º bimestre).
7. **RP-07 Gráficos.** Painel e detalhe do ativo desenham velas, MMS 9/21, IFR
   e volatilidade a partir das cotações fixas. O cálculo de MMS/IFR existe
   apenas para traçar as linhas (exceção única de cálculo, aprovada pelo
   Sergio).
8. **RP-08 Alerta.** Ao abrir a janela principal como investidor, o alerta de
   PETR4 dispara e a barra de status mostra "Alerta disparado", como no
   sistema real (Observer manual de `ServicoAlerta`).
9. **RP-09 Branch órfã.** `entrega-13-10` não tem commit ancestral de `main`;
   contém apenas a whitelist do design. `inicio.txt`, `APS/`, `.claude/`,
   `.agents/`, `padroes/`, `design-system/` e `docs/analises/` nunca entram.
10. **RP-10 Convivência.** `analisador-b3` e os 5 alvos de teste continuam
    compilando e passando em `main` após as mudanças.

## Dados, integrações e permissões

- Fonte dos dados fixos: `recursos/sql/002_dados_semente.sql` e
  `003_ativo_demonstracao.sql` (transcritos para C++), `dados/*.csv` embutidos
  em `recursos/prototipo.qrc`, e os alertas/recomendações que
  `ferramentas/capturar-telas.cpp` monta.
- Nenhuma integração externa; nenhum arquivo gravado pelo protótipo.
- GitHub: push da branch nova e exclusão da antiga autorizados pelo Sergio em
  12/10/2026.

## Cenários e exceções

- **Dado** login como investidor, **ação** abrir Minha carteira e comprar 10
  PETR4 → **resultado** a posição PETR4 passa a 210 e o resumo recalcula; ao
  reabrir o programa, volta a 200.
- **Dado** login como admin, **ação** Importar cotações → pasta `dados/` →
  **resultado** resumo fixo e nova linha no histórico; nenhuma cotação muda.
- **Dado** Regras de análise → Backtesting → **resultado** tabela fixa, sem
  cálculo sobre as cotações.
- **Dado** Sincronizar online → **resultado** mensagem de "previsto para a
  versão final", sem acesso à rede.
- **Dado** `ldd analisador-b3-prototipo` → **resultado** sem `libQt6Sql` nem
  `libQt6Network`.

## Critérios de aceite

1. `cmake --build build --target analisador-b3-prototipo` em Release sem erros
   nem avisos; `ldd` sem Sql/Network.
2. `ctest` do sistema completo: 5 alvos, 88 casos passando (RP-10).
3. `capturar-telas-prototipo` gera as 12 telas em modo offscreen com os mesmos
   dados visíveis das figuras do documento (login, 9 telas, 2 janelas).
4. Smoke offscreen do executável por 10 s sem crash e sem criar arquivo
   `.sqlite`.
5. Branch `entrega-13-10` publicada; `git log --oneline entrega-13-10 | wc -l`
   igual a 1; `git merge-base main entrega-13-10` vazio; a árvore da branch
   compila sozinha num diretório limpo.
6. `13/10-prototipo` ausente em `git ls-remote --heads origin`.

## Decisões pendentes

- ✅ Resolvidas em 12/10 (decisão do Sergio): documento do 1º bimestre restrito
  ao conteúdo até a Aula 10 (saiu Pontos de Caso de Uso; Planning Poker fica,
  porque o modelo exige a seção 2.6); saiu a coluna "Tempo realizado" (143 h) e o
  R$ 4.290,00; Capítulo 4 e conclusões alinhados ao protótipo; quadros de UC e
  dicionário do 1º bimestre sem afirmar implementação (variantes `-1bim`);
  versão 0.9 do documento.
- 🟦 [VALIDAR c/ Sergio] Pendências restantes do documento do 1º bimestre (não
  bloqueiam esta spec):
  - cliente "própria equipe" sem ADR que registre a troca;
  - Figura 1 do Planning Poker é imagem gerada, e o texto afirma sessão online;
  - histórico de versões com datas/autores de 19/08 a conferir;
  - tabela de nível de conhecimento preenchida por uma pessoa só;
  - R$ 30/h da viabilidade sem fonte;
  - TLS (quadro do UC020) sem entrada na lista de abreviaturas;
  - `dicionario-parte2.md:123` cita a `FonteYahooFinance` como implementação
    existente; avaliar uma variante `-1bim`.
