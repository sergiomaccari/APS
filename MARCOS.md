# Marcos — Trabalhos de APS (UTFPR)

Objetivo geral: entregar os 2 trabalhos da disciplina APS com nota máxima — 1º bim
(Plano do Projeto + protótipo de telas) e 2º bim (Projeto completo + sistema
funcionando) — seguindo o método spec-driven do hub.

## Status

Legenda: ⬜ pendente · 🟡 em andamento · ✅ concluído · ❌ bloqueado

| # | Marco | Spec / plano | Critério de "pronto" | Status | Atualizado em |
|---|-------|--------------|----------------------|--------|---------------|
| 1 | Análise do material da disciplina, veteranos e metodologia | não necessário | Sínteses em `docs/analises/` + doc da equipe atualizado | ✅ | 2026-08-18 |
| 2 | Definição do tema (com análise crítica) e spec aprovada | `docs/specs/0001-tema-e-escopo.md` | Spec com status `aprovada` pelo Sergio (e validada com a equipe) | 🟡 | 2026-08-18 |
| 2b | Leitura das 14 aulas (desbloqueadas com a senha) e síntese das convenções do professor | não necessário | `docs/analises/aulas-*.md` regravados com conteúdo real | ✅ | 2026-08-18 |
| 3 | Trabalho 1º bimestre — Plano do Projeto (parte escrita, 8,0 pts) | [`docs/planos/0001-plano-1o-bimestre.md`](docs/planos/0001-plano-1o-bimestre.md) | Documento no modelo do professor completo, revisado, entregue até 13/10/2026 | 🟡 minuta completa | 2026-08-19 |
| 4 | Protótipo de telas (2,0 pts) | idem | Telas gráficas funcionais cobrindo os casos de uso, apresentadas em 20/10/2026 | 🟡 sistema pronto; falta ensaio | 2026-08-19 |
| 5 | Trabalho 2º bimestre — Projeto (parte escrita, 8,0 pts) | [`docs/planos/0002-plano-2o-bimestre.md`](docs/planos/0002-plano-2o-bimestre.md) | Documento completo (comunicação, DER, estados, atividades, protótipo) entregue até 17/11/2026 | 🟡 minuta completa | 2026-08-19 |
| 6 | Sistema funcionando + Testes de Validação (2,0 pts) | [`docs/planos/0002-plano-2o-bimestre.md`](docs/planos/0002-plano-2o-bimestre.md) | Requisitos atendidos pelos casos de uso e diagramas de sequência demonstrados em 24/11/2026 | 🟡 | 2026-08-19 |

### Implementação do sistema (branch `teste-de-desenvolvimento`)

| Camada | Status | Observação |
|---|---|---|
| Domínio (12 classes, 3 máquinas de estado) | ✅ | herança Ativo → Acao/FundoImobiliario |
| Banco (9 tabelas, 3 migrações) | ✅ | SQLite, idempotente, ≥5 tabelas exigidas |
| Análise (indicadores + Strategy com 4 regras) | ✅ | recomendações justificadas com números |
| Persistência (8 repositórios + transações) | ✅ | prepare/bindValue sempre |
| Serviços (6, com Observer e importação atômica) | ✅ | |
| Interface (9 telas, 4 diálogos, 3 widgets) | ✅ | Qt Widgets + Charts, navegação por papel |
| Testes automatizados (61 casos) | ✅ | Qt Test + ctest |
| Documentação de uso (README + guia de build) | ✅ | roteiro de demonstração em 7 passos |
| Revisão adversarial (2 rodadas) | ✅ | ver `LOG.md` de 19/08 |
| **Compilar, testar e rodar** | ✅ | Qt 6.4.2: build limpo, 67 testes passando, sistema executando com dados reais |
| Capturas das telas (figuras do documento) | ✅ | alvo `capturar-telas` gera 12 PNGs em `capturas/` |

### Documentos entregáveis (branch `teste-de-desenvolvimento`, em `docs/entrega/`)

| Artefato | Status | Observação |
|---|---|---|
| Modelo canônico (RF/UC/RN/classes/UCP) | ✅ | `00-modelo-canonico.md` — fonte única de verdade |
| Gerador de diagramas (JSON → PNG) | ✅ | `ferramentas/gerar-diagramas.cpp` + `diagramas/FORMATO.md` |
| 123 diagramas UML verificados visualmente | ✅ | UC, 3 classes, 2 objetos, DER, 18 seq, 18 com, 40 estados, 40 atividades |
| Seções de texto (78+ quadros no formato do professor) | ✅ | entrevista, requisitos, Planning Poker + UCP, viabilidade, 18 UCs, dicionário |
| `documento-1-bimestre.md/.docx` | ✅ minuta | 37 figuras, 83 quadros/tabelas |
| `documento-2-bimestre.md/.docx` | ✅ minuta | 136 figuras, 83 quadros/tabelas |
| Revisão da equipe + pendências 🟦 dos documentos | ⬜ | cliente fictício, sessão real de Planning Poker (Figura 1), autores do histórico |

## Notas

- ✅ **Obstáculo do marco 1 resolvido (18/08):** Sergio forneceu a senha; as 14
  aulas foram descriptografadas para `APS/Aulas/sem-senha/` e a leitura completa
  está em andamento (marco 2b).
- **Marco 2:** tema A e stack aprovados pelo Sergio em 18/08; falta socializar com a
  equipe e definir o "cliente" da entrevista (🟦 na spec 0001).
- **Aqui é só o que falta/feito (roadmap).** O histórico vai no [`LOG.md`](LOG.md).
- O que deve ser construído vive na spec aprovada em `docs/specs/`.
- Item adiado NÃO vira ✅ — registre o obstáculo e o motivo.
- Para retomar entre sessões, use [`docs/HANDOFF.md`](docs/HANDOFF.md).
- Decisões que afetarem os marcos viram um ADR em `docs/decisoes/`.
