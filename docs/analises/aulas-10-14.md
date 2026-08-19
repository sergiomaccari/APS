# Aulas 10 a 14 — APS (UTFPR, Prof. Laudelino Cordeiro Bastos)

> Extração feita com `pypdf` (o Read nativo falhou por ausência de `pdftoppm`). Vários slides são **imagens** (diagramas do estudo de caso "Controle de Leitos de UTI") e não têm texto extraível — indicados abaixo como "slide-imagem". Os títulos desses slides foram preservados, pois indicam a sequência didática de artefatos exigida.

---

## Aula 10 — Diagrama de Estados (19 p.)

### Tópicos cobertos
- Definição e finalidade do Diagrama de Estados (DE)
- Estado de um objeto e sua representação gráfica
- Transições e suas cláusulas
- Cláusulas internas do estado (entry/do/exit)
- Transição interna, ponto de decisão, barra de sincronização e ponto de junção
- Estudo de caso: Controle de Leitos de UTI (slides-imagem, p. 12-18)

### Conceitos-chave ensinados
- **O DE descreve o comportamento de CADA CLASSE, indicando o seu comportamento para todos os casos de uso.** Consequência explicitada pelo professor: "podem haver diversos diagramas de estado referentes a um mesmo processo, pois normalmente em um processo há vários objetos pertencentes a diversas classes".
- O DE é um **grafo orientado**: nós = estados; arcos = transições.
- Cada estado representa parte da vida de um objeto; um objeto passa por diversos estados durante a sua existência. Estados possíveis: **criação, inicialização, à espera de uma mensagem, finalização do objeto**.
- Transição = passagem de um estado para outro, representada por arco/segmento de reta orientado.
- **Transição interna**: não produz modificação no estado do objeto.
- **Ponto de decisão**: ponto na transição de estados onde deve ser tomada uma decisão.
- **Barra de sincronização**: indica ocorrência em paralelo de estados diferentes. **Ponto de junção**: união de dois ou mais estados em paralelo.

### Notações e convenções exigidas
| Elemento | Representação exigida |
|---|---|
| Estado | Retângulo de **cantos arredondados** |
| Estado inicial | **Círculo preenchido** |
| Estado final | **Dois círculos concêntricos**, sendo um preenchido |
| Transição | Arco / segmento de reta **orientado** |
| Ponto de decisão | (losango, conforme slide-imagem) |
| Barra de sincronização / junção | Barra (fork/join) |

**Cláusulas da transição** (as três que o professor cobra):
- **Evento** — indica o evento que resultou do estado anterior
- **Condição de Guarda** — indica *quando* ocorre a transição
- **Ação** — indica o que será feito se a condição de guarda for atendida

**Cláusulas do estado**:
- **Entry** — ações realizadas quando o objeto assume o estado
- **Do** — ações inerentes ao estado
- **Exit** — ações executadas antes da mudança de estado

### Convenção de nomenclatura/rastreabilidade dos artefatos (títulos dos slides do estudo de caso)
A sequência de derivação é explícita nos títulos:
1. "Casos de Uso Identificados - Controle Leitos de UTI"
2. "Diagrama de Classes"
3. **"Diagrama de Seqüência: Solicitar reserva de leito. Criar o DE para Paciente."**
4. **"Diagrama de Estados: classe Paciente em Solicitar reserva de leito"**
5. **"Diagrama de Seqüência: Solicitar reserva de leito. Criar o DE para SolicitacaoLeito."**
6. **"Diagrama de Estados: classe SolicitacaoLeito em Solicitar reserva de leito"**

→ Convenção de título obrigatória: **"Diagrama de Estados: classe `<NomeDaClasse>` em `<Nome do Caso de Uso>`"**. O DE é **derivado do Diagrama de Sequência** daquele caso de uso, uma vez por classe participante.

### Menções a trabalhos/entregas/avaliação/PlanodoProjeto
Nenhuma menção explícita no texto dos slides.

---

## Aula 11 — Diagrama de Atividades (+ Redes de Petri) (26 p.)

### Tópicos cobertos
- Diagrama de Atividades (DA): definição, mudança de fundamento na UML 2.0
- Componentes do DA
- Modelagem das estruturas de controle: `for`, `while`, `do while`, `switch case`
- Estudo de caso Controle de Leitos de UTI (slides-imagem, p. 12-16)
- Redes de Petri: componentes, marcação, regras de funcionamento e exemplo animado (p. 17-25)

### Conceitos-chave ensinados
- **"O Diagrama de Atividades era um caso especial do Diagrama de Estados, até a UML 2.0. Após a UML 2.0, o Diagrama de Atividades deixou de ser baseado no Diagrama de Estados e passou a ter como suporte as Redes de Petri."**
- O DA representa **algoritmos ou métodos**, sendo semelhante aos antigos **fluxogramas**.
- **O DA não representa processos completos** como o Diagrama de Sequência ou de Colaboração; **indica os passos realizados por um objeto em um determinado estado**.
- Herda componentes do DE: Estado Inicial, Estado Final, Transições e Barras de Sincronização.
- **Estado de Ação (Action)**: representa a execução de uma ação, **não podendo ser decomposta**. Retângulo de bordas arredondadas. **Pode conter pseudo-código, código-fonte ou fórmulas.**
- **Ponto de Decisão (Decision Node)** e **Ponto de Convergência (Merge Node)**: ambos representados por **losango**. Decisão = teste a ser realizado; Convergência = junção de fluxos alternativos.
- Exemplos de mapeamento de estruturas de controle para DA: `for` (com decisão + convergência marcados), `while`, `do while`, `switch case`.
- **Observação de convenção**: no exemplo do `switch case`, o professor anota explicitamente **"Não é UML, simboliza a continuidade de 'b' até 'n'"** — ou seja, símbolos fora do padrão devem ser declarados como tal no diagrama.

**Redes de Petri**:
- Utilizadas em modelagem de sistemas; expressam **concorrência e paralelismo**; baseadas em um "jogo de fichas" que circulam por uma rede de estados e transições.
- Componentes: **Lugares, Fichas, Transições, Eventos**.
- **Marcação da rede** = conjunto de lugares + fichas.
- Regras de funcionamento:
  - Cada lugar pode possuir um número máximo de fichas.
  - Uma ficha na entrada de uma transição é distribuída para todas as saídas.
  - Um lugar consome o número de fichas indicado na transição (**default = 1**).
  - Se não existem fichas na entrada, a transição não ocorre.
  - Se o lugar de saída já possui o limite máximo de fichas, a transição não ocorre.
- Exemplo trabalhado: máquina que espera uma ordem, prepara o pedido e despacha (4 condições e 4 eventos, com a rede evoluindo em 5 marcações sucessivas).

### Notação/convenção de rastreabilidade (título do slide-imagem, p. 16)
**"Diagrama de Atividades para o estado 'Verificando regras de negócio', dentro do Diagrama de Estados para a classe Paciente em Solicitar reserva de leito"**

→ Convenção de título obrigatória: **"Diagrama de Atividades para o estado `<Nome do Estado>`, dentro do Diagrama de Estados para a classe `<Classe>` em `<Caso de Uso>`"**. Confirma a cadeia de derivação: Caso de Uso → Diagrama de Sequência → Diagrama de Estados (por classe) → Diagrama de Atividades (por estado).

### Menções a trabalhos/entregas/avaliação/PlanodoProjeto
Nenhuma menção explícita.

---

## Aula 12 — Estimativa de Esforço de Software: Use Case Points e Planning Poker (36 p.)

### Tópicos cobertos
- Estimativa de esforço de software (motivação/referência acadêmica)
- **Use Case Points (UCP)** — os 5 passos completos, com exemplo numérico integral
- Reapresentação da **Especificação de Caso de Uso** (formato exigido) e do Diagrama de Classes
- **Planning Poker**

### Conceitos-chave ensinados

**Origem**: UCP proposto por **Gustav Karner (1993, Linköping, Suécia)**, artigo *Resource Estimation for Objectory Projects*; tornou-se parte do **RUP** (baseado no Objectory de Ivar Jacobson); derivado da **Análise de Pontos de Função de Allan Albrecht (IBM, 1979)**.

**1º passo — UAW (Unadjusted Actor Weight)**

| Tipo de Ator | Peso | Descrição |
|---|---|---|
| Ator Simples | 1 | Outro sistema acessado através de uma **API de programação** |
| Ator Médio | 2 | Outro sistema interagindo através de **protocolo de comunicação** (TCP/IP, FTP) |
| Ator Complexo | 3 | **Usuário interagindo através de interface gráfica** (local ou via Web) |

UAW = Σ (nº de atores de cada tipo × peso). *Exemplo UTI: 4 atores complexos × 3 = **UAW = 12**.*

**2º passo — UUCW (Unadjusted Use Case Weight)**

| Complexidade | Peso | Critério |
|---|---|---|
| Caso de Uso Simples | 5 | Até **3 transações** (incluindo fluxo alternativo); acessa **menos de 5** objetos/entidades de persistência |
| Caso de Uso Médio | 10 | De **4 a 7 transações**; acessa **de 5 a 10** objetos/entidades |
| Caso de Uso Complexo | 15 | Mais de **7 transações**; acessa **mais de 10** objetos (instâncias de classe) ou entidades (tabelas) |

*Exemplo UTI: 1 simples (5) + 3 médios (30) + 3 complexos (45) = **UUCW = 80**.*

**3º passo — UUCP** = UAW + UUCW. *Exemplo: 12 + 80 = **92***.

**4º passo — Fatores de ajuste** (procedimento similar ao da Análise por Pontos de Função), em duas partes: fatores **técnicos** (requisitos funcionais) e fatores de **ambiente** (requisitos não-funcionais associados ao processo de desenvolvimento).

*Fator de Complexidade Técnica*: **TCF = 0,6 + (0,01 × TFactor)**; TFactor = Σ (nível de influência × peso), F1..F13, escala **0 a 5** (0 = irrelevante, 5 = essencial).

| Fator | Descrição | Peso | (Valor do exemplo → Total) |
|---|---|---|---|
| F1 | Sistemas distribuídos | 2 | 2 → 4 |
| F2 | Tempo de resposta | 1 | 1 → 1 |
| F3 | Eficiência para o usuário final (on-line) | 1 | 2 → 2 |
| F4 | Processamento interno complexo | 1 | 1 → 1 |
| F5 | Código reusável | 1 | 2 → 2 |
| F6 | Facilidade de instalação | 0,5 | 3 → 1,5 |
| F7 | Facilidade de uso (facilidade operacional) | 0,5 | 4 → 2 |
| F8 | Portabilidade | 2 | 3 → 6 |
| F9 | Facilidade de mudança | 1 | 4 → 4 |
| F10 | Concorrência (acesso simultâneo à aplicação) | 1 | 1 → 1 |
| F11 | Recursos de segurança | 1 | 1 → 1 |
| F12 | Fornece acesso direto para terceiros | 1 | 0 → 0 |
| F13 | Requer treinamento especial para o usuário | 1 | 2 → 2 |
| | | | **TFactor = 27,5 → TCF = 0,875** |

*Fator de Complexidade de Ambiente*: **ECF = 1,4 + (−0,03 × EFactor)**; EFactor = Σ (nível × peso), F1..F8, escala 0 a 5.

| Fator | Descrição | Peso | (Valor do exemplo → Total) |
|---|---|---|---|
| F1 | Familiaridade da equipe com o processo formal de desenvolvimento adotado | 1,5 | 4 → 6 |
| F2 | Colaboradores de meio período | **−1** | 2 → −2 |
| F3 | Capacidade do líder do projeto em análise de requisitos e modelagem | 0,5 | 4 → 2 |
| F4 | Experiência da equipe em desenvolvimento de aplicações do gênero em questão | 0,5 | 4 → 2 |
| F5 | Experiência em Orientação a Objetos | 1 | 4 → 4 |
| F6 | Motivação da equipe | 1 | 4 → 4 |
| F7 | Dificuldades com a linguagem de programação | **−1** | 2 → −2 |
| F8 | Requisitos estáveis | 2 | 2 → 4 |
| | | | **EFactor = 14 → ECF = 0,98** |

**5º passo — UCP ajustado**: **UCP = UUCP × TCF × ECF**. *Exemplo: 92 × 0,875 × 0,98 = **78,89 UCP***.
- **Karner sugere 20 horas de trabalho por UCP.** Tempo estimado = UCP × 20 → 78,89 × 20 = **1577,8 horas**; o slide registra entre parênteses a alternativa com **fator 5** → **394,45 horas** (variação do fator hora/UCP que o professor apresenta como opção).

**Planning Poker**
- Método de estimativa **top-down baseado em julgamento de especialista** [Dagnino 2013], útil nos estágios iniciais, quando se estima o **tamanho** do software e o **esforço** é calculado com base na **velocidade do time**.
- Participantes [Raith et al. 2013]: todos os membros do time como **estimadores**, um **moderador** (Scrum Master, por exemplo) e, **opcionalmente, um representante do cliente**.
- **Cartas**: sequência de **Fibonacci incluindo o zero — 0, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89** — para refletir a incerteza inerente a estimar valores muito grandes em horas. Conjunto alternativo: 0, ½, 1, 2, 3, 5, 8, 13, 20, 40, 100, mais opcionalmente **"?" (não sei)** e **copo de café (preciso de uma pausa)**.
- Estruturação [Williams et al. 2011]: obter compreensão comum; expor suposições ocultas de implementação/verificação; discutir implicações da implementação do requisito no sistema; descobrir e resolver ambiguidades por perspectivas divergentes; expor alternativas de implementação.
- Vantagens [Børte et al. 2012]: todos participam independentemente de posição/experiência; **exige justificativa de quem estimou o maior e o menor valor**; usa as cartas como artefatos; permite que especialistas de áreas diferentes estimem juntos. Por ser um jogo, **não se torna cansativo** [Desharnais et al. 2011].

### Notações e convenções exigidas — **formato da Especificação de Caso de Uso** (p. 11-13)
Este é o quadro/template mais cobrado. Estrutura literal:

```
Diagrama de Caso de Uso: Solicitar reserva de leito
Ator Principal: Médico Assistente
Ator de Suporte: Médico Intensivista
Descrição: Caso de uso executado quando o médico assistente necessita
           realizar uma solicitação de reserva de leito.
Pré-condições:
  • Paciente cadastrado.
  • Médico Assistente tem que estar autorizado.
Pós-condições:
  • Reserva de leito solicitada
```

**Fluxo Básico** — numerado (1, 2, 3...), com subitens em letras (a, b, c...) para os campos de tela, e — regra explícita do professor: **"Deve-se identificar a função de cada elemento (entre parênteses)"**:
- `(ator)` para passos executados pelo ator
- `(sistema)` para passos executados pelo Sistema
- `(classe <Nome>)` para cada informação exibida/manipulada, indicando a **classe de origem do atributo** (ex.: "Nome do paciente **(classe Paciente)**", "Urgência **(classe SolicitacaoLeito)**") — o slide rotula esse bloco como **"Atributos"**
- `(atributo)` quando o passo registra um atributo (ex.: "O Sistema registra a data da solicitação **(atributo) (sistema)**")
- Chamada de desvio para o fluxo alternativo entre parênteses ao final do passo: ex. passo 5 → **"(5.a)"**
- Referência explícita às regras de negócio no passo: "O Sistema verifica as informações conforme **Regras de Negócio 1 e 2**"

**Fluxo Alternativo** — dois tipos:
- Fluxos "a qualquer momento", marcados com `*`: ex. "* A qualquer momento o Usuário seleciona cancelar: O Sistema desconsidera informações digitadas e encerra o caso de uso." / "* A qualquer momento o Usuário seleciona limpar: O Sistema limpa todas as informações digitadas."
- Fluxos ancorados no passo do fluxo básico, numerados **`<passo>.<letra>`** e sub-numerados **`<passo>.<letra>.<n>`**, com retorno explícito:
  ```
  5.a. As informações digitadas pelo Usuário estão incorretas
    5.a.1. O Sistema exibe quais informações estão incorretas.
    5.a.2. E retorna para o Fluxo Básico 3.
  ```

**Regras de Negócio** — lista numerada ao final: "1. O campo 'Nome do paciente' é obrigatório. 2. A 'data de nascimento' deve ter o formato dd/mm/aaaa."

### Menções a trabalhos/entregas/avaliação/PlanodoProjeto
Nenhuma menção explícita ao documento PlanodoProjeto. Porém esta é a aula com o conteúdo mais diretamente "entregável" (tabelas de UCP, TCF, ECF, cálculo de horas e cronograma).

---

## Aula 13 — Manifesto Ágil e Scrum (24 p.)

### Tópicos cobertos
- Manifesto Ágil: origem, valores, 12 princípios
- Scrum: origem do nome, papéis, fases, planejamento, sprint, reuniões diárias, revisão, retrospectiva, encerramento

### Conceitos-chave ensinados

**Manifesto Ágil** — declaração de princípios que fundamentam o desenvolvimento ágil; feito em **2001**; "Estamos descobrindo maneiras melhores de desenvolver software, fazendo-o nós mesmos e ajudando outros a fazerem o mesmo."

**Quatro pares de valores (formato "Versus" usado pelo professor)**:
| Valorizado | Versus | Menos valorizado |
|---|---|---|
| Indivíduos e Iterações | vs. | Processos e Ferramentas |
| Software funcionando | vs. | Documentação detalhada |
| Colaboração com cliente | vs. | Negociação de contratos |
| Responder a mudanças | vs. | Seguir um plano |

**12 Princípios da Agilidade** (numerados, conforme os slides): 1) satisfação do cliente pela liberação mais rápida e contínua de software de valor; 2) receber bem mudanças de requisitos mesmo em estágios tardios; 3) liberar software frequentemente (2 semanas a meses, preferindo o menor); 4) pessoas de negócio e desenvolvedores juntos a maior parte do tempo; 5) indivíduos motivados, com ambiente, suporte e confiança; 6) comunicação **face a face** como método mais eficiente; 7) **software funcionando é a principal medida de progresso**; 8) desenvolvimento sustentado/ritmo indefinidamente sustentável; 9) atenção contínua à excelência técnica e bom design; 10) simplicidade — arte de maximizar a quantidade de trabalho **não** feito (no slide: "maximizar a quantidade de trabalho feito"); 11) melhores arquiteturas, requisitos e projetos emergem de **equipes auto-organizadas**; 12) em intervalos regulares a equipe reflete e ajusta seu comportamento.

**Scrum** — nome vindo da estratégia do rugby em que jogadores recolocam a bola em jogo por trabalho em equipe; "uma alternativa para utilizar Métodos Ágeis na gerência de projetos".

**Três papéis**:
- **Product Owner** — representa os envolvidos e o negócio; dono do produto ou seu representante; cria a **visão do produto** e define as **prioridades dos requisitos**.
- **Scrum Master** — mantém os processos (normalmente no lugar de um gerente de projeto); facilitador; **remove impedimentos** e fornece liderança de processo.
- **Equipe (Team)** — grupo **multifuncional** que faz análise, projeto, implementação, testes etc.

**Fases**: Planejamento → Sprints (Reuniões Diárias, Revisão, Retrospectivas) → Encerramento.

**Planejamento** (relativamente curto):
- Deve ser feito o **Projeto da Arquitetura do Sistema**
- **Estimativas de Datas e Custos** são feitas
- **Criação do backlog**: participação de clientes e outros departamentos; **Levantamento de Requisitos** e atribuição de **prioridades**; pode conter **tarefas diretas, Casos de Uso e Histórias de Usuário (como na XP)**
- Definição de **equipes e seus líderes**
- Definição de **pacotes** a serem desenvolvidos

**Reunião de planejamento do Sprint** — timebox de **8 horas**, em **dois segmentos de 4 horas**:
- 1º segmento: PO prepara o **Product Backlog antes** da reunião; seleção dos itens que o time se compromete a tornar **incrementos potencialmente implementáveis**; **a decisão final é do Product Owner**.
- 2º segmento (imediatamente após): PO disponível para perguntas; **o Time decide sozinho como os itens serão implementados**; **nenhum outro participante além do Time pode fazer perguntas ou observações**; **resultado = Sprint Backlog**.

**Sprint**:
- O time recebe parte do backlog; **o backlog não sofrerá modificações durante o Sprint**
- Duração de **1 a 4 semanas** (30 dias na figura clássica)
- **Sempre apresenta um programa executável ao final**
- Responsabilidades do time: participar das reuniões diárias; manter o Sprint Backlog **atualizado**; disponibilizá-lo **publicamente**; compromisso de implementar todos os itens selecionados

**Reunião diária** (~**15 minutos**) — três perguntas:
1. O que você realizou desde a última reunião?
2. Quais problemas você enfrentou?
3. Em que você trabalhará até a próxima reunião?

Variante conduzida pelo ScrumMaster: o que fez desde a última daily / o que fará até a próxima / **o que está impedindo você de realizar seu trabalho o mais efetivamente possível**. "Os membros devem responder apenas a estas perguntas para não estender a reunião."
Benefícios: maior integração; rápida solução de problemas por compartilhamento de conhecimento; progresso medido continuamente → minimização de riscos.

**Reunião de Revisão** (fim do sprint): máximo **4 horas**, responsabilidade do **ScrumMaster**; time não gasta mais de **1 hora** na preparação; **deve obedecer à data de entrega — é permitida a diminuição de funcionalidades**; apresenta o produto ao cliente (PO e interessados); **"Artefatos não devem ser apresentados, pois não são funcionalidades"**. Ao final: cada stakeholder fala das impressões e sugere mudanças com prioridades; modificações no Product Backlog são discutidas entre PO e Time; ScrumMaster anuncia data e local da próxima revisão.

**Retrospectiva**: máximo **3 horas**; participam **Time, ScrumMaster e, opcionalmente, o Product Owner**; duas questões: "O que aconteceu de bom durante o último sprint?" e "O que pode ser melhorado para o próximo sprint?"; o ScrumMaster escreve as respostas e as prioriza; seu papel aqui é fazer o time encontrar melhores formas de aplicar o Scrum.

**Encerramento** (finalização do projeto): testes de integração; testes de sistema; documentação do usuário; preparação de material de treinamento; preparação de material para o **marketing** do produto.

### Menções a trabalhos/entregas/avaliação/PlanodoProjeto
Nenhuma menção direta a trabalhos da disciplina. Contém, porém, a regra de entrega que o professor enfatiza em contexto de projeto: **"Deve obedecer à data de entrega: é permitida a diminuição de funcionalidades"**.

---

## Aula 14 — Extreme Programming (XP) (20 p.)

### Tópicos cobertos
- XP: definição e posicionamento
- As 8 práticas de XP
- Ambiente colaborativo
- Planejamento Incremental: princípios e benefícios
- TDD: etapas e encaixe na XP

### Conceitos-chave ensinados
- XP é metodologia ágil que busca entregar **software de alta qualidade de forma rápida e flexível**, com abordagem **colaborativa e centrada no cliente**.

**As 8 práticas (numeradas pelo professor)**:
1. **Planejamento incremental** — plano de curto prazo e iterativo em vez de plano detalhado no início
2. **Desenvolvimento orientado a testes (TDD)** — testes antes do código
3. **Integração contínua** — integrar o código à medida que é escrito
4. **Desenvolvimento em pares** — código revisado e testado eficazmente
5. **Design simples** — evita recursos desnecessários, foco na necessidade do usuário
6. **Propriedade coletiva do código** — todos são responsáveis pelo código
7. **Ritmo sustentável** — evita longas jornadas que levam a exaustão e erros
8. **Cliente presente** — participação ativa do cliente no processo

**Planejamento Incremental na XP — 6 princípios**:
1. **Planejamento Contínuo** — planejamento inicial focado nos requisitos mais importantes e metas gerais; refinado e ajustado conforme o projeto avança
2. **Pequenas Iterações** — iterações curtas, **normalmente de 1 a 3 semanas**, cada uma resultando em incremento funcional testável e entregável
3. **Priorização Constante** — equipe + stakeholders priorizam; prioridades mudam com novas informações
4. **Entrega Contínua** — a cada iteração, um incremento que pode ir para produção
5. **Feedback Contínuo** — feedback de clientes e usuários ajusta a direção
6. **Adaptação Flexível** — disposição para se adaptar a mudanças de requisitos e prioridades

**Benefícios**: maior flexibilidade; foco constante nas necessidades do cliente; entrega rápida de valor; redução de riscos (identificados e gerenciados ao longo do tempo); melhoria da qualidade.

**TDD — 3 etapas (ciclo com cores nomeadas pelo professor)**:
1. **Escrever um teste (Teste em Vermelho)** — o teste falha inicialmente porque o código ainda não existe
2. **Escrever o código mínimo para passar no teste (Código em Verde)** — apenas o suficiente, sem otimização nem funcionalidade adicional
3. **Refatorar o código (Refatoração)** — melhorar a qualidade **sem alterar o comportamento**; os testes existentes garantem isso

**Como o TDD se encaixa na XP (5 pontos)**: 1) **Testes automatizados** — definição dada: "procedimento de verificação ou validação de software executado automaticamente por meio de ferramentas de teste, scripts ou programas de teste, em vez de manualmente por um testador humano"; 2) **Feedback contínuo** — identifica problemas de design e lógica antes de se tornarem críticos; 3) **Entrega contínua** — confiança de que alterações não quebram funcionalidades existentes; 4) **Documentação viva** — os testes documentam o comportamento do software, útil em projetos ágeis com requisitos evoluindo; 5) **Redução de erros** — erros detectados e corrigidos precocemente, economizando tempo e recursos.

### Notações/convenções
Não há notação gráfica nesta aula. A convenção relevante é terminológica: **Histórias de Usuário** (citadas na Aula 13 como oriundas da XP) como forma de item de backlog, e o vocabulário **vermelho → verde → refatorar**.

### Menções a trabalhos/entregas/avaliação/PlanodoProjeto
Nenhuma menção explícita.

---

# Síntese — impacto direto na confecção dos trabalhos

> **Observação importante:** em nenhuma das cinco aulas (10 a 14) há menção textual ao documento **PlanodoProjeto**, a prazos, notas ou critérios de avaliação. Todo o impacto é **de conteúdo/artefato** — estas aulas definem *o que* deve constar e *em que formato*. (Os documentos `PlanodoProjeto - 1o bim` e `- 2o bim` existem em `/home/sergi/repositorios/APS/APS/Documentos disponibilizados no site/` e devem ser cruzados com este conteúdo.)

## 1. A cadeia de rastreabilidade de artefatos é a espinha dorsal do trabalho
As Aulas 10 e 11 fixam, pelos títulos dos slides do estudo de caso, uma **ordem de derivação obrigatória** que o projeto deve reproduzir:

**Diagrama de Casos de Uso → Especificação textual dos Casos de Uso → Diagrama de Classes → Diagrama de Sequência (um por caso de uso) → Diagrama de Estados (um por classe participante, por caso de uso) → Diagrama de Atividades (um por estado que precise de detalhamento algorítmico)**

Consequências práticas para o trabalho:
- O número de DEs **não** é um por sistema: é **um por classe, por caso de uso**. Planeje a quantidade de diagramas cedo, pois multiplica rapidamente.
- Cada diagrama deve ser **titulado no formato do professor**:
  - `Diagrama de Estados: classe <Classe> em <Caso de Uso>`
  - `Diagrama de Atividades para o estado '<Estado>', dentro do Diagrama de Estados para a classe <Classe> em <Caso de Uso>`
- O DE deve ser **criado a partir do Diagrama de Sequência** ("Criar o DE para Paciente"), então as mensagens do DS devem virar eventos/transições do DE — consistência entre os dois será verificável.

## 2. O template de Especificação de Caso de Uso é a peça mais cobrável (Aula 12)
É o único "quadro/formulário" com formato literal nas aulas, e ele alimenta tudo o mais:
- Cabeçalho fixo: **Diagrama de Caso de Uso / Ator Principal / Ator de Suporte / Descrição / Pré-condições / Pós-condições**
- **Fluxo Básico numerado**, com **rotulagem obrigatória da função de cada elemento entre parênteses**: `(ator)`, `(sistema)`, `(classe X)`, `(atributo)` — é literalmente uma regra dita em slide ("Deve-se identificar a função de cada elemento").
- Os rótulos `(classe X)` são o que **liga a especificação ao Diagrama de Classes** — atributos citados na tela precisam existir nas classes.
- **Fluxo Alternativo** com dois padrões: `*` para os "a qualquer momento" (cancelar/limpar) e `<passo>.<letra>.<n>` para exceções ancoradas, sempre com retorno explícito ao Fluxo Básico.
- **Regras de Negócio numeradas**, referenciadas dentro do fluxo básico ("conforme Regras de Negócio 1 e 2").
- Extra crítico: **a contagem de transações do fluxo básico + alternativo e o número de objetos/entidades acessados definem a complexidade do caso de uso na estimativa UCP** — ou seja, a especificação bem-feita é pré-requisito numérico da estimativa.

## 3. A estimativa (Aula 12) é entregável de planejamento — provavelmente do Plano do Projeto (1º bim)
Para o Plano do Projeto, deve-se conseguir produzir, com números do próprio sistema:
- Tabela de **atores classificados** (simples/médio/complexo) → **UAW**
- Tabela de **casos de uso classificados** (simples/médio/complexo, justificados por nº de transações e nº de objetos/entidades) → **UUCW**
- **UUCP = UAW + UUCW**
- Tabela **F1..F13** com valor (0-5) × peso → **TFactor** → **TCF = 0,6 + 0,01×TFactor**
- Tabela **F1..F8** de ambiente (atenção aos **pesos negativos** de F2 e F7) → **EFactor** → **ECF = 1,4 − 0,03×EFactor**
- **UCP = UUCP × TCF × ECF** e **esforço = UCP × 20 h** (com a alternativa de 5 h/UCP apresentada em aula)
- O esforço em horas é o que sustenta **cronograma, alocação de equipe e custo** no Plano do Projeto.
- **Planning Poker** é a alternativa/complemento para estimar itens de backlog (cartas Fibonacci 0,1,2,3,5,8,13,21,34,55,89; justificativa obrigatória de maior e menor estimativa; moderador = Scrum Master; cliente opcional).

## 4. Gestão do projeto (Aulas 13 e 14) fornece o "como o projeto será conduzido"
O que a Aula 13 lista como conteúdo da fase de **Planejamento** do Scrum é praticamente um sumário de Plano do Projeto:
- **Projeto da Arquitetura do Sistema**
- **Estimativas de datas e custos** (encaixa diretamente com o UCP da Aula 12)
- **Criação do Product Backlog** com levantamento de requisitos e priorização (pode conter tarefas diretas, **Casos de Uso** e **Histórias de Usuário**)
- **Definição de equipes e seus líderes** (papéis: PO, Scrum Master, Team)
- **Definição de pacotes a serem desenvolvidos**

Para o Projeto do 2º bimestre (execução), os parâmetros a declarar/planejar:
- **Sprints de 1 a 4 semanas** (XP: iterações de 1 a 3 semanas), **backlog congelado durante o sprint**, **executável ao final de cada sprint**
- Cerimônias com timeboxes: planejamento **8 h (2 × 4 h)**, daily **15 min** (3 perguntas), revisão **≤ 4 h** (prep ≤ 1 h), retrospectiva **≤ 3 h** (2 perguntas)
- **Sprint Backlog atualizado e público**
- Regra de escopo/prazo: **a data de entrega deve ser obedecida; é permitida a diminuição de funcionalidades** — orientação direta para negociar escopo do trabalho em vez de atrasar
- Na revisão, **apresentar funcionalidades, não artefatos** — na apresentação final, demonstrar software funcionando
- **Encerramento**: testes de integração, testes de sistema, documentação do usuário, material de treinamento, material de marketing — checklist plausível de itens finais de entrega
- Da XP (Aula 14), práticas declaráveis no plano de qualidade/processo: **TDD (vermelho→verde→refatorar)**, **integração contínua**, **programação em pares**, **design simples**, **propriedade coletiva do código**, **ritmo sustentável**, **cliente presente**; e a justificativa de **testes automatizados como documentação viva**

## 5. Riscos/armadilhas identificáveis a partir das aulas
- Esquecer os rótulos `(ator)/(sistema)/(classe X)/(atributo)` na especificação — é regra explícita.
- Fazer **um único** Diagrama de Estados para o sistema — o professor exige por classe e por caso de uso.
- Usar símbolo não-UML sem declará-lo (o professor anota "Não é UML" quando usa um).
- Confundir Diagrama de Atividades com Diagrama de Sequência: **o DA não representa processos completos**, apenas os passos de um objeto **dentro de um estado**.
- Aplicar pesos de ambiente sem observar que **F2 (colaboradores de meio período) e F7 (dificuldades com a linguagem) têm peso −1**, e que ECF usa coeficiente **negativo** (−0,03).
- Classificar caso de uso por "tamanho da tela" em vez do critério do professor: **nº de transações (incluindo fluxo alternativo) + nº de objetos/entidades acessados**.

## Arquivos analisados
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula10.pdf`
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula11.pdf`
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula12.pdf`
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula13.pdf`
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula14.pdf`

Documentos de trabalho relacionados encontrados no repositório (não analisados nesta tarefa): `/home/sergi/repositorios/APS/APS/Documentos disponibilizados no site/PlanodoProjeto - 1o bim.doc`, `.../PlanodoProjeto - 1o  bimestre.pdf`, `.../PlanodoProjeto - 2obim.doc`, `.../PlanodoProjeto - 2o bimestre.pdf`.
