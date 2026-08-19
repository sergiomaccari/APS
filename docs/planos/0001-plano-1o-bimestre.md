# Plano — Trabalho do 1º bimestre (Plano do Projeto + protótipo de telas)

- **Spec:** [`../specs/0001-tema-e-escopo.md`](../specs/0001-tema-e-escopo.md)
- **Design:** ADRs [0001](../decisoes/0001-tema-do-projeto.md) (tema) e [0002](../decisoes/0002-stack-cpp-gui-banco.md) (stack)
- **Status:** pendente (inicia após kickoff com a equipe)

Prazos-alvo: **entrega 13/10/2026** (8,0 pts) e **testes do protótipo 20/10/2026**
(2,0 pts). Entregar cedo também antecipa a ordem de apresentação. Cronograma
reverso a partir de 13/10, com 1 semana de folga.

## Etapas

### Semana 18–24/08 — Kickoff
- [ ] Reunião da equipe: validar tema A + stack (ler `Leiam esse doc Arthur Leo Thales.md` e ADRs), definir o **cliente** da entrevista e a divisão de responsabilidades (o Histórico de Modificações do documento exige autores por etapa).

### Até 31/08 — Levantamento de requisitos
- [ ] Elaborar as **28 questões** (4 blocos × 7: organizacionais, econômicas, técnicas, operacionais) no molde MUTEQ, adaptadas ao domínio de investimentos.
- [ ] Realizar a entrevista com o cliente definido e registrar as respostas.

### Até 07/09 — Requisitos
- [ ] Definir **RF (12–19), RNF, RP e REU** com códigos `[RFxxx]` etc. RPs incluem: C++ POO, GUI Qt, banco SQL ≥5 tabelas.
- [ ] Regras de negócio por RF (recomendação sempre justificada; alerta dispara na atualização; importação transacional).

### Até 14/09 — Estimativa e viabilidade
- [ ] **Planning Poker** com a equipe (planningpokeronline.com, cartas Fibonacci; print da sessão vira Figura 1) → tabela de horas por RF.
- [ ] **UCP (Use Case Points)** conforme Aula 12: UAW, UUCW, TCF (F1–F13), ECF (F1–F8, atenção aos pesos negativos de F2/F7), UCP = UUCP × TCF × ECF — diferencial sobre os veteranos, que só fizeram Planning Poker.
- [ ] Estudo de viabilidade: organizacional, econômica, técnica, operacional + recursos.

### Até 28/09 — Modelagem UML (coração da nota)
- [ ] Diagrama de Casos de Uso (atores Administrador/Investidor, ~12–14 UCs, `<<include>>` login, `<<extend>>` onde couber).
- [ ] **Quadro de especificação de CADA UC no formato exato do professor** (Aula 12): cabeçalho fixo, fluxo básico numerado com rótulos **(ator)/(sistema)/(classe X)/(atributo)**, fluxos alternativos `*` e `<passo>.<letra>.<n>` com retorno explícito, regras de negócio numeradas e referenciadas.
- [ ] Diagrama de Classes (domínio + camadas: telas Qt, controllers, services/DAOs) com Strategy (regras de análise) e Observer (alertas) explícitos.
- [ ] **Dicionário de Informações** — um quadro por classe (atributo, descrição, tamanho, tipo, formato, domínio), meta >30 atributos com folga.
- [ ] Diagrama de Objetos (instâncias em estados representativos).
- [ ] **Diagramas de Sequência — 1 por UC**, camadas Tela → Controller → Service/DAO → BancoDeDados, fragmentos alt/opt/loop.
- [ ] Verificação de rastreabilidade RF ↔ UC ↔ classes ↔ atributos citados nos fluxos (`(classe X)` deve existir no diagrama de classes).

### Até 05/10 — Documento
- [ ] Redigir no modelo `PlanodoProjeto.doc` do professor (capítulos 1–6, Histórico de Modificações com autores/datas reais, listas de figuras/quadros corretas).
- [ ] Bibliografia **real e verificável** (Pressman, Sommerville, Booch/Rumbaugh/Jacobson — fraqueza dos veteranos: referências fabricadas).
- [ ] Revisão cruzada da equipe (nenhum "TO DO", numeração íntegra, consistência interna).

### Até 12/10 — Protótipo de telas (Qt)
- [ ] Esqueleto Qt 6 + CMake com telas navegáveis cobrindo os UCs: login, dashboard com gráfico (Qt Charts com dados de exemplo), CRUD de ativos, carteira, alertas, recomendações com justificativa.
- [ ] Prints das telas incluídos na seção Resultados do documento.

### 13/10 e 20/10 — Entrega e apresentação
- [ ] Entregar o documento em 13/10 (cedo). Ensaiar a demonstração das telas para 20/10.

## Validação

- [ ] Checklist contra o modelo do professor: todas as seções preenchidas, 1 diagrama de sequência por UC, dicionário >30 atributos, RF↔UC rastreável.
- [ ] Protótipo compila e roda no laboratório (testar em máquina limpa, sem internet).
- [ ] `MARCOS.md`, `LOG.md` e doc da equipe atualizados a cada etapa concluída.
