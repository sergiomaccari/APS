# 3 ESTUDO DE VIABILIDADE

Na presente seção é realizado o estudo preliminar da viabilidade organizacional,
econômica, técnica e operacional do projeto (PRESSMAN; MAXIM, 2016), bem como a
análise dos recursos a serem utilizados, indicando assim sua exequibilidade e as
relações de custo/benefício de sua implementação.

## 3.1 Viabilidade Organizacional

Conforme as respostas obtidas pelo levantamento de dados, a equipe não dispõe hoje
de nenhum instrumento comum de acompanhamento: cada integrante mantém a própria
planilha, com dados, fórmulas e critérios distintos, o que compromete a comparação
das análises e o estudo conjunto. O novo sistema centraliza o cadastro de ativos e
o histórico de cotações em uma única base, aplica as mesmas regras de análise para
todos e registra a justificativa de cada recomendação — transformando as discussões
de opinião em discussões sobre critérios verificáveis. A divisão em dois papéis
(Administrador e Investidor) reflete a organização adotada pela equipe, em que um
integrante é responsável pelos dados e os demais os consomem. Como cliente e
desenvolvedores são as mesmas pessoas, a implantação não depende de aprovação
externa nem exige mudança na estrutura do grupo.

## 3.2 Viabilidade Econômica

Tendo em vista que o cliente é a própria equipe, sem verba destinada ao projeto, o
sistema foi concebido para custo zero de aquisição e operação. Todas as ferramentas
de desenvolvimento são gratuitas e de código aberto (compilador GCC, CMake,
framework Qt na licença LGPL, SQLite em domínio público, Git/GitHub). O sistema
executa localmente nas máquinas já existentes, sem servidores, hospedagem ou
licenças, e os dados de cotações são obtidos gratuitamente no site da B3 e nas
corretoras. O único custo real é o tempo da equipe de desenvolvimento — estimado em
132 horas na seção 2.6 e valorado na seção 3.5 —, assumido voluntariamente como
parte da disciplina. Como benefício econômico indireto, o sistema dispensa a
assinatura de plataformas de análise pagas para as funcionalidades que oferece.

## 3.3 Viabilidade Técnica

A equipe cursou as disciplinas de programação orientada a objetos em C++ da
universidade, e as tecnologias escolhidas são maduras e amplamente documentadas: o
Qt 6 fornece os componentes de interface e de gráficos (Widgets e Charts)
necessários às telas planejadas, e o SQLite oferece um banco relacional completo
embutido na aplicação, adequado ao requisito de funcionamento offline [RNF001]. Os
algoritmos de análise técnica previstos (médias móveis simples e exponencial,
índice de força relativa e volatilidade) têm formulação clássica publicada (WILDER,
1978; MURPHY, 1999) e são implementáveis diretamente sobre as séries de cotações
importadas.

A Tabela {{T:conhecimento}} apresenta o nível de conhecimento de cada integrante nas
tecnologias do projeto, em três graus: básico (conhece os conceitos e já os aplicou
em exercícios), intermediário (já desenvolveu projetos acadêmicos com a tecnologia)
e avançado (utiliza a tecnologia com autonomia em projetos de maior porte).

**Tabela {{T:conhecimento}}. Nível de conhecimento dos integrantes em cada tecnologia.**

| Tecnologia | Sergio | Arthur | Leonardo | Thales |
|--------------|-----------|-----------|-----------|-----------|
| C++ e orientação a objetos | Avançado | Intermediário | Intermediário | Intermediário |
| Qt 6 (Widgets e Charts) | Intermediário | Básico | Básico | Básico |
| SQL e SQLite | Avançado | Intermediário | Intermediário | Intermediário |
| CMake | Intermediário | Básico | Básico | Intermediário |
| Git e GitHub | Avançado | Intermediário | Intermediário | Avançado |
| UML | Intermediário | Intermediário | Intermediário | Intermediário |
| Mercado de capitais e análise técnica | Intermediário | Básico | Básico | Intermediário |

A principal lacuna está no Qt 6, que três dos quatro integrantes conhecem apenas em
nível básico. Ela é mitigada pela documentação oficial do framework, pelos
exemplos já construídos no repositório do projeto e pela revisão de código entre os
integrantes. Nas demais tecnologias, todos têm ao menos a base exigida pelo projeto.

O principal risco técnico identificado é a variação de leiaute dos arquivos CSV de
cotações entre fontes. A mitigação adotada é dupla: fixar um formato de entrada
documentado (cabeçalho com seis colunas, datas dd/mm/aaaa, vírgula decimal) e
validar estruturalmente cada arquivo antes de gravar, rejeitando-o por inteiro em
caso de erro [RNF002], com mensagem indicando linha e motivo [REU003]. O risco de
perda de dados é mitigado pelo caráter transacional da importação e pela proteção
contra duplicidade [RNF003].

## 3.4 Viabilidade Operacional

O sistema será operado pelos próprios integrantes da equipe, que são também os seus
usuários, sem necessidade de equipe técnica dedicada nem de treinamento formal. O
fluxo operacional acompanha o ciclo do mercado: após o fechamento do pregão, o
administrador importa os arquivos de cotações e o sistema reavalia recomendações e
alertas; em seguida, cada integrante consulta o painel gráfico, as recomendações
justificadas e o resultado da sua carteira. A navegação por papel [REU001] impede
que um investidor altere a base por engano, e as mensagens de erro em português
[REU003] indicam o motivo de cada falha. Por ser uma aplicação desktop offline, a
operação não depende da infraestrutura de rede do laboratório.

Quanto à legislação aplicável, dois marcos foram considerados. O primeiro é a
Resolução CVM nº 20/2021, que reserva a analistas de valores mobiliários
credenciados a elaboração, em caráter profissional, de relatórios de análise sobre
valores mobiliários destinados à publicação, divulgação ou distribuição a terceiros
(COMISSÃO DE VALORES MOBILIÁRIOS, 2021). O AB3 não exerce essa atividade: suas
recomendações são produzidas por regras transparentes e configuráveis, para uso dos
próprios integrantes no estudo da análise técnica, sem caráter profissional e sem
divulgação a terceiros, e o sistema informa em suas telas que elas constituem apoio
à decisão, e não aconselhamento financeiro [REU004]. Caso o sistema venha a ser
oferecido a outros estudantes, será distribuído como ferramenta de estudo, mantendo
esse aviso e sem que a equipe elabore ou divulgue análises sobre ativos
específicos.

O segundo é a Lei Geral de Proteção de Dados Pessoais (BRASIL, 2018). O sistema
trata dados pessoais dos seus usuários — nome, e-mail, senha e posições da
carteira —, e o tratamento observa os princípios de finalidade, necessidade e
segurança do art. 6º da lei: são coletados somente os dados indispensáveis ao
funcionamento do sistema; eles permanecem no banco de dados local da máquina do
usuário, sem envio a servidores ou a terceiros (a sincronização online transmite
apenas o código do ativo e o período desejado); as senhas são armazenadas somente
como resumo criptográfico SHA-256 [RNF004]; e o gerenciamento de usuários é
restrito ao administrador [RNF005]. Os titulares desses dados são os próprios
integrantes da equipe, que conhecem e consentem com o tratamento.

## 3.5 Recursos a Serem Utilizados

- **Recursos humanos:** os quatro integrantes da equipe de desenvolvimento, em
  regime de tempo parcial, acumulando os papéis de analistas, projetistas e
  programadores.
- **Hardware:** os computadores pessoais da equipe e as máquinas do laboratório
  (qualquer máquina x86-64 com Windows ou Linux atende).
- **Software de desenvolvimento:** linguagem C++ (padrão C++20), framework Qt 6
  (Widgets, Charts, Sql e Test), SGBD SQLite 3, sistema de build CMake,
  versionamento Git com hospedagem no GitHub e editores Qt Creator / VS Code.
- **Dados:** arquivos CSV de cotações históricas obtidos do site da B3, de
  portais de dados de mercado (como o Investing.com) e das corretoras dos
  integrantes.
- **Bibliografia técnica:** obras de engenharia de software, UML e análise técnica
  listadas no Capítulo 6.

Considerando as 132 horas estimadas por meio do Planning Poker (Tabela
{{T:poker}}), divididas igualmente entre os quatro integrantes, e adotando como
referência o valor de R$ 30,00 por hora — compatível com a remuneração de um
desenvolvedor de software em início de carreira —, o custo de desenvolvimento do
projeto é apresentado na Tabela {{T:custos}}.

**Tabela {{T:custos}}. Horas, valor por hora e custo de desenvolvimento de cada integrante.**

| Integrante | Horas estimadas | Valor por hora (R$) | Custo (R$) |
|------------------------|------------|------------|------------|
| Sergio Roncato Maccari | 33 | 30,00 | 990,00 |
| Arthur Dal Bem Nunes | 33 | 30,00 | 990,00 |
| Leonardo Pereira Shibata | 33 | 30,00 | 990,00 |
| Thales Romagna Fabrowski | 33 | 30,00 | 990,00 |
| **Total** | **132** | — | **3.960,00** |

Com as 143 horas efetivamente realizadas (Tabela {{T:poker}}), o custo de
referência chega a R$ 4.290,00. Como o trabalho é desenvolvido no âmbito da
disciplina, sem compensação financeira, esse valor representa o custo de
oportunidade da equipe, e não um desembolso: somado ao custo zero de ferramentas,
hardware e dados, o projeto não tem custo financeiro para o cliente.

\newpage
