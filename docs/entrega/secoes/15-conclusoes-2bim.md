# 5 CONCLUSÕES

Este documento consolidou o ciclo completo de análise, projeto e construção do
sistema AB3 — Analisador de Ativos da B3. A modelagem cobre as visões estática e
dinâmica do sistema: 18 casos de uso especificados no padrão da disciplina, cada um
com seus Diagramas de Sequência e de Comunicação; Diagramas de Estados para as
classes de fronteira e de controle de cada caso de uso, acrescidos das máquinas de
estado do domínio (Alerta, Recomendacao e Importacao); um Diagrama de Atividades
detalhando o estado principal de cada máquina; e o mapeamento do modelo de classes
para um banco relacional de nove tabelas.

O sistema foi implementado integralmente na tecnologia projetada — C++20 com
orientação a objetos, Qt 6 (Widgets e Charts) e SQLite — e está funcionando: a
importação transacional valida e grava as cotações dos arquivos CSV, o painel exibe
gráficos de velas com médias móveis e indicadores, o motor de análise gera
recomendações justificadas pelas quatro regras implementadas sobre o padrão
Strategy, a carteira consolida posições com preço médio ponderado, os alertas
disparam por meio do padrão Observer, a sincronização online busca somente o
período faltante de cada ativo (com a fonte de dados isolada atrás de uma
interface, numa segunda aplicação do padrão Strategy) e o backtesting mede a
taxa de acerto e o retorno médio de cada regra sobre o histórico, com pesos
configuráveis na média do parecer consolidado. A rastreabilidade planejada foi mantida de
ponta a ponta: cada requisito funcional corresponde a um caso de uso, cada caso de
uso às classes que o realizam, e cada classe às tabelas que a persistem — o mesmo
esquema SQL modelado no Diagrama Entidade-Relacionamento é o executado pelo
sistema. A implementação é acompanhada de 67 casos de teste automatizados (Qt
Test) cobrindo os indicadores, as regras de análise e os serviços.

As divergências encontradas durante a implementação foram reincorporadas à
modelagem — os documentos descrevem o sistema como ele é, não como se imaginou que
seria — e os fluxos de exceção especificados nos casos de uso correspondem às
mensagens reais exibidas pelo sistema.

## 5.1 Trabalhos Futuros

Como evoluções além do escopo da disciplina, vislumbram-se: novas regras de
análise (MACD, Bandas de Bollinger e Estocástico) aproveitando a extensibilidade
do padrão Strategy; o agendamento automático da sincronização de cotações; a
exportação de relatórios das reuniões do clube; e o suporte multiusuário em rede
local, substituindo o SQLite por um SGBD cliente-servidor — mudança isolada pela
camada de repositórios.

\newpage
