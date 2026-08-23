# 3 ESTUDO DE VIABILIDADE

Na presente seção é realizado o estudo preliminar da viabilidade organizacional,
econômica, técnica e operacional do projeto, bem como a análise dos recursos a
serem utilizados, indicando assim sua exequibilidade e as relações de
custo/benefício de sua implementação.

## 3.1 Viabilidade Organizacional

Conforme as respostas obtidas pelo levantamento de dados, o clube não dispõe hoje
de nenhum instrumento comum de acompanhamento: cada participante mantém a própria
planilha, com dados, fórmulas e critérios distintos, o que compromete a comparação
das análises e o caráter didático das reuniões. O novo sistema centraliza o
cadastro de ativos e o histórico de cotações em uma única base, aplica as mesmas
regras de análise para todos e registra a justificativa de cada recomendação —
transformando os debates de opinião em debates sobre critérios verificáveis. A
divisão em dois papéis (Administrador e Investidor) reflete a organização já
existente no clube, em que um membro é responsável pelos dados e os demais os
consomem, de modo que a implantação não exige mudança na estrutura do grupo.

## 3.2 Viabilidade Econômica

Tendo em vista a natureza do cliente — grupo de estudos de instituição pública, sem
verba —, o projeto foi concebido para custo zero. Todas as ferramentas de
desenvolvimento são gratuitas e de código aberto (compilador GCC, CMake, framework
Qt na licença LGPL, SQLite em domínio público, Git/GitHub). O sistema executa
localmente nas máquinas já existentes, sem servidores, hospedagem ou licenças, e os
dados de cotações são obtidos gratuitamente no site da B3 e nas corretoras dos
participantes. O único custo real é o tempo da equipe de desenvolvimento — estimado
em 116 horas na seção 2.6 —, assumido voluntariamente como parte da disciplina.
Como benefício econômico indireto, o sistema substitui, no uso do clube,
plataformas de análise por assinatura.

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

O principal risco técnico identificado é a variação de leiaute dos arquivos CSV de
cotações entre fontes. A mitigação adotada é dupla: fixar um formato de entrada
documentado (cabeçalho com seis colunas, datas dd/mm/aaaa, vírgula decimal) e
validar estruturalmente cada arquivo antes de gravar, rejeitando-o por inteiro em
caso de erro [RNF002], com mensagem indicando linha e motivo [REU003]. O risco de
perda de dados é mitigado pelo caráter transacional da importação e pela proteção
contra duplicidade [RNF003].

## 3.4 Viabilidade Operacional

O sistema será operado pelos próprios membros do clube, sem necessidade de equipe
técnica dedicada. O fluxo operacional acompanha o ciclo das reuniões: antes de cada
encontro, o administrador importa os arquivos de cotações e o sistema reavalia
recomendações e alertas; durante a reunião, os participantes consultam o painel
gráfico, as recomendações justificadas e o resultado das carteiras. A navegação por
papel [REU001] impede que participantes alterem a base por engano, e as mensagens
de erro em português [REU003] tornam a operação acessível a membros de qualquer
curso. Por ser uma aplicação desktop offline, não há dependência da infraestrutura
de rede da sala do clube.

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
  participantes.
- **Bibliografia técnica:** obras de engenharia de software, UML e análise técnica
  listadas no Capítulo 6.

\newpage
