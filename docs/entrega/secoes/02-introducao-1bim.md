# 1 INTRODUÇÃO

Os quatro integrantes da equipe deste projeto são estudantes de Engenharia de
Computação da Universidade Tecnológica Federal do Paraná (UTFPR), Campus Curitiba,
que acompanham ativos negociados na B3 (Brasil, Bolsa, Balcão) — ações e fundos de
investimento imobiliário — e estudam análise técnica para apoiar suas decisões como
investidores pessoa física. A equipe é, portanto, ao mesmo tempo desenvolvedora e
cliente do sistema proposto. Hoje esse acompanhamento é feito em planilhas
individuais, sem padronização: cada integrante mantém seus próprios dados de
cotações, calcula os próprios indicadores e chega a conclusões que não podem ser
comparadas com as dos demais.

O presente projeto tem como finalidade proporcionar à equipe um sistema único de
apoio à análise de ativos, o **AB3 — Analisador de Ativos da B3**: uma aplicação de
mesa (desktop) que centraliza o cadastro de ativos, importa o histórico de cotações
a partir de arquivos CSV, apresenta gráficos e indicadores técnicos, consolida a
carteira de cada integrante e gera recomendações de compra, venda ou neutralidade
sempre acompanhadas da justificativa numérica que as produziu. O sistema destina-se
ao estudo — as recomendações são apoio à decisão, e não aconselhamento financeiro.

## 1.1 Objetivo Geral

Projetar um sistema de apoio à análise de ativos da bolsa de valores brasileira
(B3) para os próprios integrantes da equipe, estudantes da Universidade Tecnológica
Federal do Paraná, em sua atuação como investidores pessoa física.

## 1.2 Objetivos Específicos

- Projetar um sistema para cadastro de ativos (ações e fundos imobiliários) e
  importação do histórico de cotações a partir de arquivos CSV.
- Projetar a visualização de gráficos de cotações com indicadores técnicos (médias
  móveis, índice de força relativa e volatilidade).
- Projetar um motor de análise configurável que gere recomendações justificadas de
  compra, venda ou neutralidade.
- Projetar o controle de carteira de investimentos com preço médio e resultado
  consolidado, e alertas de preço notificados ao usuário.

## 1.3 Conteúdo do Plano do Projeto

Este documento está dividido em cinco partes:

1. **Capítulo 2: Levantamento de Requisitos.** Nesta parte é apresentado o
   levantamento de requisitos, realizado com base no desenvolvimento de questões
   respondidas pela própria equipe, na condição de cliente do sistema, evidenciando
   as suas principais demandas. Também é realizada a estimativa de esforço de
   software para cada requisito funcional, com o método Planning Poker.
2. **Capítulo 3: Estudo de Viabilidade.** Este capítulo explicita a viabilidade do
   projeto, examinando as ferramentas e recursos que serão utilizados.
3. **Capítulo 4: Resultados.** Nesta seção está incluído o resultado do sistema,
   aprofundando em detalhes da modelagem do projeto, como o Diagrama de Casos de
   Uso e o Diagrama de Classes da aplicação, além do protótipo das telas.
4. **Capítulo 5: Conclusões.** No capítulo 5 são apresentadas as considerações
   finais do desenvolvimento do projeto.
5. **Capítulo 6: Referências Bibliográficas.** Por fim, a última seção contém as
   referências bibliográficas utilizadas no decorrer do documento.

\newpage
