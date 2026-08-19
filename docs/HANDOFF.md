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
