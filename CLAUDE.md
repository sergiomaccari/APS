# Projeto: Trabalhos de APS — Análise e Projeto de Sistemas (UTFPR)

> Este arquivo é carregado automaticamente pelo Claude e define como trabalhar aqui.
> O método replica o hub `~/repositorios/claude-automacoes` (spec-driven, logs,
> documentação, perguntar com critério).

## Sobre o projeto

- **Objetivo:** realizar os 2 trabalhos da disciplina APS (1º bim: Plano do Projeto +
  protótipo de telas; 2º bim: Projeto completo + sistema funcionando). Ver `inicio.txt`,
  spec ativa em `docs/specs/` e `MARCOS.md`.
- **Equipe:** Sergio Roncato Maccari, Arthur Dal Bem Nunes, Leonardo Pereira Shibata,
  Thales Romagna Fabrowski.
- **Prazos:** 1º bim — entrega 13/10/2026, testes do protótipo 20/10/2026.
  2º bim — entrega 17/11/2026, testes de validação 24/11/2026 (a nota escrita só vale
  com o sistema funcionando).
- **Idioma:** PT-BR em tudo (docs, código, commits).

## Material de referência

- `APS/` — cópia do material da disciplina: `Aulas/` (14 PDFs), `Documentos
  disponibilizados no site/` (PlanodoProjeto 1º e 2º bim — **modelos obrigatórios**
  dos documentos), `trabalho veteranos/` (exemplos), `txt do site.txt` (regras de
  avaliação e prazos).
- Análises consolidadas do material ficam em `docs/`.

## Regras de trabalho (sempre)

1. **Perguntar com critério:** perguntar em decisões ambíguas/irreversíveis, de escopo
   ou que afetem a nota. Em trivialidades, assumir default sensato e registrar em
   `docs/decisoes/`.
2. **Explicitar o raciocínio** de forma enxuta antes de agir; resumo de decisões no fim.
3. **Spec-driven:** `inicio.txt` preserva o briefing; `docs/specs/` define o que
   realizar; `docs/design/` explica a solução; `docs/planos/` organiza a execução;
   `MARCOS.md` acompanha o progresso. Mudança de requisito atualiza a spec antes do
   código. Spec em `rascunho` não libera implementação.
4. **Estado vivo:** `MARCOS.md` (a fazer/feito) + `LOG.md` (diário datado, append-only)
   + `docs/HANDOFF.md` (retomada entre sessões). Item adiado não vira ✅.
5. **Doc da equipe:** manter `Leiam esse doc Arthur Leo Thales.md` sempre atualizado —
   é o canal de transparência com os colegas que não acompanham as sessões de IA.
6. **Desenvolvimento em 2 partes:** primeiro planejamento (specs/design/planos
   aprovados pelo Sergio), depois execução.
7. Delegar leitura pesada a subagentes; trazer só conclusões ao contexto principal.
8. Os documentos entregáveis devem seguir **fielmente a estrutura do modelo do
   professor** (PlanodoProjeto.doc) — capítulos, quadros de casos de uso, dicionário
   de informações, diagramas conforme os exemplos em `APS/Documentos
   disponibilizados no site/`.

## Git

- Não fazer commit ou push sem pedido explícito; preservar mudanças do usuário.
- Autor: **`Sergio Maccari <sergio.r.maccari@gmail.com>`** (GitHub `sergiomaccari`).
- Commits e PRs **sem atribuição a assistentes/IA** — proibido `Co-Authored-By` ou
  menção a Claude/fornecedor como autoria.
- Mensagens em PT-BR, estilo conventional commits. Nunca commitar segredos.
- **`inicio.txt` é privado e NUNCA vai para commit/push** (já está no `.gitignore`;
  conferir antes de todo commit que ele não entrou no stage).
- **Material do `claude-automacoes` (skills, padrões, design-system, cofre e demais
  auxiliares da Driva) NUNCA sobe neste repositório** — é propriedade do Sergio para
  a empresa e este repo é público. O `.gitignore` bloqueia `.claude/`, `.agents/`,
  `padroes/`, `design-system/`; conferir o stage antes de todo commit.

## Concluir (definition of done)

Ao fechar um marco: documentação proporcional atualizada, `MARCOS.md`/`LOG.md`/doc da
equipe em dia, validação com evidência (testes/telas rodando).
