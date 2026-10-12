# 5 CONCLUSÕES

Este Plano do Projeto consolidou o ciclo de análise do sistema AB3 — Analisador de
Ativos da B3. A partir das questões de levantamento respondidas pela própria
equipe, na condição de cliente, foram levantados 21 requisitos funcionais, 8 não
funcionais, 4 restrições de projeto e 4 requisitos de experiência do usuário, todos
rastreáveis aos 20 casos de uso especificados no padrão da disciplina e às classes
do Diagrama de Classes — cada informação citada nos fluxos dos casos de uso existe
como atributo no dicionário de informações, e cada caso de uso possui seu Diagrama
de Sequência.

O estudo de viabilidade confirmou a exequibilidade do projeto no contexto do
cliente: custo zero em ferramentas, funcionamento offline, operação pelos próprios
integrantes da equipe, que são também os usuários do sistema, e observância da
regulamentação da CVM sobre análise de valores mobiliários e da LGPD. A estimativa
de esforço, feita com o método Planning Poker (132 horas estimadas), identificou os pontos de maior complexidade: a importação
transacional de cotações, o painel gráfico de análise e o motor de recomendações.

Destaca-se que o protótipo apresentado no Capítulo 4 já foi construído na
tecnologia de interface definitiva (C++ com Qt 6), com as mesmas telas da versão
final, navegável em todos os casos de uso e operando sobre dados de demonstração
fixos mantidos em memória — o que antecipa riscos de interface e de navegação que
só apareceriam na implementação e dá segurança às estimativas apresentadas. A
persistência em banco de dados, o motor de análise e a integração com a fonte
online de cotações são implementados no segundo bimestre.

## 5.1 Trabalhos Futuros

Para o segundo bimestre, o projeto prossegue com: a implementação da persistência
em banco de dados, do motor de análise com as regras de recomendação e da
sincronização com a fonte online de cotações; a modelagem dinâmica complementar
(Diagramas de Comunicação, mapeamento Entidade-Relacionamento, Diagramas de Estados
e de Atividades); e a preparação dos Testes de Validação, com a
demonstração de cada caso de uso mapeada ao seu Diagrama de Sequência. Como
evoluções além do escopo da disciplina, vislumbram-se novas regras de análise
(MACD e Bandas de Bollinger), o agendamento automático da sincronização de
cotações e a exportação de relatórios de análise.

\newpage
