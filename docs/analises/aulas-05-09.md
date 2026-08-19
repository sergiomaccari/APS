# Aulas 05 a 09 — APS (UTFPR) — Prof. Laudelino Cordeiro Bastos

Arquivos lidos (texto extraído via pypdf; os slides contêm figuras/diagramas cujo conteúdo gráfico não é extraível como texto, mas as legendas e o corpo textual estão completos):
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula05.pdf` (65 p.)
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula06.pdf` (19 p.)
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula07.pdf` (22 p.)
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula08.pdf` (10 p.)
- `/home/sergi/repositorios/APS/APS/Aulas/sem-senha/APSECAula09.pdf` (13 p.)

---

## Aula 05 — Diagrama de Classes / Dicionário de Informações (65 slides)

### Tópicos cobertos
1. Conceito de classe, objeto, atributo, método.
2. Estrutura do Diagrama de Classes (identidade, atributos, métodos).
3. Associações, multiplicidade/cardinalidade, agregação, composição, herança, classe de associação.
4. Estereótipos UML (`<<entity>>`, `<<boundary>>`, `<<control>>`).
5. Visibilidade de atributos e métodos (`+`, `-`, `#`).
6. Heurísticas para identificação de classes (perguntas de investigação).
7. Organização da informação na empresa → motivação para o Dicionário de Informações.
8. Dicionário de Informações: formato, iterações, seleções, domínios contínuo/discreto, quadro-padrão.
9. Estudo de caso "Controle de Leitos de UTI" completo (objetivos, atores, RFs, UCs, especificação de UC, diagrama de classes, dicionário).

### Conceitos-chave
- **Classe**: descrição de um tipo de objeto — propriedades (atributos) + comportamento (operações). Agrupamento de objetos com mesma estrutura de dados e comportamento (Rumbaugh). Instanciar = criar novo objeto.
- **Identificação de classes**: identificar **substantivos**; verificar palavras importantes no contexto; **não incluir classes desnecessárias** — redundantes, vagas, irrelevantes, atributos/operações/métodos disfarçados de classe, papéis, construções de implementação.
- **Objetos**: dois objetos são distintos mesmo com atributos idênticos (identidade ≠ estado).
- **Atributos**: descrevem características dos objetos; o conjunto de valores em dado momento define o **estado**; somente atributos de interesse do sistema entram na classe.
- **REGRA EXPLÍCITA E COBRADA**: *"não confundir Classes com Entidades de Dados ou Tabelas, onde se identificam atributos de chave primária (PK) ou chave estrangeira (FK). Estes tipos de atributos NÃO são utilizados na Orientação a Objetos."* → **PK/FK são proibidos no Diagrama de Classes.**
- **Método**: representa uma "habilidade"; definir um método na classe implica que todos os objetos daquela classe realizam aquela função.
- **Associações**: relacionamento semântico entre duas classes; análogas aos relacionamentos do modelo relacional; incluem cardinalidade; **possuem identificação (nome) que explica a natureza do relacionamento**. Referência bibliográfica citada: HEUSER, Carlos A. *Projeto de Banco de Dados*.
- **Agregação**: caso especial de associação; hierarquia "parte-todo" / "uma-parte-de"; o agregado contém outros objetos como atributos seus. *"Não faz sentido agregar objetos que não compõem ou montam o outro. Ex.: um endereço não compõe uma pessoa."*
- **Composição**: forma especial de associação; um objeto só pode ser parte de **uma** composição em determinado momento. Exemplos dados: Microcomputador (Monitor, Gabinete, Mouse, Teclado); Carro–Motor–Peça com `0..*`.
- **Herança**: compartilhamento de atributos e operações em relação hierárquica (Rumbaugh); incorporação de definição de outro objeto (Yourdon). Subclasse herda tudo da superclasse e adiciona o específico; **propriedades da superclasse não são repetidas na subclasse**. Relação **"é-um" / is-a**. Herança simples (uma classe base) vs. múltipla (mais de uma). Objetivo: **reuso de código**.
- **Classe de associação**: elemento com propriedades de associação e de classe; **surge apenas a partir de associação com multiplicidade "muitos para muitos" E quando houver atributos a considerar**.

### Notações e convenções exigidas
- **Nomenclatura de herança aceita pelo professor** (usa as quatro formas): Super-Classe/Sub-Classe; Classe Pai/Classe Filho; Classe Base/Classe Derivada; **Generalização/Especialização**.
- **Multiplicidade — notação exata ensinada**:
  - `0..1` (zero a um)
  - `1..1` (um e somente um)
  - `0..*` ou `*` (zero a muitos / muitos)
  - `1..*` (um a muitos)
  - `2` (somente dois)
  - `2..5` (dois a cinco)
  - `1,4,6,12` (um, quatro, seis ou doze)
- **Estereótipos obrigatórios no Diagrama de Classes** (modelo de 3 camadas):
  - `<<entity>>` (entidade): papel principal é **armazenar dados**; contém dados recebidos ou gerados pelo sistema.
  - `<<boundary>>` (fronteira): realiza a **interface com os atores**; contém o protocolo de comunicação com monitores, impressoras, placas de rede etc.
  - `<<control>>` (controle): **executa processos**; apresenta o fluxo de execução dos casos de uso; comanda outras classes; interpreta eventos dos objetos *boundary* repassando-os aos objetos *entity*.
- **Visibilidade** (mesma convenção para atributos e métodos): `+` pública, `-` privada, `#` protegida (acessível a métodos/funções membro de classes derivadas).
- **Nomenclatura de atributos** observada em todos os exemplos: **camelCase sem acentos** — `nome`, `enderecoResidencial`, `telefoneResidencial`, `celular`, `dataNascimento`, `diagnostico`, `dataSolicitacao`, `dataRegistroReserva`, `equipamNecessarios`, `urgencia`, `aceitacao`, `motivoNaoAceitacao`, `codigoPS`, `CRM`. Métodos com parênteses: `registrarPessoa()`.
- **Nomes de classes**: substantivo, iniciando em maiúscula, sem espaço (`SolicitacaoLeito`, `EquipeHigienizacao`, `ProfissionalSaude`, `MaterialConsumido`).

### Regra de construção do Modelo de Classes
- É um **diagrama (documento) ÚNICO**, que permite visualizar todos os objetos de entidade da análise e os relacionamentos entre eles.
- Criado **consolidando os objetos e todas as descrições de casos de uso em um único diagrama**, identificando estruturas hierárquicas, herança, agregação e associações.
- Pode surgir do levantamento de requisitos ou como consequência das fontes de informação.
- **Recomendações de identificação**: identificar substantivos em documentos do negócio; buscar classes já identificadas em projetos existentes.
- **Perguntas de investigação para achar classes** (checklist do professor):
  1. Existem informações que devem ser registradas ou transformadas? (se há informação armazenada/transformada/analisada/manipulada → existe pelo menos uma classe responsável por esses atributos)
  2. Existem sistemas externos? (devem ser vistos como classes com as quais o sistema interage)
  3. Existem representações organizacionais? (áreas de negócio, departamentos, filiais → classes, especialmente em modelos de negócio)
  4. Quais papéis os atores representam no ambiente de negócio? (usuário, operador do sistema, cliente → classes)
  5. Existem dispositivos manipulados pelo sistema? (dispositivos técnicos conectados são candidatos a classes)
  6. Existem padrões, bibliotecas, componentes ou outros itens? (de projetos anteriores, parceiros ou fornecedores → contêm candidatos a classes)

### Receita para montar o Diagrama de Classes a partir dos Casos de Uso (slides 59–60)
**Controle e Interface:**
- Definir **uma classe de interface (`<<boundary>>`) para CADA ator**.
- Definir **uma classe (`<<control>>`) para CADA caso de uso e para cada caso de uso derivado**.

**Entidades:**
- Definir quais dados participam dos casos de uso.
- Agrupar os dados que tenham afinidade.
- Identificar os possíveis relacionamentos.
- Identificar as possíveis generalizações e especializações.
- Identificar os métodos que podem atuar sobre os dados de cada classe, **a partir das ações apresentadas nos casos de uso**.

### Dicionário de Informações — especificação formal exigida
**Definição**: listagem organizada de todos os elementos pertinentes a um sistema, com definições **precisas e rigorosas**, para que usuário e desenvolvedor conheçam todas as classes. Representa **metadados** sobre os atributos.

**Regras:**
- **Todas as classes são descritas, com exceção das abstratas.**
- *"Não existe um padrão de representação formal"* — mas o professor fornece e usa o quadro abaixo.

**Quadro-padrão (colunas, nesta ordem):**

| Atributo | Descrição | Tamanho | Tipo | Domínio | Formato |
|---|---|---|---|---|---|
| Nome do atributo | Descrição do atributo | tamanho | Alfabético / Alfanumérico / Numérico / Data | Contínuo **ou** Discreto (colocar os valores) | Colocar o formato caso exista. Ex.: `99/99/9999`, `XXX` |

> Observação: no slide-resumo (p.47) a ordem é Atributo–Descrição–Tamanho–Tipo–Domínio–Formato; nos quadros do estudo de caso (p.61–64) a ordem usada é **Atributo–Descrição–Tamanho–Tipo–Formato–Domínio**. A segunda é a efetivamente aplicada nos exemplos.

**Cabeçalho de cada quadro**: `NomeDaClasse: descrição da classe` (ex.: `Pessoa: Identifica as características básicas de qualquer pessoa`).

**Símbolos de FORMATO:**
- Alfanumérico: `X`
- Alfabético: `A`
- Numérico: `9`
- Hora: `99:99:99` ; `hh:mm:ss`
- Data: `99/99/9999` ; `dd/mm/aaaa`

**Definição de ITERAÇÕES (repetição de símbolos):**
- `a = 1{b}` → só limita o mínimo
- `a = {b}10` → só limita o máximo
- `a = 1{b}10` → limita mínimo e máximo
- `a = {b}` → sem limite definido
- Uso real nos exemplos: `{X}60`, `{X}100`, `{9}10`, `{9}8`, `1{X}60`

**Representação de SELEÇÕES**: colchetes com opções separadas por barra —
`sexo = [Masculino | Feminino]` ; `tipoCliente = [Governo | Indústria | Universidade | Outro]`

**DOMÍNIOS de valores:**
- **Contínuo**: existe um intervalo onde os valores são válidos (ex.: R$ 0,00 a R$ 999.999,99; 0ºC a 300ºC). Todos os dados relacionados a um significado.
- **Discreto**: só aceita certos valores determinados; **é necessário colocar o valor E o significado** (ex.: Nº Departamento 36=Vendas, 08=Contas, 29=Depósito, 71=Divulgação; Estado Civil C=Casado, S=Solteiro, D=Divorciado, V=Viúvo). Geralmente são códigos com significado.

**Regra de herança no Dicionário (crítica para o trabalho):** classes especializadas **DEVEM repetir/incluir a descrição dos atributos de todas as classes acima na hierarquia**. Ex.: `Paciente` inclui `nome`, `sexo`, `dataNascimento` (de `Pessoa`) + `diagnostico`; `Intensivista` deve incluir os atributos de `Pessoa`, `Profissional de Saúde` e `Médico`. (Atenção: isto é o **oposto** da regra do Diagrama de Classes, onde a subclasse não repete os atributos herdados.)

### Estudo de caso "Controle de Leitos de UTI" (padrão de entrega implícito)
Sequência-modelo apresentada — é o esqueleto que o professor espera ver reproduzido:
1. **Declaração dos Objetivos do Sistema** — Objetivo geral ("Controlar a utilização dos leitos de uma UTI") + Objetivos específicos (controlar reservas; controlar liberação; controlar limpeza; emitir relatórios).
2. **Atores identificados** — Médico Assistente, Médico Intensivista, Equipe de Higienização, Profissional de Saúde.
3. **Requisitos Funcionais identificados** — codificados **FR01…FR07**, redigidos no padrão *"«Ator» pode «verbo» «objeto»"* (ex.: "Médico Assistente pode solicitar reserva de leito - FR01").
4. **Casos de Uso identificados** — codificados **UC01…UC07**, redigidos no infinitivo, sem o ator no nome (ex.: "Solicitar reserva de leito - UC01"). Mapeamento explícito RF → UC.
5. **Diagrama de Casos de Uso**.
6. **Especificação textual de cada caso de uso** (quadro — ver formato abaixo).
7. **Diagrama de Classes** (boundary/control/entity).
8. **Dicionário de Informações** (um quadro por classe).

### Formato do quadro de ESPECIFICAÇÃO DE CASO DE USO (obrigatório)
Campos, nesta ordem:
- **Nome**: `UC001: Solicitar reserva de leito` (nota: no texto corrido aparece `UC01`, no quadro `UC001` — o professor usa ambos; o quadro formal usa 3 dígitos)
- **Atores**: distinguir **Ator Principal** e **Ator de Suporte**
- **Descrição**: "Caso de uso executado quando …"
- **Pré-condições** (lista com traços)
- **Pós-condições**
- **Fluxo Básico** — tabela de **duas colunas: "Ações dos atores" | "Ações do sistema"**, com passos **numerados sequencialmente 1, 2, 3…** atravessando as duas colunas (o número é único e global no fluxo, alternando de lado conforme quem age). Os passos do sistema referenciam as regras de negócio e apontam o desvio (ex.: "5 - Verifica as informações conforme as Regras de Negócio RN001 e RN002. Caso haja uma regra de negócios não atendida vai para 5.a.")
- **Regras de Negócio** — codificadas entre colchetes: `[RN001] O campo "Nome do paciente" é obrigatório.` / `[RN002] A "data de nascimento" deve ter o formato dd/mm/aaaa.`
- **Fluxo Alternativo 1**, **Fluxo Alternativo 2**, … — mesma tabela de duas colunas, numeração reiniciada em 1
- **Fluxo de Exceção** — mesma tabela; passos identificados com a **numeração do passo básico + letra** (`5.a.`), apresentando a mensagem de erro literal e indicando o retorno ("volta para o Fluxo Básico 3")
- Anotação didática do professor no passo 2: os campos exibidos na tela (**nome do paciente, data de nascimento, sexo, diagnóstico, equipamentos necessários, urgência**) *"são atributos"* — ou seja, a especificação do UC deve alimentar diretamente os atributos do Diagrama de Classes e o Dicionário de Informações.

### Trabalhos / avaliação / PlanodoProjeto
Não há menção explícita a trabalhos, entregas, prazos, critérios de avaliação ou ao documento "PlanodoProjeto" nesta aula. O impacto é indireto e forte: os artefatos e formatos aqui definidos são exatamente os itens que compõem os documentos de entrega.

---

## Aula 06 — Diagrama de Objetos (Instâncias) (19 slides)

### Tópicos cobertos
1. Conceito e finalidade do Diagrama de Objetos/Instâncias.
2. Notação de objetos e de vínculos (links).
3. Estudo de caso Controle de Leitos de UTI: Diagrama de Casos de Uso, Diagrama de Classes atualizado, Dicionário de Informações (4 quadros), Diagrama de Objetos.
4. Tutorial da ferramenta CASE (criação de objetos e links).

### Conceitos-chave
- **Finalidade**: melhor entendimento do modelo de classes **e sua validação**.
- **Não é um diagrama independente** — é **complemento do Diagrama de Classes**.
- Fornece visão dos **valores dos atributos armazenados pelos objetos em um determinado instante de tempo**.
- **Simula as possibilidades de preenchimento de um objeto a partir das definições do Dicionário de Informações** — ou seja, é o teste de consistência entre Dicionário e Modelo de Classes.
- **Os objetos NÃO apresentam métodos, apenas atributos** (com valores atribuídos).
- **Vínculos (links)** entre objetos são **instâncias das associações** do Diagrama de Classes.
- **Uma ligação entre objetos NÃO possui cardinalidade (multiplicidade).**
- **Em uma especialização, apenas aparece o objeto especializado; a generalização não é apresentada** (não se instancia a superclasse separadamente).

### Notações e convenções exigidas
- **Identidade do objeto — três formas aceitas** (slide 3 lista "uma das três formas"; o texto extraído mostra duas explicitamente):
  - `: Paciente` (objeto anônimo — só a classe, precedida de dois-pontos)
  - `Paciente 1` / `nomeObjeto : Classe` (objeto nomeado)
  - Em ambos os casos com o estereótipo `<<entity>>` acima do nome.
- **Atributos no objeto**: `atributo = valor`, um por linha, sem tipo. Ex.:
  ```
  <<entity>>
  : Paciente
  nome = Ronaldo
  sexo = M
  dataNascimento = 11/04/1978
  diagnostico = fratura
  ```
- Valores devem **respeitar o formato/domínio declarado no Dicionário** (`sexo = M` conforme domínio discreto F/M; `dataNascimento` em dd/mm/aaaa).

### Ferramenta CASE (procedimento exigido)
Instruções de uso (ferramenta compatível com Astah/UML):
1. **Criar um novo *Class Diagram*, mas nomeá-lo "Diagrama de Objetos"** (não existe tipo próprio).
2. Clicar no ícone **InstanceSpecification** e criar o objeto.
3. Preencher a aba **Base Class** (vincula a instância à classe do modelo).
4. Para associações: clicar no botão **Link**, depois em uma classe e depois na outra.

### Dicionário de Informações — refinamento e autocrítica do professor
- Quadros apresentados para `Pessoa`, `Paciente`, `Intensivista`, `SolicitaçãoLeito`.
- O professor **aponta explicitamente um erro na versão anterior**: *"Há um erro nesta definição do Dicionário de Informações: faltam os novos atributos `codigoPS` e `CRM`"* — e na página seguinte apresenta o quadro corrigido de `Intensivista` incluindo:
  - `codigoPS` — "Código do Profissional de Saúde no hospital" — 10 — Numérico — `{9}10` — Contínuo
  - `CRM` — "Número do registro do médico do Conselho Regional de Medicina" — 8 — Numérico — `{9}8` — Contínuo
- **Lição de consistência cobrada**: qualquer alteração no Diagrama de Classes (aqui: "Adicionado `codigoPS` e `CRM`") **deve ser propagada ao Dicionário de Informações**, inclusive nas subclasses (que repetem os herdados).
- Note-se a evolução de `equipamNecessarios` e `motivoNaoAceitacao` mantendo 60 posições, e `urgencia` como discreto (Alto/Médio/Baixo), `aceitacao` como `[Sim | Não]`.

### Trabalhos / avaliação / PlanodoProjeto
Sem menção explícita.

---

## Aula 07 — Diagrama de Sequência (22 slides)

### Tópicos cobertos
1. Conceito, objetivo e elementos do Diagrama de Sequência.
2. Tipos de mensagens.
3. Estudo de caso Controle de Leitos de UTI recapitulado integralmente (objetivos, atores, RF, UC, especificação de UC, diagrama de classes).
4. Derivação do Diagrama de Sequência a partir do Fluxo Básico do caso de uso.

### Conceitos-chave
- **Definição**: diagrama composto de objetos que apresenta a interação existente em um conjunto de objetos, **enfatizando a ordenação temporal das mensagens**.
- **Objetivo principal**: mostrar as comunicações existentes entre objetos para a execução dos processos do sistema.
- **REGRA DE COBERTURA**: *"Normalmente, para CADA caso de uso é feito um diagrama de sequência, que representa o cenário do caso de uso."* Para descrever um caso de uso pode ser necessário analisar **vários cenários**; **cada cenário pode ser descrito por um diagrama de sequência**.
- **Layout obrigatório**: objetos das classes que participam da interação ficam **acima, no eixo X**; **o objeto que inicia a interação fica mais à ESQUERDA** e os demais vão sendo inseridos à direita. As **mensagens (chamadas de métodos) são dispostas ao longo do eixo vertical (Y)**, conforme os vínculos entre os objetos, **em ordem crescente no tempo**.
- **Elementos nomeados pelo professor** (legendas dos diagramas: "objetos / ativação / linha do tempo"):
  - **Objeto**: representação da instância de uma classe.
  - **Linha do tempo**: linhas verticais **tracejadas ou fechadas**, traçadas da base dos objetos até a parte inferior do diagrama.
  - **Ativação**: período em que o objeto está em execução (**linhas fechadas**), realizando processamento.
- **Mensagens**:
  - **Síncronas**: normalmente chamadas de funções (métodos).
  - **Assíncronas**: o objeto de origem envia a mensagem e prossegue seu processamento, independentemente do tratamento no objeto receptor.
  - **Auto-chamada ou auto-delegação**: objeto envia mensagem para si mesmo.

### Convenção-chave: derivação Fluxo Básico → Diagrama de Sequência (slide 21)
O professor coloca o Fluxo Básico ao lado do diagrama e mostra a correspondência passo a passo. Regra explicitada:
- O passo **"1 - Médico Assistente seleciona a opção de solicitação da reserva de leito"** aparece **entre parênteses e marcado como "Retirado após análise da sequência de operações"** — ou seja, **passos do fluxo que não geram troca de mensagem entre objetos são eliminados na tradução**; os passos 2 a 8 é que viram mensagens.
- Título do diagrama no padrão: **"Diag. de Seq. relativo ao UC 'Solicitar reserva de leito'"** — o diagrama é nomeado explicitamente pelo caso de uso que representa.

### Reforço do padrão de especificação (repetido da Aula 05)
Aula 07 repete integralmente: Declaração de Objetivos (geral + específicos), lista de Atores, RF01–RF07 (grafados "FR01"…"FR07"), UC01–UC07, o quadro de especificação de UC completo (Nome/Atores/Descrição/Pré-condições/Pós-condições/Fluxo Básico/Regras de Negócio/Fluxos Alternativos 1 e 2/Fluxo de Exceção) e o Diagrama de Classes. A repetição em três aulas seguidas indica que **este é o formato exigido nas entregas**.

### Trabalhos / avaliação / PlanodoProjeto
Sem menção explícita.

---

## Aula 08 — Diagrama de Comunicação (Colaboração) (10 slides)

### Tópicos cobertos
Definição, relação com o Diagrama de Sequência, e os quatro elementos: objetos, atores, vínculos (links), mensagens, além da auto-chamada. Comparação lado a lado dos dois diagramas.

### Conceitos-chave
- **Diagramas de comunicação são DERIVADOS dos diagramas de sequência. Eram anteriormente chamados de diagramas de colaboração.**
- **Diferença essencial**: o diagrama de sequência enfatiza a **ordenação temporal das mensagens**; o diagrama de comunicação dá ênfase à **organização dos objetos que participam de uma interação e à sua comunicação**.
- **Objetos**: instâncias de classes; **diferentemente do diagrama de sequência, NÃO possuem linha do tempo nem período de ativação**.
- **Atores**: os mesmos representados no diagrama de sequência e no diagrama de casos de uso — entidades externas ao sistema.
- **Vínculos (Links)**: o objetivo da construção do diagrama de comunicação é **agrupar as mensagens entre pares de objetos, de maneira a levantar as necessidades de comunicação**. Representado por **uma linha unindo os dois objetos**.
- **Mensagens**: são as mesmas do diagrama de sequência; **a ordem em que são chamadas não é importante na disposição gráfica — a indicação temporal é expressa APENAS na NUMERAÇÃO das mensagens**.
- **Regra estrutural**: *"Há apenas um ÚNICO vínculo entre os objetos, sendo que cada vínculo suporta TODAS as mensagens encaminhadas entre os objetos."* (não se desenha uma linha por mensagem).
- **Auto-chamada/auto-delegação**: como no diagrama de sequência, um objeto pode enviar mensagens para si mesmo.

### Notações e convenções
- Numeração sequencial das mensagens sobre os links (é o único portador da ordem temporal).
- Um único link por par de objetos.
- Sem lifeline, sem barra de ativação.
- Prática implícita: o diagrama de comunicação é construído **a partir do** diagrama de sequência já feito (mesmo conjunto de objetos e mensagens, reorganizados).

### Trabalhos / avaliação / PlanodoProjeto
Sem menção explícita.

---

## Aula 09 — Mapeamento do Diagrama de Classes para o DER (13 slides)

### Tópicos cobertos
Regras de mapeamento OO → relacional para: associação, herança (duas possibilidades), agregação e classe de associação; DER completo mapeado do estudo de caso.

### Conceitos-chave
- **Objetivo do mapeamento**: *"a sincronização entre o sistema e o banco de dados (persistência)"*.
- **Regra transversal, repetida em todos os casos**: *"Necessidade de criação de chave primária. A chave primária é um atributo cujo valor distingue cada instância em uma tabela."* E: **"Os métodos NÃO são utilizados"** (o DER só carrega atributos).

### Regras de mapeamento (por construção UML)
1. **Associação** → relacionamento no DER; criar PK em cada tabela e propagar a **FK** para o lado apropriado.
   - Exemplo: `Internamento(codigoInternamento PK, dataAlta, dataInternamento)` e `Atendimento(codigoAtendimento PK, descricaoAtendimento, dataAtendimento, codigoInternamento FK)`.
2. **Herança — primeira possibilidade**: **criar uma tabela para CADA classe que compõe a hierarquia**.
   - Exemplo: `Pessoa(nome, sexo, dataNascimento)` e `Paciente(codigoPaciente PK, diagnostico, nome FK)` — a FK da subclasse aponta para a chave da superclasse.
3. **Herança — segunda possibilidade**: **criar uma ÚNICA tabela para toda a hierarquia**.
   - Exemplo: `Medico(CRM, cargoMedico)`.
4. **Agregação** → relacionamento no DER com criação de PK e FK no lado "parte".
   - Exemplo: `EquipeHigienizacao(numEquipe PK)` e `Funcionario(codFuncionario PK, numEquipe FK)`.
5. **Classe de Associação** → tabela associativa com **as FKs das duas tabelas relacionadas mais os atributos próprios**.
   - Exemplo: `MaterialConsumido(valorMaterialConsumido, quantidadeMaterial, codigoAtendimento FK, codigoMaterial FK)`, ligando `Atendimento` e `Material`.

### Notações e convenções exigidas no DER
- Tipos de dados **em maiúsculas, notação de banco**: `INTEGER`, `CHAR(80)`, `VARCHAR(60)`, `VARCHAR(20)`, `VARCHAR(80)`, `VARCHAR(100)`, `DATE`.
- Formato de linha: `nomeAtributo: TIPO(tamanho)` e, quando for chave estrangeira, sufixo **`(FK)`**.
- A **chave primária aparece como primeiro atributo da tabela** (sem marcação `(PK)` explícita nos slides — a posição a identifica; apenas FKs são marcadas).
- Nomenclatura de colunas mantém o **camelCase das classes** (`codigoAtendimento`, `dataUltimaHigienizacao`, `motivoNaoAceitacao`), diferentemente do padrão snake_case comum em SQL.
- **Contraste obrigatório com a Aula 05**: PK/FK **não existem no Diagrama de Classes** e **só aparecem no DER**. Este é um par de regras que o professor sublinha nos dois extremos.

### DER final consolidado do estudo de caso (slide 12) — 11 tabelas
`Atendimento`, `Internamento`, `Pessoa`, `Paciente`, `EquipeHigienizacao`, `Funcionario`, `Material`, `MaterialConsumido`, `ProfissionalSaude`, `Leito`, `Medico`, `SolicitacaoLeito` — com as FKs: `Atendimento.codigoInternamento`, `Atendimento.codigoPS`; `Internamento.codSolicitacaoLeito`, `Internamento.numeroLeito`; `Paciente.nome`; `Funcionario.numEquipe`, `Funcionario.nome`; `MaterialConsumido.codigoAtendimento`, `MaterialConsumido.codigoMaterial`; `ProfissionalSaude.nome`; `Leito.numEquipe`; `Medico.codigoPS`; `SolicitacaoLeito.codigoPaciente`, `SolicitacaoLeito.CRM`.

Observe que a hierarquia `Pessoa → ProfissionalSaude → Medico` e `Pessoa → Paciente` foi mapeada pela **primeira possibilidade** (uma tabela por classe, encadeando FKs: `Medico.codigoPS → ProfissionalSaude.codigoPS → ProfissionalSaude.nome → Pessoa.nome`).

### Trabalhos / avaliação / PlanodoProjeto
Sem menção explícita.

---

## Síntese — impacto direto na confecção dos trabalhos

### 1. Nenhuma dessas cinco aulas cita explicitamente o documento "PlanodoProjeto", prazos, notas ou critérios de avaliação
As Aulas 05–09 são puramente técnicas/metodológicas. O que elas fornecem é o **conteúdo dos artefatos** e, sobretudo, os **formatos exatos** que o professor usa e, por consequência, espera receber. Regras de entrega devem ser buscadas nas Aulas 01–04 e 10–14 (fora do escopo desta tarefa).

### 2. A "espinha dorsal" do documento de projeto está definida
O estudo de caso "Controle de Leitos de UTI" é repetido inteiro nas Aulas 05, 06 e 07, sempre na mesma ordem. Essa repetição é o **template de facto do trabalho**:

1. Declaração dos Objetivos do Sistema (objetivo geral + objetivos específicos em lista)
2. Atores identificados
3. Requisitos Funcionais identificados — codificados **FR01…FRnn**, texto "«Ator» pode «ação»"
4. Casos de Uso identificados — codificados **UC01…UCnn**, verbo no infinitivo, com o mapeamento RF→UC declarado em texto
5. Diagrama de Casos de Uso
6. Especificação textual de cada UC (quadro completo)
7. Diagrama de Classes (com estereótipos)
8. Dicionário de Informações (um quadro por classe)
9. Diagrama de Objetos (validação do modelo)
10. Diagrama(s) de Sequência (um por UC/cenário)
11. Diagrama(s) de Comunicação
12. DER mapeado

Os itens 1–8 são tipicamente material de **Plano do Projeto (1º bimestre)**; os itens 9–12 (modelagem dinâmica + persistência) são material do **Projeto (2º bimestre)**. As Aulas 05 e parte da 07 sustentam o 1º bim; Aulas 06, 07, 08 e 09 sustentam o 2º bim.

### 3. Rastreabilidade é o critério implícito mais forte
O professor demonstra, e cobra pela demonstração, uma cadeia de derivação sem lacunas:

```
Objetivos → Atores → FRnn → UCnn → Especificação do UC (Fluxo Básico + RNnnn)
   → atributos (citados no passo 2 do fluxo: "são atributos")
   → Diagrama de Classes (boundary por ator, control por UC, entities pelos dados)
   → Dicionário de Informações (todas as classes, menos abstratas)
   → Diagrama de Objetos (valida o Dicionário instanciando valores)
   → Diagrama de Sequência (um por UC; passos do Fluxo Básico viram mensagens)
   → Diagrama de Comunicação (derivado do de sequência)
   → DER (persistência)
```
Cada código (FR01, UC01, UC001, RN001) deve aparecer coerentemente em todos os artefatos. A autocrítica da Aula 06 ("faltam `codigoPS` e `CRM` no Dicionário") é a evidência de que **inconsistência entre artefatos é falha explicitamente sinalizada**.

### 4. Regras de ouro que produzem perda de ponto se violadas
- **Não colocar PK/FK no Diagrama de Classes** (Aula 05, dito com destaque "Importante"). **Colocar PK/FK apenas no DER** (Aula 09).
- **Todo atributo do Diagrama de Classes deve estar no Dicionário de Informações**, exceto classes abstratas (que não são descritas).
- **Subclasse NÃO repete atributos no Diagrama de Classes, mas REPETE no Dicionário de Informações** (regra invertida entre os dois artefatos — armadilha clássica).
- **Uma classe `<<boundary>>` por ator e uma classe `<<control>>` por caso de uso** — a contagem de classes de controle/fronteira é conferível contra a lista de UCs e atores.
- **Classe de associação só quando a multiplicidade for N:N E houver atributos próprios.**
- **Diagrama de Objetos**: sem métodos, sem cardinalidade nos links, sem instanciar a generalização.
- **Diagrama de Comunicação**: um único link por par de objetos, ordem só na numeração, sem lifeline/ativação.
- **Diagrama de Sequência**: objeto iniciador mais à esquerda, tempo crescente para baixo, um diagrama por UC/cenário, nomeado pelo UC.

### 5. Checklist de formatação a aplicar nos trabalhos
- **Códigos**: `FR01`, `UC01` (no texto) / `UC001` (no quadro de especificação) / `RN001` entre colchetes.
- **Nomes**: classes em PascalCase sem espaço/acento; atributos em camelCase sem acento; métodos com `()`.
- **Multiplicidades** na notação `0..1`, `1..1`, `0..*`, `1..*`, `2..5`, `1,4,6,12`.
- **Visibilidade** `+ - #`.
- **Dicionário**: colunas Atributo | Descrição | Tamanho | Tipo | Formato | Domínio; cabeçalho `Classe: descrição`; formatos com `X`/`A`/`9`, iterações `{X}60`, `1{X}60`, `{9}8`; datas `dd/mm/aaaa`; seleções `[Sim | Não]`; domínio Contínuo ou **Discreto com valores E significados listados**.
- **DER**: tipos SQL em maiúsculas (`INTEGER`, `VARCHAR(n)`, `CHAR(n)`, `DATE`), PK como primeiro atributo, FK marcada com `(FK)`, sem métodos.
- **Quadro de UC**: 8 seções fixas, tabelas de duas colunas "Ações dos atores | Ações do sistema", numeração contínua no Fluxo Básico, fluxos alternativos numerados do 1, exceções como `N.a.` com mensagem literal e retorno indicado.

### 6. Ferramenta
Modelagem em ferramenta CASE UML (padrão Astah, conforme os nomes de comandos: *Class Diagram*, *InstanceSpecification*, aba *Base Class*, botão *Link*). O Diagrama de Objetos deve ser criado como um **Class Diagram renomeado para "Diagrama de Objetos"**.
