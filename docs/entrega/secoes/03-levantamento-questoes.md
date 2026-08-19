# 2 LEVANTAMENTO DE REQUISITOS

Na presente seção são apresentadas perguntas organizacionais, feitas para alinhar o
projeto às necessidades do clube; econômicas, para o levantamento de custos e
viabilidade financeira; técnicas, para compreender o escopo metodológico e as
ferramentas para o desenvolvimento do projeto, bem como a capacidade do cliente de
manter o funcionamento do sistema; e perguntas operacionais, visando entender as
demandas e necessidades dos usuários do sistema. Para mais, são apontados os
requisitos funcionais e não funcionais, levantados com base nas respostas às
questões, bem como restrições do projeto e requisitos de experiência do usuário, na
finalidade de assimilar as necessidades e expectativas do cliente. Por fim, foi
levantada a estimativa de esforço de software para cada requisito funcional,
utilizando o método Planning Poker e, como verificação complementar, a técnica de
Pontos de Caso de Uso (UCP).

As respostas a seguir foram dadas pelo coordenador do clube, cliente do projeto. 🟦

## 2.1 Questões Organizacionais

1. **Quais são as principais necessidades do cliente?**

Foi relatada a necessidade de centralizar o acompanhamento dos ativos estudados
pelo clube, hoje disperso em planilhas individuais, e de padronizar a análise com
critérios objetivos: os debates nas reuniões partem de números diferentes e de
regras que cada participante aplica à sua maneira.

2. **Quais alterações deverão ser feitas na rotina do clube?**

As reuniões quinzenais passarão a partir dos dados e das recomendações exibidos
pelo sistema. Um dos membros assumirá o papel de administrador, responsável por
manter o cadastro de ativos e importar as cotações após os pregões.

3. **Qual o tempo necessário para suprir as necessidades apresentadas?**

Será necessário ao menos de três a quatro meses para o desenvolvimento de um
projeto que atenda às necessidades levantadas, de modo que o clube gostaria de
utilizar o sistema ainda neste semestre letivo.

4. **Como os novos métodos apresentados pelo projeto se diferenciam dos métodos atuais utilizados pelo clube?**

Atualmente cada participante mantém a própria planilha, sem padrão de cálculo, e as
conclusões não são comparáveis nem auditáveis. O sistema unificará a base de
cotações e aplicará as mesmas regras de análise para todos, registrando a
justificativa numérica de cada recomendação.

5. **Quais os principais benefícios que o projeto trará para o cliente?**

Manter um histórico central de cotações dos ativos estudados; gerar recomendações
sempre justificadas, que servem de material didático nas reuniões; consolidar as
carteiras com preço médio e resultado calculados automaticamente; e avisar os
participantes por alertas de preço.

6. **Existem riscos ao realizar a mudança para o novo sistema definido pelo projeto?**

Há dependência do formato dos arquivos CSV de cotações obtidos da B3 e das
corretoras — mudanças de leiaute exigiriam ajuste na importação. Há também o risco
de participantes interpretarem as recomendações como aconselhamento financeiro,
motivo pelo qual o sistema deve deixar claro seu caráter de apoio ao estudo.

7. **Como outras organizações similares resolveram necessidades similares?**

Outros clubes utilizam plataformas comerciais de análise (home brokers das
corretoras, TradingView, Status Invest). São soluções pagas ou dependentes de
internet, cujos critérios de análise não são transparentes nem configuráveis — o
que não atende ao propósito didático do clube.

## 2.2 Questões Econômicas

1. **Quanto custará o projeto, aproximadamente?**

O projeto não poderá ser custeado, visto que o clube é um grupo de estudos de uma
instituição pública, sem verba própria. O desenvolvimento será voluntário, pela
equipe do projeto.

2. **É necessário o custeio do projeto? Se sim, de que forma?**

Não. Todas as ferramentas previstas são gratuitas e de código aberto, e o sistema
executará nas máquinas já existentes dos participantes e do laboratório.

3. **Qual será o gasto para manter o sistema?**

Nenhum gasto direto. As cotações históricas são disponibilizadas gratuitamente pelo
site da B3 e pelas corretoras, e o banco de dados é embutido na aplicação, sem
servidores a manter.

4. **Haverá algum impacto significativo na verba do clube?**

Não há verba envolvida; o clube não possui receita nem despesa regulares.

5. **O sistema pode auxiliar com a verba de alguma forma?**

Indiretamente: participantes que hoje assinam plataformas de análise pagas poderão
substituí-las pelo sistema nas atividades do clube, economizando as mensalidades.

6. **Quais são os possíveis competidores do sistema no mercado?**

Plataformas de análise como Status Invest, Fundamentus e TradingView, além dos home
brokers das corretoras. Nenhuma delas, porém, é offline, gratuita e com critérios de
recomendação transparentes e configuráveis.

7. **Existe a possibilidade de oferecer o sistema a outros clientes?**

Existe. O clube pretende disponibilizar o sistema gratuitamente a outros clubes de
investimento e ligas acadêmicas, preservando os créditos da equipe.

## 2.3 Questões Técnicas

1. **Qual linguagem de programação será usada no desenvolvimento do sistema?**

A linguagem de programação utilizada será C++, com o paradigma de orientação a
objetos, pela robustez, pelo desempenho no processamento das séries de cotações e
por ser a linguagem estudada pela equipe na universidade.

2. **Como será construída a interface gráfica do sistema?**

A interface gráfica utilizará o framework Qt 6, nos módulos Widgets (telas) e
Charts (gráficos), produzindo uma aplicação desktop nativa.

3. **O sistema fará uso de banco de dados?**

Sim. Será utilizado o SGBD relacional SQLite, embutido na aplicação (sem servidor),
com no mínimo cinco tabelas.

4. **O sistema precisará de conexão com a internet?**

Não. O sistema funcionará totalmente offline, a partir das cotações importadas de
arquivos CSV — requisito do cliente, pois a sala usada pelo clube não possui rede
confiável.

5. **Qual será a ferramenta de versionamento de código utilizada?**

Durante o desenvolvimento do projeto será usada a plataforma de hospedagem GitHub,
com controle de versões Git.

6. **Como será feita a manutenção do sistema/suporte técnico após seu desenvolvimento?**

Pela própria equipe enquanto seus membros participarem do clube. O código será
documentado e acompanhado de guia de compilação e execução, permitindo que futuros
membros assumam a manutenção.

7. **Como serão obtidos os dados de cotações dos ativos?**

Por arquivos CSV no padrão brasileiro (datas dd/mm/aaaa e vírgula decimal),
exportados do site da B3 ou das corretoras e importados no sistema pelo
administrador.

## 2.4 Questões Operacionais

1. **Quais são os principais usuários desse sistema?**

O coordenador e o membro responsável pela base de dados (papel Administrador) e os
demais participantes do clube (papel Investidor).

2. **Quais funcionalidades são essenciais para o sistema?**

A importação do histórico de cotações, o gráfico de preços com médias móveis, as
recomendações justificadas de compra, venda ou neutralidade, o controle de carteira
e os alertas de preço.

3. **Quais funcionalidades são desejáveis?**

Indicadores adicionais (índice de força relativa, volatilidade), indicadores
fundamentalistas (P/L e Dividend Yield), a configuração dos parâmetros das regras
de análise e o arquivamento de alertas já disparados.

4. **Qual a preferência de interação dos clientes com o sistema (por exemplo: mobile, web, desktop)?**

Aplicação desktop, para uso nas máquinas do laboratório e nos notebooks dos
participantes (Windows e Linux).

5. **O sistema poderia auxiliar na otimização de algum processo já existente?**

Sim, na consolidação das carteiras: hoje o preço médio e o resultado de cada
participante são calculados manualmente em planilha, processo demorado e propenso a
erros.

6. **Existem prazos ou ciclos específicos considerados críticos para o sistema?**

Sim: as reuniões quinzenais do clube, antes das quais as cotações devem estar
importadas e as recomendações atualizadas, e o fechamento mensal do resultado das
carteiras.

7. **Quais recursos de usabilidade são importantes para garantir que todos utilizem bem o sistema?**

Distinguir ganhos e perdas por cor e por sinal numérico, mensagens de erro em
português indicando o motivo (e a linha, no caso da importação), navegação que
mostre a cada papel somente as opções permitidas e a indicação clara de que as
recomendações não constituem aconselhamento financeiro.

\newpage
