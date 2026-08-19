# Leiam esse doc — Arthur, Leo e Thales 👋

> Documento vivo, mantido pela IA (Claude) que o Sergio usa no dia a dia como Tech
> Lead. Explica **como o projeto da disciplina está sendo conduzido**, o raciocínio
> por trás de cada etapa e como vocês participam. Última atualização: **18/08/2026**.

## 1. O que está acontecendo aqui

O Sergio configurou o Claude (a IA que ele usa profissionalmente na Driva) para
conduzir os dois trabalhos de APS usando o **mesmo método de desenvolvimento que ele
usa no trabalho**. Isso não é "pedir pro ChatGPT fazer o trabalho": é um processo de
engenharia de software com etapas, documentos e pontos de aprovação humana — que,
aliás, é exatamente o que a disciplina de Análise e Projeto de Sistemas ensina.

## 2. O método: desenvolvimento *spec-driven*

A ideia central: **nenhuma linha de código ou documento final é produzida antes de
existir uma especificação aprovada por humanos**. O fluxo é:

```
inicio.txt  →  spec aprovada  →  design  →  plano  →  execução  →  validação
(briefing)     docs/specs/       docs/design/  docs/planos/          testes/telas
```

- **Spec** (`docs/specs/`): define *o que* o sistema deve fazer — requisitos,
  escopo, critérios de aceite. Fica com status `rascunho` até alguém aprovar.
  Enquanto for rascunho, a IA está proibida de implementar.
- **Design** (`docs/design/`): explica *como* resolver — arquitetura, modelagem.
- **Plano** (`docs/planos/`): quebra a execução em etapas pequenas e verificáveis.
- **ADRs** (`docs/decisoes/`): cada decisão relevante vira um registro curto com
  contexto, alternativas consideradas e consequências — para ninguém perguntar
  depois "por que fizemos assim?".
- **MARCOS.md**: painel do que falta e do que está pronto (roadmap).
- **LOG.md**: diário datado de tudo que foi feito, *append-only* (não se apaga).
- **HANDOFF** (`docs/HANDOFF.md`): nota de retomada — qualquer sessão nova (ou
  qualquer um de vocês) consegue continuar de onde parou.

Por que isso importa para a disciplina: o professor cobra exatamente artefatos de
análise e projeto (requisitos, casos de uso, diagramas UML, viabilidade). O nosso
processo **produz esses artefatos como subproduto natural**, em vez de escrevê-los
de última hora para preencher o documento.

## 3. Como a IA está sendo usada (raciocínio detalhado)

1. **Leitura total do material** — a IA copiou e leu as 14 aulas do professor, os
   dois modelos de documento (PlanodoProjeto 1º e 2º bim, exemplo MUTEQ), o texto do
   site com as regras de nota, e os trabalhos de veteranos. Para não estourar o
   contexto de uma sessão só, ela despachou **6 subagentes em paralelo**, cada um
   lendo uma fatia e devolvendo só a síntese (técnica chamada *fan-out* de agentes).
2. **Régua da avaliação extraída dos modelos** — o documento do 1º bim vale 8,0 pts
   e precisa seguir a estrutura exata do professor: levantamento de requisitos com
   entrevista (questões organizacionais/econômicas/técnicas/operacionais),
   RF/RNF/restrições/REU, estimativa com Planning Poker, estudo de viabilidade,
   diagrama + descrição tabular de cada caso de uso, diagrama de classes, dicionário
   de informações, diagramas de objetos e de sequência. Os testes do protótipo
   (2,0 pts, 20/10) são **telas gráficas funcionais representando os casos de uso**.
   No 2º bim entram comunicação, DER, estados, atividades — e o sistema **rodando**
   (sem sistema funcionando, a parte escrita nem é corrigida).
3. **Análise crítica do tema** — a ideia inicial (avaliador de empresas da B3 com
   recomendações de compra/venda) está sendo ponderada contra alternativas, com
   critérios explícitos (aderência ao que o professor cobra, existência de um
   "cliente" para entrevistar, esforço de modelagem UML vs. esforço de código,
   risco). O resultado vira um ADR e uma spec — que o Sergio (e vocês) aprovam.
4. **Duas fases estanques** — primeiro **planejamento** (tudo acima), depois
   **execução** (documentos finais + protótipo + sistema). A IA não pula a
   aprovação humana entre as fases.
5. **Transparência** — este documento, o `MARCOS.md` e o `LOG.md` são atualizados a
   cada avanço. Se quiserem auditar qualquer decisão, está tudo em `docs/`.

## 4. Estado atual (18/08/2026)

- ✅ Material da disciplina copiado para `APS/` e analisado (sínteses em
  `docs/analises/`). Os 14 PDFs de aula estavam criptografados; o Sergio forneceu a
  senha, foram descriptografados (`APS/Aulas/sem-senha/`) e a leitura completa das
  482 páginas está em andamento.
- ✅ Metodologia do Sergio (hub `claude-automacoes`) absorvida e replicada aqui.
- ✅ Análise dos veteranos: o SOL entrega **volume** (66 quadros de dicionário,
  25 diagramas de estados), mas com profundidade mecânica — estados de tela em vez
  de ciclo de vida de domínio, seções "TO DO", bibliografia suspeita, backend
  inexistente. É exatamente onde vamos ser melhores (detalhes em
  `docs/analises/veterano-sol.md`).
- ✅ Análise crítica do tema concluída → **ADR 0001**: proposta recomendada é o
  **"Analisador B3"** refinado — análise e apoio à decisão de investimentos com
  recomendações **baseadas em regras transparentes** (cada recomendação explica o
  porquê — padrão Strategy em POO), gráficos com métricas por período, carteira e
  alertas. Alternativas B (sistema p/ cliente real) e C (paper trading) documentadas.
- ✅ Restrição definida pelo Sergio: **C++ orientado a objetos** — casa perfeitamente
  com a disciplina (que é toda POO/UML). Stack proposta (**ADR 0002**): Qt 6 +
  Qt Charts + SQLite + CMake (demo 100% offline, sem servidor para dar problema no
  laboratório).
- ✅ **Tema A (Analisador B3) e stack Qt 6 + SQLite aprovados pelo Sergio em
  18/08** — ADRs 0001/0002 aceitas, spec 0001 aprovada. Faltam duas validações com
  vocês: confirmar o tema/stack e definir quem faz o papel de **cliente** na
  entrevista de requisitos.
- ✅ **As 14 aulas foram lidas na íntegra** (482 págs; sínteses em
  `docs/analises/aulas-*.md`). Descobertas que valem nota: o professor tem um
  **template exato de especificação de caso de uso** (fluxo básico com rótulos
  `(ator)/(sistema)/(classe X)/(atributo)`, fluxos alternativos `*` e
  `<passo>.<letra>.<n>`), ensina **Use Case Points** na Aula 12 (podemos entregar
  UCP *além* do Planning Poker — os veteranos não fizeram) e exige a cadeia
  UC → Sequência → Estados (por classe/UC) → Atividades (por estado).
- ✅ **Plano do 1º bimestre pronto**: `docs/planos/0001-plano-1o-bimestre.md` —
  cronograma reverso a partir de 13/10 com uma semana de folga.
- ⬜ Próximo passo: **kickoff da equipe (semana 18–24/08)** — validar tema/stack,
  definir o cliente da entrevista e dividir responsabilidades. Depois começa a
  fase de execução.

## 4b. O sistema já está rodando (19/08/2026)

Além do planejamento, o **sistema do 2º bimestre já está implementado e funcionando** —
bem antes do prazo. Isso nos dá folga para focar no documento escrito, que é onde está
a maior parte da nota.

- **Compilou de primeira** e os **67 testes automatizados passam** (`ctest`).
- Stack: C++20 + Qt 6 (Widgets e Charts) + SQLite, compilado com CMake.
- **9 tabelas** no banco (o professor exige no mínimo 5), criadas por migrações
  versionadas que rodam sozinhas na primeira execução.
- **9 telas** funcionando: login, painel de análise (gráfico de candles com médias
  móveis), detalhe do ativo, recomendações, carteira, alertas, e as telas de
  administração (ativos, importação de cotações, regras, usuários).
- Padrões de projeto aplicados de verdade (bom material para o documento):
  **Strategy** nas 4 regras de análise, **Observer** nos alertas, **Repository** na
  persistência, **Singleton** na conexão e **herança/polimorfismo** em
  `Ativo → Ação / Fundo Imobiliário`.
- Importa **650 cotações** dos CSVs de demonstração em uma operação, de forma
  transacional: um arquivo com erro é rejeitado por inteiro (temos um CSV inválido
  de propósito para demonstrar isso na apresentação).
- As recomendações saem justificadas com os números que as geraram, e a demonstração
  cobre os três resultados: **PETR4 Compra**, **ITUB4 Neutro** e **MGLU3 Venda**.

**As figuras das telas já estão prontas** na pasta `capturas/` do repositório — foram
geradas automaticamente por um utilitário (`capturar-telas`), então dá para regerá-las
a qualquer momento depois de mexer na interface. Elas servem direto no capítulo
"Protótipo" dos dois documentos.

Para rodar na sua máquina: instale `build-essential cmake qt6-base-dev qt6-charts-dev
libqt6sql6-sqlite`, depois `cmake -S . -B build && cmake --build build -j` e
`./build/analisador-b3`. O passo a passo completo (com solução de problemas) está em
`docs/processos/como-compilar-e-rodar.md`, e o roteiro de demonstração em 7 passos
está no `README.md`. Credenciais: `admin@analisadorb3.com` / `admin123` e
`investidor@analisadorb3.com` / `investidor123`.

## 5. O que a equipe precisa decidir/fazer

1. **Aprovar (ou vetar) o tema** — ler o ADR 0001 (`docs/decisoes/`) com a análise
   crítica B3 vs. alternativas. A decisão é nossa, não da IA.
2. **Cliente para a entrevista** — o modelo do professor pressupõe um cliente
   respondendo às questões de levantamento (o MUTEQ entrevistou o diretor do museu;
   o SOL usou a UTFPR como cliente fictício). Sugestões: um investidor pessoa
   física que conheçamos, ou um "clube de investimentos" fictício.
3. **Planning Poker** — a estimativa de esforço é uma dinâmica da equipe (o modelo
   usa o site planningpokeronline.com); reservem uma call de ~30 min quando os
   requisitos estiverem definidos.
4. **Divisão de responsabilidades** — o histórico de modificações do documento
   final lista quem fez o quê, por data; vamos combinar isso na spec.
5. **C++/Qt** — quem não conhece Qt não precisa se assustar: o esqueleto, exemplos
   e documentação de apoio serão gerados no repositório na fase de execução.

## 6. Planejamento do 1º bimestre (resumo — plano completo em `docs/planos/0001-plano-1o-bimestre.md`)

Cronograma reverso a partir da entrega de **13/10/2026**, com 1 semana de folga.
Entregar cedo também antecipa nossa ordem de apresentação (regra do professor).

| Período | Etapa | Entregável |
|---|---|---|
| 18–24/08 | **Kickoff da equipe** | Tema/stack validados por todos; **cliente da entrevista definido**; divisão de responsabilidades |
| até 31/08 | Levantamento de requisitos | 28 questões (4 blocos × 7) + entrevista realizada e registrada |
| até 07/09 | Requisitos | RF (12–19), RNF, RP, REU codificados + regras de negócio |
| até 14/09 | Estimativa e viabilidade | **Planning Poker** (call da equipe) + **Use Case Points** (diferencial: os veteranos não fizeram UCP) + estudo de viabilidade |
| até 28/09 | Modelagem UML | Diagrama de UCs; quadro de especificação de **cada** UC no formato do professor; diagrama de classes; dicionário de informações (>30 atributos); diagrama de objetos; **1 diagrama de sequência por UC** |
| até 05/10 | Documento | Redação no modelo `PlanodoProjeto.doc`, bibliografia real, revisão cruzada |
| até 12/10 | Protótipo | Telas Qt navegáveis cobrindo os UCs + prints no documento |
| 13/10 | **Entrega** | Documento enviado |
| 20/10 | **Apresentação** | Testes do Protótipo (telas funcionais) |

### E o 2º bimestre? (resumo — plano completo em `docs/planos/0002-plano-2o-bimestre.md`)

⚠️ Regra dura: **a parte escrita do 2º bim só é corrigida se o sistema for
apresentado funcionando em 24/11** — e não há reavaliação. Por isso o plano roda
em **duas trilhas paralelas**: o documento (comunicação, DER, estados, atividades —
derivam da modelagem de setembro, não do código) e o sistema (3 sprints Scrum).

| Período | Trilha A — Documento | Trilha B — Sistema |
|---|---|---|
| 14–27/10 | Feedback do professor + reavaliação do 1º bim (até 27/10) | — |
| até 26/10 | Diagramas de Comunicação + DER/schema SQL | Sprint 1 (20/10–02/11): banco, login, CRUD ativos, importação CSV |
| até 02/11 | Diagramas de Estados (incl. ciclo de vida de domínio) | idem |
| até 09/11 | Diagramas de Atividades | Sprint 2: gráficos, indicadores, carteira, alertas, recomendações |
| até 16/11 | Prints do protótipo + revisão final | Sprint 3: exceções, dados de demo, teste em máquina limpa |
| **17/11** | **Entrega do documento** | Ensaio geral UC por UC |
| **24/11** | — | **Testes de Validação (sistema rodando)** |

**O que cada um precisa saber já:**
- A divisão exata de tarefas sai no kickoff (o Histórico de Modificações do
  documento exige autores por etapa — como no exemplo MUTEQ).
- Reservem ~30 min para a call de **Planning Poker** na semana de 08–14/09.
- O sistema será em **C++ com Qt 6** — quem não conhece, sem pânico: o esqueleto,
  exemplos e um guia de build entram no repositório na fase de execução.
- Regra de ouro extraída das aulas (Scrum, Aula 13): **"a data de entrega deve ser
  obedecida; é permitida a diminuição de funcionalidades"** — cortamos escopo,
  nunca o prazo.

## 7. Prazos (do site do professor)

| Entrega | Data | Valor |
|---|---|---|
| Plano do Projeto (1º bim, escrito) | **13/10/2026** | 8,0 |
| Testes do Protótipo (telas funcionais) | **20/10/2026** | 2,0 |
| Projeto (2º bim, escrito) | **17/11/2026** | 8,0 |
| Testes de Validação (sistema rodando) | **24/11/2026** | 2,0 |

Atraso reduz o valor máximo em 25% + 10% por dia. Reavaliação do 1º bim só do que
foi entregue na primeira versão (até 27/10). **Não há reavaliação no 2º bim.**
