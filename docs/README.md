# Documentação — <NOME DO PROJETO>

Índice da documentação. Mantenha esta lista atualizada conforme cria docs.

## Estado do projeto
- [`../MARCOS.md`](../MARCOS.md) — o que **falta/feito** (roadmap).
- [`../LOG.md`](../LOG.md) — o que **foi feito**, datado (diário).
- [`HANDOFF.md`](HANDOFF.md) — **retomada** da próxima sessão (alerta ativo + fatos a não re-descobrir).

## Specs (`specs/`)
Fonte normativa do que deve ser realizado: escopo, requisitos, cenários e critérios
de aceite. Depois de aprovada, a spec prevalece sobre o briefing `inicio.txt`.

## Designs (`design/`)
Como a solução atenderá à spec: arquitetura, UX, componentes, fluxo de dados e riscos.
Use quando a solução exigir decisões não triviais.

## Planos (`planos/`)
Sequência verificável de implementação derivada de uma spec aprovada.

## Plataformas (`plataformas/`)
Como cada ferramenta/serviço funciona e como é usada aqui.
- <ex.: [n8n](plataformas/n8n.md)>

## Dados (`dados/`)
Modelo de dados / esquema de banco: tabelas, relações, migrações.
- Validações duráveis de fonte/qualidade ficam em `dados/validacoes/`, sem extratos
  com PII ou segredos.
- <ex.: [leads](dados/leads.md)>

## Processos (`processos/`)
Como cada automação/processo funciona, passo a passo.
- <ex.: [sincronizacao-leads](processos/sincronizacao-leads.md)>

## Decisões (`decisoes/`)
Registros de decisão (ADR) — o que foi decidido e por quê.
- <ex.: [0001 — escolha do banco](decisoes/0001-escolha-do-banco.md)>

## Incidentes (`incidentes/`)
Quebras/comportamentos inesperados, para não repetir (sintoma · causa · correção · prevenção).
- <ex.: [2026-06-17 — timeout do code-runner do n8n](incidentes/2026-06-17-n8n-code-runner-timeout.md)>

---
Convenções de documentação: hub `claude-automacoes/padroes/documentacao.md`.
