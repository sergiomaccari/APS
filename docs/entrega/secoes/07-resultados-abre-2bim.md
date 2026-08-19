# 4 RESULTADOS

## 4.1 Protótipo

As figuras a seguir apresentam as telas do sistema em execução, com dados reais
importados de arquivos CSV de cotações da B3. Diferentemente de um protótipo de
fachada, as telas mostradas pertencem ao sistema final, implementado em C++ com
Qt 6: os números exibidos (indicadores, recomendações e resultado da carteira) são
calculados pelo próprio sistema a partir das cotações importadas.

![Figura {{F:tela-01-login}}. Tela de login.](figuras/tela-01-login.png)

![Figura {{F:tela-11-janela-principal-admin}}. Janela principal com a navegação do papel Administrador.](figuras/tela-11-janela-principal-admin.png)

![Figura {{F:tela-12-janela-principal-investidor}}. Janela principal com a navegação do papel Investidor.](figuras/tela-12-janela-principal-investidor.png)

![Figura {{F:tela-02-painel-analise}}. Painel de análise, com gráfico de velas, médias móveis e indicadores do período.](figuras/tela-02-painel-analise.png)

![Figura {{F:tela-03-detalhe-ativo}}. Tela de detalhes do ativo, com fundamentos, histórico e reavaliação regra a regra.](figuras/tela-03-detalhe-ativo.png)

![Figura {{F:tela-04-recomendacoes}}. Tela de recomendações vigentes com justificativa.](figuras/tela-04-recomendacoes.png)

![Figura {{F:tela-05-carteira}}. Tela da carteira com resultado consolidado.](figuras/tela-05-carteira.png)

![Figura {{F:tela-06-alertas}}. Tela de alertas de preço.](figuras/tela-06-alertas.png)

![Figura {{F:tela-07-ativos}}. Tela de administração de ativos.](figuras/tela-07-ativos.png)

![Figura {{F:tela-08-importacao}}. Tela de importação de cotações com relatório de validação.](figuras/tela-08-importacao.png)

![Figura {{F:tela-09-regras}}. Tela de configuração das regras de análise.](figuras/tela-09-regras.png)

![Figura {{F:tela-10-usuarios}}. Tela de gerenciamento de usuários.](figuras/tela-10-usuarios.png)

## 4.2 Modelagem

A modelagem do AB3 parte dos requisitos do Capítulo 2: cada requisito funcional é
atendido por um caso de uso (com [RF009] e [RF010] atendidos pelo mesmo UC009), e
cada caso de uso é especificado com seus fluxos, regras de negócio e classes
participantes. Aos artefatos da primeira entrega (casos de uso, classes, dicionário
de informações, objetos e sequência) somam-se, nesta versão, os Diagramas de
Comunicação, o mapeamento para o Diagrama Entidade-Relacionamento, os Diagramas de
Estados e os Diagramas de Atividades.

### Diagrama de Casos de Uso

O sistema possui dois atores. O **Administrador** mantém a base — cadastro de
ativos, importação de cotações, configuração das regras de análise e gerência de
usuários. O **Investidor** consome as análises — painel gráfico, recomendações,
carteira e alertas. Todos os casos de uso exigem autenticação prévia (UC001).

![Figura {{F:uc-casos-de-uso}}. Diagrama de casos de uso.](figuras/uc-casos-de-uso.png)
