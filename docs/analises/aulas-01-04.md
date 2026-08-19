# Resumo detalhado — APS (UTFPR, Prof. Laudelino Cordeiro Bastos) — Aulas 01 a 04

Arquivos lidos (todos em `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/`): `APSECAula01.pdf` (57p), `APSECAula02.pdf` (62p), `APSECAula03.pdf` (49p), `APSECAula04.pdf` (60p).
Extração feita via `pypdf` (o Read nativo falhou: `pdftoppm`/poppler ausente).

---

## Aula 01 — "Análise e Projeto de Sistemas: Introdução"

### Tópicos cobertos
- Apresentação do professor (Doutor POLI-USP em Sistemas de Informação Hospitalar, 2002; Mestre CEFET-PR em Processamento de Imagens, 1995; Eng. Computação PUCPR, 1991).
- Motivação: por que fazer Análise e Projeto de Sistemas.
- Ementa e bibliografia da disciplina.
- **Método de avaliação** (slide 10 — ver observação abaixo).
- Software: definição, evolução histórica (1950→hoje), características.
- Dado / Informação / Conhecimento.
- Definições: Análise, Processo, Sistema; Teoria Geral de Sistemas; tipos de sistemas.
- Papéis (usuário, cliente, analista) e problemas do desenvolvimento de sistemas.

### Conceitos-chave
- **Justificativa da disciplina**: SAP faturou €30 bi em 2022 com sistema de +400 milhões de LOC — "é inviável desenvolver um sistema desse porte sem análise e projeto".
- **Dados de falha (Standish Group 2015, 8380 projetos)**: 31% cancelados antes do término; 52% custam 189% a mais que o orçamento; apenas **16%** terminam no prazo e no orçamento. O relatório aponta **requisitos de software** (levantamento, priorização, elicitação) como principais responsáveis pelas falhas. Também cita o estudo ESPITI/1995 (3800 respostas) sobre importância relativa dos problemas de software.
- **Ementa oficial**: Introdução; Teoria Geral de Sistemas; Conceitos de A&P de Sistemas; Paradigmas de A&P; Ferramentas de A&P; Critérios em Projetos de Sistemas; Estágios e Objetivos do Projeto; **Técnicas de documentação**.
- **Dado / Informação / Conhecimento**: `40` (dado) → `40 °C` / "o paciente está com febre" (informação) → "tomar uma decisão" (conhecimento).
- **Software é formado por três coisas**: (1) Instruções (programas), (2) Estruturas de dados, (3) **Documentos que descrevem a operação e o uso dos programas**. (Ponto reiterado: documentação é parte do produto.)
- **Características do software**: é desenvolvido por engenharia, não manufaturado (custos concentrados no trabalho de engenharia — projetos de software não podem ser geridos como projetos de manufatura); **não se desgasta, mas se deteriora** por causa das modificações; a maioria é feita sob medida, não montada de componentes.
- **Curvas de falha**: curva do hardware ("mortalidade prematura" + "desgaste"); curva idealizada do software (taxa constante até obsolescência); **curva real do software** — picos súbitos de falha a cada mudança de requisito, com patamar residual sempre acima do anterior. Cita Brooks (*The Mythical Man Month*, 1995): ao corrigir um defeito há **20% a 50% de chance de introduzir outro**.
- **Não existe "peça de reposição" para software**: toda falha indica erro de projeto ou de tradução para código executável; manutenção de software é mais complexa que a de hardware.
- **Eras do software** (linha do tempo 1950→2010): 1ª era (batch, distribuição limitada, software customizado); 2ª era (multiusuário, tempo real, banco de dados, software houses); 3ª era (sistemas distribuídos, inteligência embarcada, hardware barato/PCs, workstations); 4ª era (tecnologia OO, sistemas especialistas, redes neurais, computação paralela, WWW).
- Marco histórico enfatizado: **por volta de 1970, Tom DeMarco desenvolve a Engenharia de Software baseada em modelos** — "visualizar o funcionamento de um sistema futuro antes que ele seja construído", quebrando o paradigma do "começar já codificando". Esse é o argumento central que sustenta todo o trabalho da disciplina.
- **Análise** (do grego *analýein*, desatar): dissolução de um conjunto em suas partes; estudo de um problema que antecede a ação de resolvê-lo; **propósito é modelar um sistema de forma que possa ser entendido**.
- **Processo**: série de fenômenos sucessivos com relação de causa e efeito.
- **Sistema**: grupo de itens que interagem/interdependem formando um todo unificado.
- **Teoria Geral de Sistemas**: Ludwig von Bertalanffy (1968); auxiliou a criação da Análise e Projeto Estruturado (DeMarco e Yourdon).
- **Tipos de sistemas**: naturais × feitos pelo homem (sociais, transporte, comunicação, manufatura, financeiros, automatizados); concretos × abstratos; **fechados × abertos** (software = aberto); emergentes × **teleológicos** (planejados, dirigidos por objetivos — software é teleológico; exemplo da casa do castor).
- **Definição operacional da disciplina** (slide 54, decorar): *"Análise e Projeto de Sistemas representa o estudo detalhado (análise) de uma área de trabalho (processo) e o desenvolvimento (projeto) de um conjunto de softwares integrados (sistema) destinado à execução, controle e acompanhamento do processo."*
- **Participantes e responsabilidades**: **usuário** (utilizará o sistema; responsável pela decisão de integrá-lo às operações; **somente ele pode aceitar ou não o sistema**); **cliente** (contrata os serviços da desenvolvedora); **analista** (responsável por estudos de viabilidade e alternativas, relações custo/benefício, especificações, prazos e testes de aceitação).
- **Problemas no desenvolvimento**: produtividade, demanda reprimida, tempo de desenvolvimento, descontinuidade, confiabilidade, manutenção, eficiência, portabilidade, segurança.

### Bibliografia exigida/indicada
Pressman (*Engenharia de Software*, 7ª ed., 2011); Sommerville (9ª ed., 2011); Fairley (1985); Gane & Sarson (*Análise Estruturada de Sistemas*); Yourdon (*Análise Estruturada Moderna*); Davis (*A&P de Sistemas — abordagem estruturada*); Heuser (*Projeto de Banco de Dados*); Silberschatz & Korth; Teorey/Lightstone/Nadeau; **Booch, Rumbaugh & Jacobson — *UML: Guia do Usuário*** (referência normativa para notação UML da disciplina); Groff & Weinberg (*SQL, the complete reference*).

### Avaliação
- O slide 10 é intitulado **"Método de avaliação"**, mas **o conteúdo não é extraível do PDF** (a página contém apenas o título rotacionado no content stream; não há imagem embutida, XObject nem anotação). O quadro de avaliação provavelmente foi apresentado em aula ou em outro documento (p.ex. plano de ensino). **Não é possível recuperar pesos/notas a partir deste arquivo** — recomenda-se buscar o plano de ensino ou as aulas 05+ / arquivos do projeto.

---

## Aula 02 — "Análise de Requisitos de um Projeto: Entrevistas, Questionários, Reuniões e Observação / Estudo de Viabilidade"

### Tópicos cobertos
Técnicas de elicitação (entrevista, questionário, reunião, observação); Estudo de Viabilidade (4 categorias); PMBOK como guia para projetos (Collect Requirements); vídeos/reflexões (Cortella, sonda Mangalyaan, Alegoria da Caverna); **regras do trabalho** (slide 61).

### Conceitos-chave

**Entrevista**
- Definição: conversa entre duas pessoas, provocada por uma delas, com objetivo definido. Proporciona contato pessoal que faz o entrevistado se sentir parte influente do processo.
- Objetivos: obter informações registradas **apenas na memória** e informações sobre os integrantes do processo.
- **Preparação**: conhecer o assunto (manuais, revistas, normas, legislação, **RFP – Request for Proposal**, outros sistemas semelhantes); com acesso à organização: planos de negócio, fluxos de processos atuais, modelos de dados lógicos. Identificar as pessoas (posição na hierarquia, poder de decisão, interesse, participação em outras atividades). **Elaborar agenda** (marcar com antecedência, pauta, local/horário/duração, confirmar um dia antes). **Elaborar roteiro** (objetivos, principais tópicos, prioridade dos tópicos, nível de informação desejado).
- **Durante**: vestir-se adequadamente; conquistar confiança; evitar desvio de assunto; escutar mais e ser neutro; esclarecer a pessoa; perguntas gerais seguidas de complementares; dar espaço a dúvidas; anotar se possível; deixar abertura para novos contatos.
- **Após**: revisar a entrevista; **elaborar uma ata dos assuntos discutidos e pedir aprovação do cliente/usuário** (convenção de documentação exigida).
- **Limitações**: despreparo do entrevistado; respostas agradáveis; palpites e respostas falsas; tomar notas durante a entrevista; escolha da pessoa incorreta; **ambiguidade** (significado atribuído pelo analista difere do pretendido pelo cliente); **conhecimento tácito** (o cliente sabe mas não transmite).
- **Classes de conhecimento dos requisitos** (Ferrari, Spoletini & Gnesi, *Requirements Engineering*, 2016): (1) conhecida pelo analista e pelo cliente; (2) conhecida pelo analista e não pelo cliente; (3) **não conhecida pelo analista e conhecida pelo cliente = conhecimento tácito**; (4) não conhecida por nenhum dos dois.

**Questionário**
- Definição: conjunto de perguntas com respostas objetivas ou graduais, em sequência lógica e progressiva.
- Preparação: introdução com objetivos; instruções de preenchimento; estimativa de duração; prazo de devolução; contatar superior hierárquico quando necessário.
- Perguntas claras (linguagem clara, frases concisas, termos precisos) e simples/diretas (preferir respostas fechadas).
- Tipos: **pergunta complementar** (esgota um tema por sequência lógica) e **pergunta encadeada** (testa a coerência das respostas). **Fazer teste-piloto com os usuários antes de aplicar.**
- Limitações: resistência ao preenchimento; frios e impessoais; respostas distorcidas.

**Reunião**
- Definição: intercâmbio de ideias, sugestões e opiniões visando aceitação de um ponto de vista.
- Objetivos: comunicação direta; integração do grupo; definição/esclarecimento de problemas; tomada de decisão; troca de experiências.
- Ambiente físico: sala ampla/ventilada/iluminada, mesa central, cadeiras, quadro, flipchart, material de consumo, aviso "Reunião" na porta, checagem dos equipamentos.
- Abertura: apresentação do condutor, do objetivo e agenda, dos participantes; **determinar regras** e **estabelecer papéis**.
- Tópicos: exposição do problema; intercâmbio de experiências; levantamento de causas e consequências; levantamento de alternativas; avaliação das consequências de cada alternativa; escolha; busca de consenso; **definição de responsáveis e prazos**.
- Recomendações e limitações (deslocamento, faltas/atrasos, domínio por um indivíduo, comparecimento desnecessário, ambiente inadequado, não participação por falta de confiança).
- **Características do condutor**: conciliador; abre espaço; administra conflitos; esclarece definições; controla pauta e tempo.
- **Técnicas de perguntas** (nomenclatura exigida): **dirigida** (a um participante), **geral** (a todos), **redistribuída** (feita ao condutor e devolvida a um participante/ao grupo), **reversa** (o condutor devolve a quem formulou).

**Observação**
- Definição: técnica que utiliza os sentidos para obter aspectos da realidade.
- Objetivos: confirmar informações das entrevistas/reuniões/literatura comparando com a realidade; **levantar volumes (quantidades) de informação**; identificar o **fluxo de documentos** ao longo das tarefas.
- Recomendações: colocar-se no ambiente sem interferir; pedir que expliquem a rotina; verificar se os manuais correspondem ao serviço realizado; observar em período normal.
- Cuidados/sinais: empregados fora de sua atividade; confusão/ruído além do normal; chefes com mesa cheia de papéis; pessoas perambulando; ausência de funcionários.

**Conclusão da parte de requisitos**: além do domínio das técnicas, "realizar análise de requisitos requer dedicação, paciência, facilidade para o diálogo e sensibilidade".

**Estudo de Viabilidade**
- Definição: estudos preliminares que investigam as necessidades de informação dos potenciais usuários e determinam requisitos de recursos, relações custo/benefício e viabilidade do projeto. Métodos de coleta: entrevistas/questionários com funcionários, usuários, clientes, gerentes; observação pessoal, gravação em vídeo ou envolvimento nas atividades; exame de documentos, relatórios, manuais de procedimentos e registros; desenvolvimento/simulação/observação de um modelo das atividades de trabalho.
- **Quatro categorias de viabilidade** (estrutura fixa e cobrada):
  1. **Organizacional** — eficácia com que o sistema proposto apoia os objetivos estratégicos da organização.
  2. **Econômica** — economia de custos, aumento de receita, redução de investimento, aumento de lucros. **Custos**: tangíveis (hardware, software, salários) e intangíveis (satisfação do cliente e dos usuários finais). **Benefícios**: tangíveis (aumento de vendas, redução de custos operacionais, aumento de eficiência) e intangíveis (disponibilidade de informações, posição competitiva, imagem da organização, melhor atendimento ao cliente).
  3. **Técnica** — capacidade, confiabilidade e disponibilidade de hardware, software e rede; **capacidade do pessoal que vai fazer e que vai utilizar o sistema**.
  4. **Operacional** — aceitação dos usuários finais; apoio administrativo; requisitos dos clientes, fornecedores e governo.
- **Exemplos de questões para elicitação por categoria** (modelo de roteiro):
  - Organizacionais: quais as principais necessidades do cliente? quais os principais benefícios que o projeto trará?
  - Econômicas: qual será aproximadamente o custo do projeto? existe mercado real para o produto?
  - Técnicas: quais ferramentas de software e hardware serão utilizadas? o cliente dispõe de infraestrutura suficiente para sustentar o novo sistema?
  - Operacionais: quais são os futuros usuários? os futuros usuários apoiam o desenvolvimento do sistema?

**PMBOK**
- PMBOK = *Project Management Body of Knowledge*, do Standards Committee do **PMI**; padrão ANSI desde 1999. A estrutura de gerenciamento é decomposta em **áreas de conhecimento** — "nove áreas" no modelo citado. Slides 46–47 mostram o processo **Collect Requirements (PMBOK 4ª edição)** com sua visão geral **Inputs / Tools & Techniques / Outputs**. Indicação de leitura: revista *Project Management Journal* no Periódicos CAPES.

**Reflexões/vídeos**: "Dois engenheiros e vários milhões" (Mário Sérgio Cortella); sonda indiana **Mangalyaan** (US$ 74 mi vs. MAVEN US$ 671 mi; princípio *jugaad* — solução mais engenhosa e menos onerosa; truque de orbitar a Terra por um mês para ganhar velocidade); **Alegoria da Caverna de Platão** — "nem tudo é o que parece" (11 slides), metáfora para requisitos aparentes vs. reais.

### ⚑ Sobre o trabalho (slide 61) — REGRAS EXPLÍCITAS
- **Não podem ser realizados projetos relativos a: videolocadoras e similares.**
- **A linguagem deve ser orientada a objetos: C++ ou Java** (e suas linguagens derivadas).
- **A implementação do projeto deve ter classes e métodos com correspondência no trabalho** (ou seja, o código tem de refletir o modelo documentado — rastreabilidade modelo↔código).
- **Não pode ser utilizada a linguagem PHP.**
- **Equipes de 3 ou 4 alunos**; a formação e dissolução das equipes é responsabilidade dos alunos.

---

## Aula 03 — "O Processo de Software"

### Tópicos cobertos
Abrangência da Engenharia de Software; métodos, ferramentas, procedimentos; paradigmas/ciclos de vida: Cascata, Prototipação, Espiral; **Processo Unificado (UP)** — fases e fluxos de trabalho; RUP.

### Conceitos-chave
- **Três elementos fundamentais da Engenharia de Software**: **Métodos, Ferramentas e Procedimentos (Processos)**. Objetivos: dar ao gerente o controle do processo e ao profissional uma base para construir software de alta qualidade.
- **Métodos** ("como fazer"): planejamento e estimativa de projeto; análise de requisitos; projeto da estrutura de dados; arquitetura do programa e algoritmos; codificação; testes; manutenção.
- **Ferramentas**: suporte automatizado/semi-automatizado; integradas formam **CASE (Computer Aided Software Engineering)**.
- **Procedimentos**: elo entre métodos e ferramentas — definem a **sequência** de aplicação dos métodos; **os produtos que se exige que sejam entregues (documentos, relatórios, formulários, diagramas, código)**; os controles de qualidade e de alterações; os **marcos de referência** que permitem administrar o progresso. (Este slide é a base conceitual do formato de entrega dos trabalhos.)
- **Paradigmas = Ciclos de Vida = Modelos de Processo de Software**; são uma estratégia de desenvolvimento.
- **Escolha do modelo de processo**: adequação do modelo à aplicação; métodos e ferramentas; controles e produtos a entregar; características do processo (visibilidade, clareza, apoio de ferramentas, produtividade, qualidade).
- **Cascata (Ciclo de Vida Clássico)**: fases em separado com passagem formal; **cada fase envolve um ou mais documentos que devem ser aprovados antes de iniciar a seguinte**; na prática permite-se retorno à fase anterior. Fases: **Engenharia de Sistemas → Análise de Requisitos → Projeto → Codificação → Testes → Manutenção**. Problemas: projetos reais raramente seguem o fluxo sequencial; é difícil estabelecer todos os requisitos no início (incerteza natural); o cliente só vê versão executável em etapa avançada. Vantagens: visibilidade do processo; "significativamente melhor do que uma abordagem casual".
- **Prototipação**: modelo que serve para identificar requisitos (protótipo de interação com o usuário ou implementando algumas funções).
  - **Baixa fidelidade** = esboços sem detalhes → **mockups / wireframes**; **alta fidelidade** = nível alto de detalhe de aparência, construídos na própria ferramenta de desenvolvimento.
  - **Vertical** (visão profunda e específica) × **Horizontal** (visão ampla e rasa).
  - Apropriada quando o cliente definiu objetivos gerais mas não detalhou entrada/processamento/saída, ou quando o desenvolvedor não tem certeza da eficiência de um algoritmo ou da forma de interação homem/máquina.
  - **Ciclo (6 atividades)**: Coleta e refinamento dos requisitos → Projeto rápido → Construção do protótipo → Avaliação do protótipo pelo cliente → Refinamento do protótipo → Engenharia do produto.
  - Problemas: o cliente "força" o protótipo como produto final; o desenvolvedor faz implementação comprometida e depois se esquece de que as escolhas eram inadequadas. **Regra de ouro**: "definir as regras do jogo logo no começo" — cliente e desenvolvedor concordam que o protótipo serve para definir requisitos; após identificados os requisitos, **o protótipo deve ser descartado** e a versão de produção construída com critérios de qualidade.
- **Espiral**: combina Cascata + Prototipação e adiciona a **Análise dos Riscos**; estrutura iterativa. **Quatro atividades por volta**: **Planejamento** (objetivos, alternativas e restrições) → **Análise de Risco** (análise das alternativas, identificação e resolução dos riscos) → **Construção/Engenharia** (desenvolvimento do produto no nível seguinte) → **Avaliação do Cliente** (avaliação do produto e planejamento das novas fases), com decisão de **prosseguir/não prosseguir**. Comentários: realística para grande escala; exige experiência na determinação de riscos; difícil convencer clientes de que a abordagem evolutiva é controlável.
- **Processo Unificado (UP)** — modelo central da disciplina:
  - Incorpora prototipação, iteração e entrega incremental; usa **orientação a objetos** e é "projetado e documentado utilizando a **notação UML** para ilustrar os processos em ação".
  - **4 Fases**: **Concepção/Iniciação (Inception)** — ênfase no **escopo**; **Elaboração** — ênfase na **arquitetura**; **Construção** — ênfase no **desenvolvimento**; **Transição** — ênfase na **implantação**.
  - **5 Fluxos de Trabalho do Processo (Disciplinas)**: **Requisitos, Análise, Projeto, Implementação, Testes**. As iterações ocorrem executando os fluxos de trabalho de cima para baixo; matriz Fases × Fluxos com Iteração 1..n.
  - **Concepção**: foco na comunicação com o cliente para identificação de requisitos e no planejamento para delimitar o escopo; **identificar todas as entidades externas com que o sistema interage (atores)**, definir a natureza da interação em alto nível, **identificar todos os casos de uso e o plano do projeto**; protótipos podem apoiar a comunicação.
  - **Elaboração**: analisar o domínio do problema, estabelecer a arquitetura, **refinar o plano do projeto** e identificar os maiores riscos; foco em análise e projeto; decisões de arquitetura exigem compreensão de escopo, principais funcionalidades e requisitos não funcionais (ex.: desempenho).
  - **Construção**: demais componentes e características desenvolvidos e integrados, exaustivamente testados; ênfase em gerenciamento de recursos e controle de operações (custos, cronograma, qualidade); transição da propriedade intelectual para **software e documentação a serem entregues**.
  - **Transição**: do ambiente de desenvolvimento para o de produção/usuários; surgem novas versões, correções e características adiadas.
  - **Requisitos (fluxo)**: descrever *o que* o sistema deve fazer — obter, organizar e documentar funcionalidades e restrições. **A modelagem de Casos de Uso representa os Requisitos Funcionais.** Um Caso de Uso especifica uma sequência de ações, **incluindo caminhos alternativos**, que leva a um resultado observável produzido por um determinado ator.
  - **Análise (fluxo)**: analisar os requisitos e montar o **modelo de classes e de objetos, com foco nas classes de negócio, mais o dicionário de informações**. **O Modelo de Casos de Uso é a entrada para o Modelo de Análise**; o Modelo de Análise é a especificação detalhada dos requisitos, definindo os casos de uso como colaboração entre tipos conceituais de objetos.
  - **Projeto (fluxo)**: descrição da implementação a partir do Modelo de Análise, adequada ao ambiente de implementação (distribuição de objetos, ambiente gráfico, bancos de dados, reuso de legados, bibliotecas). **"São incluídos os Diagramas de Sequência, de Estado e de Atividades."** ← define quais diagramas UML entram na etapa de Projeto.
  - **Implementação**: organizar o código em subsistemas **organizados em camadas**; implementar classes e objetos em termos de componentes (fontes, binários, executáveis); testar componentes como unidades; integrar resultados em sistema executável.
  - **Testes**: verificar a interação entre objetos; a integração adequada de todos os componentes; se todos os requisitos foram corretamente implementados; garantir que defeitos sejam encontrados e corrigidos antes da implantação.
  - **RUP**: acrescenta ao UP os fluxos **Modelagem de Negócios, Requisitos, Análise e Projeto, Implementação, Testes, Implantação** e os **Fluxos de Trabalho de Suporte: Gerência de Configurações e Alterações, Gerenciamento de Projeto, Ambiente**. Observação sobre nomenclatura: usa-se "Processo Unificado" para o processo genérico, também para evitar problemas de marca registrada (RUP é marca da IBM).

### Menções a trabalhos/entregas
Sem menção explícita a nota, mas duas passagens são normativas para os trabalhos: os **Procedimentos** definem "os produtos que se exige que sejam entregues (documentos, relatórios, formulários, diagramas, código)" e a fase de **Elaboração** exige "**refinar o plano do projeto**" — o Plano do Projeto é artefato de Concepção, refinado na Elaboração.

---

## Aula 04 — "Requisitos / Diagramas de Casos de Uso (Use Case Diagram)"

### Tópicos cobertos (slide 2, declarados pelo professor)
Requisitos; Atores; Casos de Uso; **Include, Extend e Generalização**; **Cenário Descritivo ou Especificação de Casos de Uso**. Mais dois estudos de caso completos: **Controle de Leitos de UTI** e **EaD Remota Tablet (PEDRO)**.

### Conceitos-chave

**Requisitos**
- Definições: Aurélio ("condição necessária para a obtenção de certo objetivo") e **IEEE 610-1990** (condição/capacidade de que um usuário necessita para resolver um problema ou atingir um objetivo; ou condição/capacidade que o sistema precisa atingir para satisfazer contrato, norma, especificação ou outro documento).
- **Elicitação**: descobrir o domínio da aplicação, os serviços a fornecer e as restrições; envolve cliente, usuários finais, gerentes, pessoal de manutenção, especialistas do domínio — as **Partes Envolvidas / Stakeholders**.
- **Tipos (Leffingwell & Widrig, 2002)** — Requisitos de Software: **Funcionais**, **Não Funcionais**, **Restrições de Projeto**.
  - **Funcionais**: descrevem o comportamento do sistema, suas ações para cada entrada — *o que* tem que ser feito. Orientados a ações: "quando o usuário executa x, o sistema faz y". Podem ser escritos como frase declarativa simples **ou na forma de um Caso de Uso**.
  - **Não Funcionais**: expressam **como** deve ser feito o sistema; definem se o sistema será eficiente; apresentam restrições e especificações de uso para os requisitos funcionais.
  - **Restrições de Projeto**: impõem limitações sobre o projeto do sistema ou sobre os processos de construção. Recomendação: "sempre que possível, as opções de projeto devem ser deixadas livres para os desenvolvedores"; exemplo de restrição: "desenvolver utilizando Oracle Database Express Edition".
- **Classificação dos RNF — Grady (1992), modelo URPS**: **Usabilidade**, **Confiabilidade (Reliability)**, **Desempenho (Performance)**, **Supportability**.
  - Usabilidade: esforço para aprender, operar, preparar a entrada e interpretar a saída.
  - Confiabilidade: subdividida em **Disponibilidade (Availability)** (ex.: 99% do tempo em funcionamento), **MTBF** (tempo médio entre falhas), **MTTR** (tempo médio para reparação; ex.: 5 minutos), **Acurácia**, **Taxa de Defeitos** (ex.: erros por milhares de linhas de código), **Erros por Tipo** (pequenos, significantes, críticos).
  - Desempenho: desempenho esperado em relação à carga recebida.
  - Supportability = **manutenabilidade e flexibilidade**: facilidade de modificar para acomodar melhoramentos e reparos.
- **Requisitos de Experiência do Usuário (UX)** — categoria adicional exigida pelo professor: relacionam-se à satisfação/prazer do usuário final; abrangem usabilidade, utilidade, emoções, estética, motivações e valores. Base: **Hassenzahl (2003)** — aspectos **pragmáticos** (qualidade instrumental) × **hedônicos** (bem-estar psicológico); **Maier & Berry (2018)** — sintetização de características para classificar um requisito de experiência. Exemplos: "o usuário final deve se sentir no controle"; "o sistema deve ter um design minimalista"; "o sistema deve facilitar o acesso rápido a notícias de moda"; "o sistema deve defender a reciclagem".

**⚑ Convenção de nomenclatura de requisitos (slide 17 — exemplos canônicos)**
```
RF01  – O médico pode internar um paciente.
RF02  – O usuário pode pesquisar todo ou um subconjunto do banco de dados.
RNF01 – O médico pode internar um paciente que já esteja cadastrado.
RNF02 – As consultas ao banco de dados não devem ultrapassar 3 segundos.
RP01  – Desenvolver o projeto utilizando o SGBD MySQL.        (Restrição de Projeto)
RE01  – O médico deve se sentir seguro ao utilizar o sistema. (Requisito de Experiência)
```
Observação importante de coerência: no estudo de caso da UTI (slides 44–45) o professor usa a variante **FR01…FR07** para requisitos funcionais, e nas especificações usa **UC001** e **RN001/RN002** com três dígitos, enquanto no diagrama usa **UC01…UC07**. Ou seja, aceita-se prefixo + número sequencial; **o essencial é manter um padrão único e consistente em todo o documento e rastrear RF ↔ UC 1:1 (ou explicitar quando um RF gera mais de um UC)**.

**Diagrama de Casos de Uso**
- Serve às atividades de negócio e à representação dos requisitos; descreve **o que** o sistema deverá fazer (ou o que o sistema existente faz); agrupa requisitos, descreve funcionalidades e representa **os papéis de negócio** que interagem com elas. **Define inicialmente o que o sistema faz, não como** — não se preocupa com implementação, mas com as necessidades do negócio.
- **Atores**: agentes que interagem com o sistema; **entidades externas** (jargão da análise estruturada essencial) com interesse em interagir; representam **papéis no negócio**.
  - **Notação exigida**: ícone humano (*stick figure*) **ou** retângulo com o estereótipo **`<<ator>>`** (ex.: `<<ator>> Impressora`) — "sempre indicando que se trata de um ator".
  - Um ator pode ser: tipo de usuário (Gerente, Motorista, Profissional de Saúde), **hardware** (eletrocardiógrafo, controlador de temperatura, GPS) ou **software** (sistema de estoque, folha de pagamento, prontuário eletrônico, banco de dados).
  - **Três tipos de ator** (classificação cobrada e usada nas especificações):
    - **Ator Principal** — tem objetivos de usuário satisfeitos pelo uso do sistema. Identificar para encontrar os objetivos do usuário, que guiam os casos de uso.
    - **Ator de Suporte** — fornece um serviço (p.ex. informações) ao sistema. Identificar para esclarecer interfaces externas e protocolos de comunicação.
    - **Ator de Bastidor** — tem interesse no comportamento do caso de uso mas não é principal nem de suporte (p.ex. um órgão governamental). Identificar para garantir que todos os interesses estejam identificados e satisfeitos (interesses sutis, de fácil esquecimento).
- **Caso de Uso**: funcionalidade requerida do sistema; cada caso de uso representa **uma funcionalidade completa conforme percebida pelo ator**; **não confundir com módulo ou função**.
- **Perguntas para encontrar casos de uso**: quais funções o ator necessita? o que o ator precisa fazer? o ator precisa criar/apagar/ler/armazenar/modificar alguma informação? precisa ser notificado de eventos do sistema? precisa notificar o sistema sobre eventos? o trabalho diário do ator poderia ser simplificado/tornado mais eficiente? quais entradas e saídas o sistema necessita? quais os principais problemas do método/sistema atual?
- **Relacionamentos** — podem envolver dois atores, um ator e um caso de uso, ou dois casos de uso.
  - **Entre atores**: como atores são entidades externas, as relações entre eles não fazem parte do sistema, mas podem ser incluídas para auxiliar a representação do **modelo de negócios**. Tipos: **comunicação/associação** e **generalização/especialização** (um ator é caso específico de outro).
  - **Ator ↔ Caso de uso**: expressa comunicação; **os atores se comunicam com o sistema sempre através dos casos de uso**; as setas podem indicar a ativação do caso de uso ou o sentido do fluxo de dados.
  - **Entre casos de uso** (normalmente **não** são de comunicação — casos de uso são aplicações completas que se comunicam pelo banco de dados do sistema):
    - **Inclusão (`include`)**: reutilização do mesmo conjunto de passos por casos de uso diferentes. "Fica clara quando o analista se depara copiando e colando o mesmo conjunto de passos em use cases diferentes". **Se o reaproveitamento não ocorrer, não é necessário representar esta estrutura.** O caso de uso *include* **sempre é chamado** pelo caso de uso origem.
    - **Extensão (`extend`)**: conjunto de passos realizados **somente sob determinada condição**. Diferentemente do include, **nem sempre será executado**; o caso de uso estendido **pode acessar e modificar propriedades do caso de uso base** (o que não acontece no include). Exemplo do professor: "envia via SEDEX apenas se houver urgência".
    - **Generalização**: relacionamento entre um caso de uso geral e outro mais específico; o geral é abstração dos específicos.
- **Diagrama com fronteira do sistema**: o exemplo do slide 29 mostra a caixa **"Sistema de Caixa Eletrônico"** contendo os casos de uso *Apresentar o saldo*, *Imprimir o extrato*, *Realiza saque*, com o ator **Cliente** à esquerda e `<<ator>> Impressora` à direita — ou seja, **espera-se a fronteira do sistema nomeada e atores de suporte do lado oposto**.

**⚑ Especificação de Casos de Uso (Cenário Descritivo) — TEMPLATE OBRIGATÓRIO**
Slides 34–38. "Normalmente descrito utilizando-se um editor de texto." Estrutura e composição:

| Campo | Conteúdo exigido |
|---|---|
| **Nome** | Nome do use case (ex.: "UC001: Solicitar reserva de leito"; "Emissão de Nota Fiscal") |
| **Atores** | Listados **por tipo**: "Ator Principal: …", "Ator de Suporte: …" |
| **Descrição** | Descrição sucinta do que o caso de uso faz |
| **Pré-condições** | Estado do sistema **no início** do use case. Opcional — "usar apenas quando embutir clareza" |
| **Pós-condições** | Estado do sistema **no final**; **deve ser verdadeira não importa qual ramo/alternativa seja seguido**. Também opcional |
| **Fluxo Básico** | Lista de passos **sob a visão do usuário**, em situações de perfeição, onde a execução de um passo está condicionada ao término de outro(s) |
| **Fluxos Alternativos** | Sequências alternativas de eventos: variação do fluxo básico ou outro cenário relacionado (ex.: uso de Help, limpeza de campos, cancelar) |
| **Fluxos de Exceção** | Ações quando o fluxo **não pode ser concluído** por quebra de regra de negócio, tanto no básico quanto nos alternativos |
| **Regras de Negócio** | Condições/restrições sobre os processos de negócio; **devem ser checadas nos fluxos de eventos do caso de uso** |

**Formato do quadro dos fluxos** (visível nos slides 50–51 e 56–58): tabela de **duas colunas — "Ações dos atores" | "Ações do sistema"** — com passos numerados sequencialmente (1, 2, 3, …), atribuindo cada passo à coluna correspondente. Sub-passos de exceção/alternativa referenciam o passo-mãe com letra: **`5.a.`**, **`2.a.`**, **`9.a.`**, sempre indicando para onde o fluxo retorna ("volta para o Fluxo Básico 3", "o sistema retorna à ação do sistema número 1"). Regras de negócio são referenciadas dentro dos passos entre colchetes: **`[RN001]`**, **`[RN002]`** — e o passo do fluxo básico diz explicitamente "Verifica as informações conforme as Regras de Negócio RN001 e RN002. Caso haja uma regra de negócios não atendida vai para 5.a.". Quando não houver conteúdo, escreve-se **"Não há neste caso de uso."** (não se omite a seção). Fluxos alternativos são **numerados** ("Fluxo Alternativo 1", "Fluxo Alternativo 2", …) e podem ser **nomeados** quando disparam outro caso de uso ("Fluxo Alternativo 2 - Acessar Cursos Disponíveis").

Exemplos de Regras de Negócio no padrão do professor:
```
[RN001] O campo "Nome do paciente" é obrigatório.
[RN002] A "data de nascimento" deve ter o formato dd/mm/aaaa.
```

**Estudo de Caso 1 — Controle de Leitos de UTI (roteiro completo, modelo a imitar)**
1. **Declaração dos Objetivos do Sistema** — *Objetivo geral*: "Controlar a utilização dos leitos de uma UTI". *Objetivos específicos* (lista): controlar o serviço de reservas de leitos; controlar o serviço de liberação de leitos; controlar a limpeza dos leitos; emitir relatórios sobre a reserva de leitos.
2. **Requisitos Identificados** — em frases narrativas do negócio ("Quando o Médico Assistente necessita de uma vaga de UTI, solicita a reserva de vaga"; "O Médico Intensivista recebe dados da nova Solicitação de Reserva de Vaga (SRV) para análise"; etc.).
3. **Atores Identificados** — Médico Assistente, Médico Intensivista, Equipe de Higienização, Profissional de Saúde.
4. **Requisitos Funcionais Identificados** — cada necessidade do negócio é convertida em RF nomeado e numerado (FR01…FR07), no formato **"⟨Ator⟩ pode ⟨verbo⟩ ⟨objeto⟩ – FRxx"**.
5. **Casos de Uso Identificados** — cada RF vira UC no formato **verbo no infinitivo + objeto**: "Solicitar reserva de leito - UC01", "Registrar reserva de leito - UC02", "Cancelar reserva - UC03", "Efetivar a reserva de leito - UC04", "Registrar atendimento - UC05", "Emitir relatórios - UC06", "Conceder alta ao paciente - UC07".
6. **Diagrama de Casos de Uso — "Visão Geral"** (o professor nomeia o diagrama assim: *"Diagrama de Casos de Uso — Visão Geral - Controle Leitos de UTI"*).
7. **Especificação de cada caso de uso** no template acima.

**Estudo de Caso 2 — EaD Remota Tablet (projeto PEDRO)**: mesmo roteiro; "Diagrama de Casos de Uso — Visão Geral - EaD Remota Tablet"; especificação de "UC002: Apresentar o Conteúdo" com pré-condição "Estar autenticado no sistema", pós-condição "Ter feito uma opção da apresentação do conteúdo", fluxo básico de 9 passos e quatro fluxos alternativos.

**⚑ Resumo/receita oficial para elaborar o Diagrama de Casos de Uso (slide 59) — passo a passo cobrado**
1. Identificar os **Requisitos Funcionais** do sistema.
2. **Relacionar** os Requisitos Funcionais com os Casos de Uso do sistema.
3. Identificar os **Atores** do sistema.
4. Identificar as **Associações** entre atores e casos de uso.
5. **Elaborar o Diagrama de Casos de Uso.**
6. **Descrever os cenários** dos Casos de Uso.
7. **Verificar relacionamentos entre casos de uso**: inclusão, extensão e generalização.

---

## Síntese: o que dessas aulas impacta diretamente os trabalhos

### A. Restrições formais do trabalho (regras duras — Aula 02, slide 61)
- **Equipes de 3 ou 4 alunos**, formadas/dissolvidas por responsabilidade dos alunos.
- **Tema proibido**: videolocadoras e similares (evitar também domínios triviais equivalentes: locadoras de qualquer coisa, "CRUD de aluguel").
- **Linguagem obrigatoriamente orientada a objetos: C++ ou Java** (ou derivadas). **PHP é expressamente proibido.**
- **Rastreabilidade documento↔código é critério**: "a implementação do projeto deve ter classes e métodos com correspondência no trabalho" — as classes e métodos do 2º bimestre precisam corresponder ao modelo entregue; nomes devem bater.

### B. O que provavelmente compõe o **Plano do Projeto (1º bimestre)**
A sequência das aulas 01→04 desenha exatamente o sumário de um Plano do Projeto na **fase de Concepção do Processo Unificado** ("identificar todas as entidades externas (atores), identificar todos os casos de uso e o plano do projeto"). Itens que as aulas fornecem prontos:
1. **Identificação do sistema / domínio escolhido** e equipe (3–4 alunos).
2. **Declaração dos Objetivos do Sistema**: objetivo geral (uma frase) + objetivos específicos (lista) — formato do estudo de caso UTI.
3. **Métodos de elicitação empregados** (Aula 02): entrevista (com **agenda**, **roteiro** e **ata aprovada pelo cliente/usuário**), questionário (com introdução, instruções, duração estimada, prazo, teste-piloto), reunião (com pauta, papéis, regras, responsáveis e prazos) e observação (volumes de informação e fluxo de documentos). Documentar quem foi entrevistado e por quê (posição hierárquica, poder de decisão, interesse). Explicitar cuidados com **ambiguidade** e **conhecimento tácito**.
4. **Estudo de Viabilidade nas 4 categorias** — Organizacional, Econômica (custos/benefícios tangíveis e intangíveis), Técnica, Operacional — respondendo às questões-modelo do slide 42. Este é o item de maior probabilidade de cobrança literal no Plano do Projeto.
5. **Requisitos Identificados** (narrativa de negócio) → **Requisitos Funcionais** numerados (RF/FR) → **Requisitos Não Funcionais** classificados por **URPS** (Usabilidade, Confiabilidade com Disponibilidade/MTBF/MTTR/Acurácia/Taxa de Defeitos, Desempenho, Supportability) → **Restrições de Projeto** (RP) → **Requisitos de Experiência do Usuário** (RE, com distinção pragmático/hedônico). Usar exatamente os prefixos e a numeração consistente.
6. **Atores identificados**, classificados em **Principal / Suporte / Bastidor**.
7. **Casos de Uso identificados** (verbo no infinitivo + objeto, com código UCxx), **mapeados 1:1 aos RF**.
8. **Diagrama de Casos de Uso "Visão Geral"** com fronteira do sistema nomeada, `<<ator>>` para atores não-humanos, include/extend/generalização apenas quando realmente houver reuso/condição.
9. **Especificação (Cenário Descritivo) de cada caso de uso** no template de 9 campos, com o quadro de duas colunas "Ações dos atores | Ações do sistema", numeração de passos, sub-passos `n.a.`, `[RNxxx]` referenciadas nos fluxos e "Não há neste caso de uso." onde vazio.
10. **Modelo de processo adotado** (Aula 03): justificar a escolha (Cascata / Prototipação / Espiral / UP) usando os critérios do slide 7 — adequação à aplicação, métodos e ferramentas, controles e produtos a entregar, visibilidade/clareza/produtividade/qualidade. O UP é a escolha "natural" da disciplina porque é OO e documentado em UML.
11. Se for usar protótipos: nomear corretamente — **mockup/wireframe = baixa fidelidade**; classificar como **vertical** ou **horizontal**; e declarar a "regra do jogo" (protótipo serve para definir requisitos e será descartado).
12. **Cronograma/planejamento** por fases e iterações do UP (matriz Fases × Fluxos de Trabalho) e **análise de riscos** se adotar espiral — a Elaboração exige "refinar o plano do projeto e identificar seus maiores riscos".

### C. O que impacta o **Projeto do 2º bimestre**
- Segundo a Aula 03, os artefatos por fluxo de trabalho são:
  - **Requisitos** → Modelo de Casos de Uso (feito no 1º bim.).
  - **Análise** → **modelo de classes e de objetos com foco nas classes de negócio + dicionário de informações**; o Modelo de Casos de Uso é a **entrada** do Modelo de Análise.
  - **Projeto** → **Diagramas de Sequência, de Estado e de Atividades** (lista explícita do professor — esperar esses três).
  - **Implementação** → código organizado em **subsistemas em camadas**, classes/objetos como componentes, testes de unidade, integração.
  - **Testes** → verificar interação entre objetos, integração de componentes, se todos os requisitos foram implementados.
- Portanto: **a qualidade dos casos de uso e das especificações do 1º bimestre determina diretamente o modelo de classes, os diagramas de sequência e o código do 2º bimestre** — os passos numerados do fluxo básico viram mensagens no diagrama de sequência, e as regras de negócio viram validações/métodos.
- A exigência de "classes e métodos com correspondência no trabalho" fecha o ciclo de rastreabilidade: RF → UC → Especificação (passos + RN) → Classe/Método em Java ou C++.

### D. Posturas e argumentos que o professor valoriza (úteis na introdução/justificativa dos trabalhos)
- Citar os números do **Standish Group** e do **ESPITI** para justificar o esforço de análise; atribuir as falhas aos **requisitos**.
- Lembrar que **software é instruções + estruturas de dados + documentação**, e que ele **se deteriora pelas mudanças** (Brooks: 20–50% de chance de introduzir defeito ao corrigir um).
- Explicitar os papéis: **somente o usuário pode aceitar ou não o sistema**; o **analista** responde por viabilidade, custo/benefício, especificações, prazos e testes de aceitação.
- O espírito "jugaad" (Mangalyaan) e a Alegoria da Caverna: soluções engenhosas e de baixo custo; desconfiar das aparências nos requisitos ("nem tudo é o que parece").

### E. Lacunas identificadas
- **O quadro do "Método de avaliação" (Aula 01, slide 10) não é recuperável do PDF** — a página contém apenas o título; não há imagem, XObject ou anotação com o conteúdo. **Nenhuma das quatro aulas menciona nominalmente o documento "PlanodoProjeto"**, pesos de nota, datas de entrega ou critérios de correção. A referência mais próxima é a Aula 03 (fase de Elaboração: "refinar o plano do projeto") e a Aula 02 slide 61 ("Sobre o trabalho"). Recomenda-se procurar o plano de ensino da disciplina, o arquivo-modelo `PlanodoProjeto` e as aulas 05–14 para os critérios de avaliação.
