# Spec — Tema e escopo dos trabalhos de APS

- **Status:** aprovada
- **Dono:** Sergio Roncato Maccari
- **Aprovada por:** Sergio (tema A e stack Qt 6 + SQLite; validação com a equipe pendente para o papel de cliente)
- **Aprovada em:** 2026-08-18
- **Origem:** [`../../inicio.txt`](../../inicio.txt)
- **Substitui:** nenhuma

## Problema e resultado esperado

A equipe precisa entregar os 2 trabalhos de APS (UTFPR) com nota máxima:
**Plano do Projeto** (13/10/2026, 8,0 pts) + **protótipo de telas** (20/10, 2,0 pts)
no 1º bimestre; **Projeto completo** (17/11, 8,0 pts) + **sistema funcionando nos
Testes de Validação** (24/11, 2,0 pts) no 2º. Os documentos devem seguir fielmente
o modelo `PlanodoProjeto.doc` do professor.

## Escopo

### Inclui

- **Sistema "Analisador B3"** (tema proposto — ver [ADR 0001](../decisoes/0001-tema-do-projeto.md)):
  análise e apoio à decisão de investimentos na B3, em **C++ orientado a objetos**
  (restrição do inicio.txt item 7), GUI desktop com gráficos, banco SQL ≥5 tabelas.
- Atores: **Administrador** (login, gerenciar ativos, importar cotações CSV/API,
  cadastrar regras de análise, gerenciar usuários) e **Investidor** (login, buscar e
  visualizar ativos, gráficos com métricas por intervalo — médias móveis,
  variação, RSI, P/L, dividend yield —, montar carteira, criar alertas, ver
  recomendações **explicadas** de compra/venda/neutro baseadas em regras).
- ~12–14 casos de uso, cada um com quadro de especificação no padrão do professor.
- Todos os artefatos dos dois documentos: entrevista (4 blocos de questões),
  RF/RNF/RP/REU, Planning Poker (dinâmica com a equipe), viabilidade, diagramas de
  casos de uso/classes/objetos/sequência (1º bim) + comunicação/DER/estados/
  atividades e protótipo com prints (2º bim).
- Stack proposta ([ADR 0002](../decisoes/0002-stack-cpp-gui-banco.md)): Qt 6 +
  Qt Charts + SQLite + CMake; libcurl/nlohmann para atualização opcional de dados.

### Não inclui

- Machine learning ou promessa de acurácia preditiva (recomendações = regras
  transparentes com justificativa; disclaimer de que não é aconselhamento financeiro).
- Execução real de ordens/paper trading multiusuário (fica como "trabalhos futuros").
- Deploy web/mobile — é aplicação desktop C++.

## Requisitos e regras de negócio (esboço — detalhar no documento do 1º bim)

1. RF na faixa de 12–19, mapeados 1:1 (ou N:1) em UCs, seguindo o molde
   Administrador-CRUD + Investidor-consulta do exemplo MUTEQ.
2. RP obrigatórias: backend em C++ (POO); banco SQL com no mínimo 5 tabelas; GUI
   desktop; documento no modelo do professor.
3. Regras de negócio com substância: recomendação sempre acompanhada da regra que a
   gerou; alerta dispara quando a condição é satisfeita na atualização de dados;
   importação valida formato/duplicidade antes de gravar.

## Dados, integrações e permissões

- Séries históricas oficiais da B3 (CSV público) como fonte primária — demo offline.
- API pública de cotações (ex.: brapi.dev) como atualização opcional.
- Papéis: Administrador ≥ Investidor; telas administrativas ocultas ao Investidor.

## Cenários e exceções

- **Dado/ação:** apresentação no laboratório sem internet. **Resultado esperado:**
  sistema roda 100% com o banco SQLite local e dados já importados.
- **Dado/ação:** CSV malformado na importação. **Resultado esperado:** rejeição com
  mensagem clara, nada gravado (transação).

## Critérios de aceite

1. Documento do 1º bim com todas as seções do modelo preenchidas (nenhum "TO DO"),
   entregue até 13/10/2026.
2. Telas funcionais cobrindo os casos de uso demonstradas em 20/10/2026.
3. Documento do 2º bim completo até 17/11/2026 e sistema executando os fluxos dos
   UCs (com diagramas de sequência correspondentes) em 24/11/2026.
4. Rastreabilidade RF ↔ UC ↔ classes ↔ código verificável.

## Decisões pendentes

- ✅ Tema A (Analisador B3) — aprovado pelo Sergio em 18/08/2026 (ADR 0001 aceita).
- ✅ Stack Qt 6 + Qt Charts + SQLite + CMake — aprovada pelo Sergio em 18/08/2026 (ADR 0002 aceita).
- ✅ Senha das aulas fornecida (18/08/2026) — 14 PDFs descriptografados em `APS/Aulas/sem-senha/`.
- 🟦 [VALIDAR c/ equipe] Quem faz o papel de cliente na entrevista (investidor real
  conhecido vs. clube de investimentos fictício).
- 🟦 [VALIDAR c/ equipe] Socializar tema/stack com Arthur, Leo e Thales (doc da equipe).

> Spec aprovada — planejamento detalhado liberado. Ajustes de requisito passam por
> atualização desta spec antes do código.
