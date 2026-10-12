## 2.6 Estimativa de Esforço de Software com Planning Poker

Utilizamos o método Planning Poker (GRENNING, 2002) para estimar o esforço de
software com base em cada requisito funcional levantado. A equipe reuniu-se em sessão online com cartas
da sequência de Fibonacci (1, 2, 3, 5, 8, 13, 21), revotando os requisitos em que
houve divergência até o consenso (Figura {{F:planning-poker}}). Com base nos
resultados elaboramos a Tabela {{T:poker}}, que também registra o tempo
efetivamente realizado na implementação de cada requisito.

![Figura {{F:planning-poker}}. Sessão de Planning Poker: baralho utilizado e carta de consenso de cada requisito funcional.](figuras/planning-poker.png)

**Tabela {{T:poker}}. Estimativa de esforço e tempo efetivamente realizado, em horas, para cada requisito funcional.**

| Código do Requisito Funcional | Estimativa de esforço (horas) | Tempo realizado (horas) |
|----------|----------|----------|
| [RF001] | 5 | 4 |
| [RF002] | 5 | 5 |
| [RF003] | 3 | 3 |
| [RF004] | 2 | 2 |
| [RF005] | 13 | 16 |
| [RF006] | 5 | 5 |
| [RF007] | 5 | 5 |
| [RF008] | 3 | 2 |
| [RF009] | 13 | 17 |
| [RF010] | 5 | 5 |
| [RF011] | 8 | 8 |
| [RF012] | 13 | 15 |
| [RF013] | 5 | 4 |
| [RF014] | 5 | 6 |
| [RF015] | 5 | 6 |
| [RF016] | 5 | 5 |
| [RF017] | 5 | 4 |
| [RF018] | 8 | 9 |
| [RF019] | 3 | 2 |
| [RF020] | 8 | 11 |
| [RF021] | 8 | 9 |
| **Total** | **132** | **143** |

Assim, a estimativa de esforço total para o desenvolvimento e cumprimento de todos
os requisitos funcionais foi de **132 horas** — cerca de 33 horas por integrante ao
longo do bimestre. As maiores estimativas couberam à importação transacional de
cotações [RF005], ao painel gráfico com indicadores [RF009] e ao motor de
recomendações [RF012], por concentrarem validações, cálculo numérico e regras de
negócio, seguidas da sincronização online [RF020] e do backtesting [RF021], que
envolvem integração externa e reprocessamento do histórico.

O tempo efetivamente realizado na implementação totalizou **143 horas**, 11 horas
(8,3%) acima do estimado. Os desvios concentraram-se nos requisitos que exigiram o
primeiro contato da equipe com recursos ainda pouco conhecidos: o painel gráfico
[RF009] (+4 h), pela curva de aprendizado do Qt Charts; a importação
transacional [RF005] (+3 h), pela quantidade de casos de validação; a
sincronização online [RF020] (+3 h), pela verificação do suporte a TLS e do
formato de resposta da fonte de dados; e o motor de recomendações [RF012] (+2 h),
pela calibração das regras. Os cadastros simples e as consultas ficaram iguais ou
abaixo da estimativa, o que indica que o Planning Poker foi bem calibrado para
tarefas conhecidas e tende a subestimar a integração com tecnologias novas —
aprendizado que será aplicado às estimativas do segundo bimestre.

\newpage
