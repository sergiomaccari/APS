# 5 CONCLUSÕES

Este Plano do Projeto consolidou o ciclo de análise do sistema AB3 — Analisador de
Ativos da B3. A partir da entrevista com o cliente foram levantados 19 requisitos
funcionais, 7 não funcionais, 4 restrições de projeto e 4 requisitos de experiência
do usuário, todos rastreáveis aos 18 casos de uso especificados no padrão da
disciplina e às classes do Diagrama de Classes — cada informação citada nos fluxos
dos casos de uso existe como atributo no dicionário de informações, e cada caso de
uso possui seu Diagrama de Sequência.

O estudo de viabilidade confirmou a exequibilidade do projeto no contexto do
cliente: custo zero em ferramentas, funcionamento offline e operação pelos próprios
membros do clube. A estimativa de esforço combinou duas técnicas — Planning Poker
(116 horas) e Pontos de Caso de Uso (145,45 UCP) — que convergiram na
identificação dos pontos de maior complexidade: a importação transacional de
cotações, o painel gráfico de análise e o motor de recomendações.

Destaca-se que o protótipo apresentado no Capítulo 4 já foi construído na
tecnologia definitiva (C++ com Qt 6 e SQLite), navegável em todos os casos de uso e
calculando indicadores reais sobre cotações importadas — o que antecipa riscos
técnicos que só apareceriam na implementação e dá segurança às estimativas
apresentadas.

## 5.1 Trabalhos Futuros

Para o segundo bimestre, o projeto prossegue com: a modelagem dinâmica complementar
(Diagramas de Comunicação, mapeamento Entidade-Relacionamento, Diagramas de Estados
e de Atividades); o endurecimento do sistema com a cobertura dos fluxos de exceção
especificados e testes automatizados; e a preparação dos Testes de Validação, com a
demonstração de cada caso de uso mapeada ao seu Diagrama de Sequência. Como
evoluções além do escopo da disciplina, vislumbram-se a atualização automática de
cotações via API quando houver conexão, novas regras de análise (MACD e Bandas de
Bollinger) e o backtesting das regras sobre o histórico importado.

\newpage
