# Análise do projeto veterano "SOL — Sistema de Ocupação de Locais"

Caminho: `/home/sergi/repositorios/APS/APS/trabalho veteranos/SOL-Sistema-de-Ocupacao-de-Locais-main/SOL-Sistema-de-Ocupacao-de-Locais-main/`

---

## 1. O que o projeto é e o tema escolhido

**Tema:** sistema web de gestão, visualização e reserva de salas para a UTFPR Campus Curitiba Sede Centro, substituindo o processo manual (e-mail/presencial) por uma plataforma self-service. Cliente fictício: a própria UTFPR. Equipe: Arthur G. P. Heberle, Luiz Henrique de Souza Correia, Rafael de Andrade Fernandes e Vinícius Romualdo Silva (UTFPR 2026).

**Domínio modelado:**
- 4 papéis de usuário: `GESTOR` (admin, aprova tudo), `PROFESSOR` (reserva diária/semestral, registra projetos), `TUTOR`/aluno de projeto (gere sala do projeto, atividades públicas/privadas, horários livres), `ALUNO` (somente visualização).
- Regras de negócio centrais: reserva de 1 dia auto-aprovada se a sala está livre; reserva semestral/recorrente sempre `PENDENTE` até aprovação do Gestor; salas de departamento não reserváveis (`permiteReserva=false`); membros de projeto têm aprovação automática na própria sala; atividades privadas visíveis só a membros e Gestor; interseção de horários livres dos membros para sugerir reuniões.
- 7 entidades: `Usuario`, `Sala`, `Reserva`, `Projeto`, `Disciplina`, `MembroProjeto`, `Disponibilidade`.

---

## 2. Artefatos de análise/projeto produzidos (a "régua" do que a disciplina cobra)

O documento principal é `PlanodoProjeto.md` (versão markdown de um PDF de ~80 páginas, com imagens embutidas em base64). O **histórico de modificações** do documento revela o cronograma de entregas da disciplina — útil como mapa do que é cobrado:

| Etapa (data no histórico) | Artefato |
|---|---|
| 0.1–0.3 (mar) | Definição do projeto, questões de entrevista, entrevista com cliente |
| 0.4 | Requisitos funcionais e casos de uso |
| 0.5 | Estudo de viabilidade |
| 0.6–0.6.1 | Especificação de casos de uso |
| 0.6.2 | Diagrama de Classes + Dicionário de Informações |
| 0.6.3 | Diagrama de Objetos + Diagramas de Sequência |
| 0.7 (28/04) | **Entrega da 1ª parte** |
| 0.8.1 | Diagramas de Comunicação |
| 0.8.2 | Mapeamento para Diagrama Entidade-Relacionamento |
| 0.8.3 | Diagramas de Estados |
| 0.8.4 | Diagramas de Atividades |
| 0.9 (15/06) | **Conclusão final** |

### Lista exata dos artefatos no repositório

**No `PlanodoProjeto.md`:**
1. Levantamento de requisitos via 4 blocos de questões de entrevista (organizacionais, econômicas, técnicas, operacionais) — 7 perguntas/respostas cada.
2. Requisitos: 9 RF, 3 RNF, 2 RP (uma delas: "banco com no mínimo 5 tabelas" — restrição imposta pela disciplina), 2 REU.
3. Estimativa de esforço com **Planning Poker** (Tabela 1: 89h totais).
4. Estudo de viabilidade (organizacional, econômica, técnica, operacional + recursos).
5. Protótipo (seção existe mas está **vazia**: "(TO DO: Inserir as prints do site aqui)").
6. **1 Diagrama de Casos de Uso** (Figura 1, imagem).
7. **12 especificações de casos de uso** em quadros (UC001 Autenticação … UC012 Gerenciar Membros do Projeto), com ator, pré/pós-condições, fluxo básico, alternativo, regras de negócio e exceções.
8. **2 Diagramas de Classes** (backend e frontend separados — Figuras 2 e 3) + **1 Diagrama de Objetos** (Figura 4).
9. **Dicionário de Informações com 66 quadros** (Quadros 13–66): 7 entidades, 6 controllers, 6 services, 10 DTOs, componentes/páginas React e models frontend — cada atributo com tamanho, tipo, formato e domínio. Exigência citada: "total superior a 30 atributos".
10. **10 Diagramas de Sequência** (Figuras 5–14, um por UC, com UC009-011 agrupados).
11. Conclusões, trabalhos futuros e referências bibliográficas.

**Na pasta `diagramas/` (2ª parte, em Mermaid):**
12. `01_diagramas_comunicacao_SOL.md` — **12 diagramas de comunicação** (1 por UC), simulados com `flowchart LR` e mensagens numeradas.
13. `02_diagramas_estados_SOL.md` — **25 diagramas de estados**, escolhendo por UC uma classe de fronteira (página/componente) e uma de controle (service/controller).
14. `03_diagramas_atividades_SOL.md` — **25 diagramas de atividades**, um para o estado mais representativo de cada classe escolhida.
15. `diagramas_de_classes_SOL.md` — diagramas de classes backend (model/enums/repository/service/controller/dto em namespaces) e frontend em Mermaid, mais uma seção de "análise crítica" apontando inconsistências do documento principal.
16. `00_decisoes_modelagem_SOL.md` — justificativa das escolhas de classes/estados por UC. Nota importante: declara ter seguido "o padrão do grupo de referência" (isto é, copiaram a estrutura da entrega de outro grupo).
17. `casosdeuso.md` — diagramas de sequência Mermaid por UC (versão intermediária, dessincronizada do documento final).

**Código:** frontend React funcional como protótipo (sem backend — "planejado" Java 17 + Spring Boot 3 + PostgreSQL, nunca iniciado).

---

## 3. Estrutura e stack técnica

```
SOL/
  PlanodoProjeto.md          # documento de 80 págs exportado do Google Docs (470KB c/ imagens base64)
  casosdeuso.md              # sequências Mermaid (colagem bruta de chat de IA)
  00_decisoes_modelagem_SOL.md
  diagramas/                 # comunicação, estados, atividades, classes (Mermaid)
  CLAUDE.md / AGENTS.md / RULES.md   # arquivos de contexto para IA
  frontend/                  # React + TypeScript + Vite
    src/pages (8) | components (9) | services (9) | routes (2) | context | types
```

- **Stack real:** React **19** + TypeScript + Vite + react-router-dom 7 (o CLAUDE.md/README dizem "React 18" — desatualizado). Sem nenhuma outra dependência de runtime.
- **Persistência:** `mockDB.ts` — banco falso em `localStorage` com dados seed; toda a camada de services opera sobre ele.
- **Auth:** mock com credenciais hardcoded (`123456` para todos), token falso, papel guardado em localStorage; guard de rotas por hierarquia de papéis (`ProtectedRoute` com `minRole`).
- **Backend:** inexistente. Endpoints REST apenas planejados no CLAUDE.md.
- **Testes:** zero. **CSS:** zero (um reset de 1 linha; todo o estilo é inline).
- ~2.300 linhas de TS/TSX no total.

---

## 4. Avaliação crítica honesta

### Pontos fortes (a régua a bater)
- **Volume documental grande e o rito completo da disciplina cumprido:** entrevista → requisitos rotulados → Planning Poker → viabilidade → 12 UCs especificados → classes + dicionário de 66 quadros → objetos → sequência → comunicação → estados → atividades. O esqueleto metodológico está todo lá.
- O **dicionário de informações** é extenso e cobre até camadas de arquitetura (controllers, services, DTOs, componentes React) — claramente valorizado pela disciplina ("total superior a 30 atributos" é citado como meta).
- As **regras de negócio têm substância real** (aprovação condicional por tipo de sala/papel, visibilidade pública/privada, interseção de horários) — não é um CRUD raso.
- O **protótipo frontend funciona de fato**: fluxos de login por papel, mapa de salas com estado visual, dashboard de aprovação, gestão CRUD, e um componente de heatmap de disponibilidade (`HorariosLivresComponent`) genuinamente elaborado. TypeScript bem tipado, services separados, guard de rotas limpo.

### Fraquezas (onde dá para fazer muito melhor)

**Modelagem:**
- **Profundidade mecânica, não analítica.** Os 25 diagramas de estados/atividades são gerados por fórmula (1 classe de tela + 1 service por UC, todos com o mesmo formato "Aguardando X → Processando X → Sucesso/Erro"). São diagramas de estados de *fluxo de requisição*, não de *ciclo de vida de objetos* — o diagrama de estados mais interessante do domínio (o da **Reserva**: PENDENTE → APROVADA/REJEITADA, recorrência, cancelamento) não existe. Estados de `LoginPage` ("Tela aberta → Aguardando autenticação") são triviais.
- **Especificações de UC desiguais:** UC001–UC007 completos; UC008–UC011 minguam para 3 linhas (sem pré/pós-condições, sem exceções). UC007 tem erro de copy-paste (a regra de negócio RN07 está na linha "Fluxo de Exceção").
- **Inconsistências internas não corrigidas:** `casosdeuso.md` usa atores ("Coordenador") e semântica de UCs diferentes do documento final; a "análise crítica" em `diagramas_de_classes_SOL.md` referencia números de quadros errados; a Lista de Figuras não bate com os UCs; datas do histórico fora de ordem (0.1 em 17/04 antes de 0.2 em 03/03).
- **Seção de protótipo vazia** ("TO DO: Inserir as prints") e o mapeamento ER prometido no histórico (0.8.2) não aparece como artefato próprio no repositório.
- **Referências bibliográficas aparentemente fabricadas:** além do Sommerville (real), constam obras não verificáveis com forte cara de alucinação de IA ("COSTA, Felipe. *Desenvolvimento Web Moderno com Java, Spring Boot e React*. UTFPR Press, 2024"; "OLIVEIRA, Maria; SOUZA, Carlos. *UML Essencial*. Editora TechBook" — o *UML Essencial* real é de Martin Fowler). Risco acadêmico sério e um ponto fácil de superar: citar bibliografia real.
- Admitem abertamente ter **copiado o padrão de modelagem "do grupo de referência"** (outro grupo), em vez de justificar escolhas próprias.

**Código:**
- É um **protótipo de fachada**: sem backend, sem API, sem banco real (a RP001 "mínimo 5 tabelas" não é cumprida por implementação), sem testes, sem CSS estruturado. O código **viola as próprias convenções declaradas** no CLAUDE.md (CSS Modules prometidos, zero arquivos `.module.css`; React 18 declarado, React 19 instalado).
- Divergências código × regras modeladas: em `reservaService.criar`, conflito de horário numa reserva diária vira `PENDENTE` silenciosamente, quando o UC002 especifica exceção "Sala não está mais disponível"; `MapaComponent.ocupada` marca sala como ocupada para sempre se não houver filtro de data. Segurança é decorativa (qualquer um vira GESTOR editando o localStorage — aceitável num mock, mas nunca discutido na documentação).
- Sem acessibilidade, sem tratamento de datas robusto, estilos inline por toda parte.

**Síntese da régua:** a disciplina parece premiar **cobertura ritual de artefatos UML + dicionário volumoso + protótipo demonstrável**. O grupo entregou quantidade com consistência superficial. Para superar: (a) diagramas de estados do *domínio* (ciclo de vida de Reserva/Projeto) em vez de estados de tela; (b) rastreabilidade real requisito → UC → classe → código; (c) documento sem seções TO DO nem numeração quebrada; (d) implementar o banco/backend mínimo que a RP001 exige; (e) bibliografia verdadeira; (f) testes ainda que mínimos — nada disso existe no trabalho veterano.

---

## 5. Uso de IA — evidência abundante e explícita

O projeto foi **abertamente desenvolvido com Claude Code e outros LLMs**:

- **`CLAUDE.md`** — arquivo de contexto para o Claude Code: stack, papéis, rotas, regras de negócio, entidades, convenções de código e endpoints planejados. É o "resumo executivo" do projeto para a IA (e, ironicamente, o documento mais claro e conciso do repositório).
- **`AGENTS.md`** — orquestração de subagentes: manda ler o PDF do plano com **3 agentes em paralelo** (Agent A: requisitos, B: UCs/classes, C: dicionário/sequências), define regras para agentes de código ("só modificar `frontend/src/`", "todo componente deve ter stub visível") e de revisão (`npm run build` sem erros). Contém caminho absoluto da máquina de um integrante (`/home/aheberle/UTFPR/APS/...`).
- **`RULES.md`** — regras comportamentais globais para o assistente ("nunca assuma, pergunte antes", "proponha alternativas", "confirme escopo antes de editar 3+ arquivos") — um arquivo de política pessoal de uso de IA, não um artefato do projeto.
- **`casosdeuso.md`** — **colagem bruta de uma resposta de chatbot**, com preâmbulo intacto: "Com certeza! Compreendi a sua solicitação… Ignorarei a sugestão que dei anteriormente na Turn 1…". Nem o texto conversacional foi limpo.
- **`diagramas/*.md`** — declaram-se "arquivos **gerados**" a partir do `PlanodoProjeto (1).md`; o de classes inclui uma seção de "Análise Crítica e Alterações Sugeridas" com callouts `[!NOTE]/[!IMPORTANT]/[!TIP]` e checkmarks — formato característico de saída de LLM (a IA revisando o documento dos próprios alunos e listando o que eles deveriam corrigir).
- O frontend inteiro tem assinatura de geração assistida (estilo uniforme, comentários-divisores, tipagem completa de uma vez) e o fluxo do AGENTS.md confirma que foi construído assim.
- As referências bibliográficas provavelmente fabricadas reforçam que partes do documento formal também passaram por LLM sem verificação.

**Conclusão sobre IA:** os veteranos usaram IA de forma intensa e razoavelmente sofisticada na *infraestrutura* (contexto, subagentes, convenções), mas com pouca curadoria no *conteúdo* (chat colado sem edição, referências falsas, análise crítica da IA anexada em vez de aplicada). Isso define bem onde está a oportunidade: usar IA com o mesmo nível de ferramentaria, porém revisando, integrando e assumindo autoria do resultado.
