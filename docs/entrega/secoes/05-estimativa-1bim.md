## 2.6 Estimativa de Esforço de Software com Planning Poker

Utilizamos o método Planning Poker (GRENNING, 2002) para estimar o esforço de
software com base em cada requisito funcional levantado. A equipe reuniu-se em sessão online com cartas
da sequência de Fibonacci (1, 2, 3, 5, 8, 13, 21), revotando os requisitos em que
houve divergência até o consenso (Figura {{F:planning-poker}}). Com base nos
resultados elaboramos a Tabela {{T:poker}}.

![Figura {{F:planning-poker}}. Sessão de Planning Poker: baralho utilizado e carta de consenso de cada requisito funcional.](figuras/planning-poker.png)

**Tabela {{T:poker}}. Estimativa de esforço, em horas, para cada requisito funcional.**

| Código do Requisito Funcional | Estimativa de esforço (horas) |
|----------|----------|
| [RF001] | 5 |
| [RF002] | 5 |
| [RF003] | 3 |
| [RF004] | 2 |
| [RF005] | 13 |
| [RF006] | 5 |
| [RF007] | 5 |
| [RF008] | 3 |
| [RF009] | 13 |
| [RF010] | 5 |
| [RF011] | 8 |
| [RF012] | 13 |
| [RF013] | 5 |
| [RF014] | 5 |
| [RF015] | 5 |
| [RF016] | 5 |
| [RF017] | 5 |
| [RF018] | 8 |
| [RF019] | 3 |
| [RF020] | 8 |
| [RF021] | 8 |
| **Total** | **132** |

Assim, a estimativa de esforço total para o desenvolvimento e cumprimento de todos
os requisitos funcionais foi de **132 horas** — cerca de 33 horas por integrante ao
longo do bimestre. As maiores estimativas couberam à importação transacional de
cotações [RF005], ao painel gráfico com indicadores [RF009] e ao motor de
recomendações [RF012], por concentrarem validações, cálculo numérico e regras de
negócio, seguidas da sincronização online [RF020] e do backtesting [RF021], que
envolvem integração externa e reprocessamento do histórico.

\newpage
