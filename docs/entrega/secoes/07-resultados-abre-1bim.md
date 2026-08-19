# 4 RESULTADOS

## 4.1 Conteúdo dos Resultados

Este capítulo apresenta os resultados do projeto em duas partes. A primeira é o
protótipo das telas do sistema, já construído na tecnologia definitiva (C++ com
Qt 6) e navegável de ponta a ponta, cobrindo todos os casos de uso levantados. A
segunda é a modelagem do sistema: o Diagrama de Casos de Uso, a especificação de
cada caso de uso no padrão da disciplina, o Diagrama de Classes com o dicionário de
informações, os Diagramas de Objetos e um Diagrama de Sequência para cada caso de
uso.

### Protótipo das telas

As figuras a seguir apresentam as telas do protótipo em execução, com dados reais
importados de arquivos CSV de cotações.

![Figura {{F:tela-01-login}}. Protótipo da tela de login.](figuras/tela-01-login.png)

![Figura {{F:tela-11-janela-principal-admin}}. Janela principal com a navegação do papel Administrador.](figuras/tela-11-janela-principal-admin.png)

![Figura {{F:tela-12-janela-principal-investidor}}. Janela principal com a navegação do papel Investidor.](figuras/tela-12-janela-principal-investidor.png)

![Figura {{F:tela-02-painel-analise}}. Protótipo do painel de análise, com gráfico de velas, médias móveis e indicadores do período.](figuras/tela-02-painel-analise.png)

![Figura {{F:tela-03-detalhe-ativo}}. Protótipo da tela de detalhes do ativo, com fundamentos e histórico.](figuras/tela-03-detalhe-ativo.png)

![Figura {{F:tela-04-recomendacoes}}. Protótipo da tela de recomendações vigentes com justificativa.](figuras/tela-04-recomendacoes.png)

![Figura {{F:tela-05-carteira}}. Protótipo da tela da carteira com resultado consolidado.](figuras/tela-05-carteira.png)

![Figura {{F:tela-06-alertas}}. Protótipo da tela de alertas de preço.](figuras/tela-06-alertas.png)

![Figura {{F:tela-07-ativos}}. Protótipo da tela de administração de ativos.](figuras/tela-07-ativos.png)

![Figura {{F:tela-08-importacao}}. Protótipo da tela de importação de cotações com relatório de validação.](figuras/tela-08-importacao.png)

![Figura {{F:tela-09-regras}}. Protótipo da tela de configuração das regras de análise.](figuras/tela-09-regras.png)

![Figura {{F:tela-10-usuarios}}. Protótipo da tela de gerenciamento de usuários.](figuras/tela-10-usuarios.png)

## 4.2 Modelagem

A modelagem do AB3 parte dos requisitos do Capítulo 2: cada requisito funcional é
atendido por um caso de uso (com [RF009] e [RF010] atendidos pelo mesmo UC009), e
cada caso de uso é especificado, na sequência, com seus fluxos, regras de negócio e
classes participantes.

### Diagrama de Casos de Uso

O sistema possui dois atores. O **Administrador** mantém a base — cadastro de
ativos, importação de cotações, configuração das regras de análise e gerência de
usuários. O **Investidor** consome as análises — painel gráfico, recomendações,
carteira e alertas. Todos os casos de uso exigem autenticação prévia (UC001).

![Figura {{F:uc-casos-de-uso}}. Diagrama de casos de uso.](figuras/uc-casos-de-uso.png)
