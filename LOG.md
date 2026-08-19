# Log — Trabalhos de APS (UTFPR)

> **Diário do que FOI FEITO** (datado, *append-only*). O que **falta** e o **status** ficam em
> [`MARCOS.md`](MARCOS.md) — aqui é só o histórico. Não apague entradas.
> Registre cada avanço relevante: **o que mudou · por quê · efeito** (commit/deploy).

## 2026-08-18
- Copiado o material da disciplina de `C:\Users\sergi\Documents\UTFPR\APS` para `APS/` — centralizar aulas, modelos do professor e exemplos de veteranos no repositório.
- Lidos integralmente os modelos do professor (`PlanodoProjeto - 1º bim` e `2º bim`, exemplo MUTEQ) e o `txt do site.txt` — extraídos prazos, composição das notas e a estrutura obrigatória dos documentos.
- Disparada análise paralela (6 agentes) das 14 aulas, dos 2 projetos de veteranos, do hub `claude-automacoes` e dos repositórios GitHub (pessoais + org Driva-tecnologia) — base para a escolha crítica do tema e o planejamento.
- Montado o esqueleto do projeto no padrão do hub: `CLAUDE.md`, `MARCOS.md`, `LOG.md`, `docs/` (specs/design/planos/decisoes/HANDOFF) — método spec-driven replicado.
- `inicio.txt` adicionado ao `.gitignore` a pedido do Sergio — briefing privado, nunca vai a commit/push.
- Criado `Leiam esse doc Arthur Leo Thales.md` (v1) — transparência do método com a equipe.
- Workflow de análise concluído (6 agentes, ~453k tokens): sínteses salvas em `docs/analises/` (veterano SOL, teste-driva, metodologia do hub, perfil GitHub). **Bloqueio encontrado:** os 14 PDFs de aula estão criptografados com senha (RC4 128-bit) — leitura impossível sem a senha.
- Adicionado item 7 ao `inicio.txt` a pedido do Sergio: **projeto em C++ orientado a objetos**.
- Análise crítica do tema concluída: ADR 0001 (tema — proposta "Analisador B3" refinado vs. alternativas) e ADR 0002 (stack — Qt 6 + Qt Charts + SQLite + CMake, propostas). Spec `0001-tema-e-escopo.md` criada em **rascunho**, aguardando aprovação do Sergio e da equipe.
- Sergio aprovou **tema A (Analisador B3)** e **stack Qt 6 + SQLite** — ADRs 0001/0002 marcadas `aceita`, spec 0001 marcada `aprovada` (pendências 🟦 restantes: cliente da entrevista e socialização com a equipe).
- Item 8 adicionado ao `inicio.txt` a pedido do Sergio: **material do claude-automacoes (skills/auxiliares da Driva) nunca sobe neste repo** — `.gitignore` blindado (`.claude/`, `.agents/`, `padroes/`, `design-system/`) e regra registrada no `CLAUDE.md`.
- Senha das aulas fornecida pelo Sergio; **14 PDFs descriptografados** para `APS/Aulas/sem-senha/` via pypdf (baixado sem pip, wheel do PyPI). Leitura completa disparada em workflow com 3 agentes **Opus** (diretriz de economia de tokens).
- Leitura completa das 14 aulas concluída (3 agentes Opus, 137k tokens): sínteses em `docs/analises/aulas-01-04.md`, `aulas-05-09.md`, `aulas-10-14.md`. Achados-chave: template exato de especificação de UC do professor (rótulos `(ator)/(sistema)/(classe X)/(atributo)`, fluxos `*` e `<passo>.<letra>.<n>`), método **Use Case Points** completo (Aula 12) além do Planning Poker, cadeia obrigatória de derivação UC → Sequência → Estados (por classe/UC) → Atividades (por estado), e regra "obedecer a data de entrega, reduzindo funcionalidades se preciso".
- Plano de execução do 1º bimestre criado: `docs/planos/0001-plano-1o-bimestre.md` (cronograma reverso a partir de 13/10 com 1 semana de folga). Fase de planejamento desta sessão concluída.

## 2026-08-18 (commit inicial)
- Planejamento do 1º bimestre incorporado ao doc da equipe (`Leiam esse doc Arthur Leo Thales.md`, seção 6) — cronograma resumido, papéis e regras de ouro.
- Primeiro commit/push do repositório (a pedido do Sergio): estrutura do projeto, docs vivos, análises, specs/ADRs/planos (commit `9ff2c3c`). Fora do versionamento: `inicio.txt` (privado), material da Driva e `APS/` (PDFs da disciplina/veteranos — repo é público).
- Plano do 2º bimestre criado (`docs/planos/0002-plano-2o-bimestre.md`) a pedido do Sergio — duas trilhas paralelas (documento × sistema em 3 sprints Scrum), cronograma reverso de 17/11 e 24/11, janela de reavaliação do 1º bim até 27/10. Detalhamento fino (quantidade exata de diagramas) fica condicionado à lista final de UCs/classes do 1º bim.
