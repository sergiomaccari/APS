# Plano — Trabalho do 2º bimestre (Projeto completo + sistema funcionando)

- **Spec:** [`../specs/0001-tema-e-escopo.md`](../specs/0001-tema-e-escopo.md)
- **Design:** ADRs [0001](../decisoes/0001-tema-do-projeto.md) e [0002](../decisoes/0002-stack-cpp-gui-banco.md)
- **Status:** pendente (detalhamento fino após a entrega do 1º bim; quantidades de
  diagramas dependem da lista final de UCs/classes)

Prazos-alvo: **entrega do documento 17/11/2026** (8,0 pts) e **Testes de Validação
24/11/2026** (2,0 pts). ⚠️ Regra do professor: **a parte escrita só é corrigida se o
sistema for apresentado funcionando** — o software é o gargalo crítico deste
bimestre. Não há reavaliação no 2º bim.

## Estratégia

Duas trilhas em paralelo, para o documento nunca esperar o código:

- **Trilha A — Documento:** os diagramas do 2º bim (comunicação, DER, estados,
  atividades) derivam da modelagem feita em setembro, **não do código** — podem
  começar imediatamente após a entrega de 13/10.
- **Trilha B — Sistema:** 3 sprints Scrum (como ensinado na Aula 13: backlog
  congelado no sprint, executável ao final, daily de 15 min, review com
  funcionalidades e não artefatos).

## Etapas

### 14–27/10 — Transição do 1º bim
- [ ] Retrospectiva pós-apresentação de 20/10; incorporar feedback do professor.
- [ ] Se houver correções do 1º bim: **reavaliação até 27/10** (só de seções já
  entregues na 1ª versão).
- [ ] Congelar lista final de UCs e classes → dimensionar a matriz de diagramas do
  2º bim (por classe × por UC, conforme Aulas 10–11).

### Trilha A — Documento (paralela às sprints)
- [ ] Até 26/10 — **Diagramas de Comunicação** (1 por UC, mensagens numeradas,
  derivados dos diagramas de sequência).
- [ ] Até 26/10 — **Mapeamento para o DER** + script SQL do schema (≥5 tabelas:
  usuario, ativo, cotacao, indicador, regra, recomendacao, carteira, posicao,
  alerta) — o mesmo schema implementado no SQLite (rastreabilidade doc ↔ código).
- [ ] Até 02/11 — **Diagramas de Estados**: os exigidos (classe de fronteira +
  classe de controle por UC, padrão das aulas) **mais os de ciclo de vida de
  domínio** (Alerta, Recomendacao, ImportacaoDeDados) — o diferencial que os
  veteranos não fizeram.
- [ ] Até 09/11 — **Diagramas de Atividades**, um por estado relevante, com o
  título no formato exato do professor ("Diagrama de Atividades para o estado
  '<X>', dentro do Diagrama de Estados para a classe <Y> em <UC>").
- [ ] Até 16/11 — Seção 4.1 Protótipo com prints de todas as telas finais;
  atualização do Histórico de Modificações; revisão cruzada; consolidação.
- [ ] **17/11 — Entrega do documento.**

### Trilha B — Sistema (sprints)
- [ ] **Sprint 1 (20/10–02/11) — Núcleo:** schema SQLite criado por migração,
  camada Repository/DAO, login e papéis (Administrador/Investidor), CRUD de
  ativos, importação CSV transacional com validação. *Critério: fluxo
  login → cadastrar ativo → importar cotações funciona de ponta a ponta.*
- [ ] **Sprint 2 (03/11–09/11) — Análise:** gráficos Qt Charts (candlestick/linha),
  indicadores (médias móveis, variação, RSI, P/L, DY), carteira e posições,
  alertas (Observer), motor de regras (Strategy) com recomendações justificadas.
  *Critério: todos os UCs do Investidor demonstráveis.*
- [ ] **Sprint 3 (10/11–16/11) — Endurecimento:** cobertura dos fluxos de exceção
  especificados nos UCs, dados de demonstração realistas, testes mínimos das
  regras de análise, build de release testado em **máquina limpa e offline**.
- [ ] **Ensaio geral (17–23/11):** roteiro de demonstração UC por UC, cada um
  mapeado ao seu diagrama de sequência (é exatamente o que os Testes de Validação
  cobram: "requisitos atendidos pelos casos de uso" + "diagramas de sequência").
- [ ] **24/11 — Testes de Validação.**

## Validação

- [ ] Checklist do modelo do 2º bim: comunicação (1/UC), DER, estados (por
  classe/UC + domínio), atividades (por estado), protótipo com prints — nenhuma
  seção vazia, numeração e listas de figuras íntegras.
- [ ] Sistema roda em máquina limpa sem internet; demo cobre todos os UCs.
- [ ] Rastreabilidade fechada: RF ↔ UC ↔ diagramas ↔ classes ↔ tabelas ↔ código.
- [ ] `MARCOS.md`, `LOG.md` e doc da equipe atualizados a cada etapa.
