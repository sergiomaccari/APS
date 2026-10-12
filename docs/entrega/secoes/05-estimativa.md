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

### Verificação complementar com Pontos de Caso de Uso (UCP)

Como contraprova da estimativa, aplicamos a técnica de Pontos de Caso de Uso
(KARNER, 1993), nos cinco passos apresentados em aula.

**Passo 1 — Peso dos atores (UAW).** Os dois atores do sistema, Administrador e
Investidor, interagem por interface gráfica e são, portanto, complexos (peso 3):

**Tabela {{T:uaw}}. Cálculo do peso não ajustado dos atores (UAW).**

| Ator | Classificação | Peso |
|---|---|---|
| Administrador | Complexo (interface gráfica) | 3 |
| Investidor | Complexo (interface gráfica) | 3 |
| **UAW** | | **6** |

**Passo 2 — Peso dos casos de uso (UUCW).** Cada caso de uso foi classificado pelo
número de transações de seu fluxo de eventos:

**Tabela {{T:uucw}}. Cálculo do peso não ajustado dos casos de uso (UUCW).**

| Caso de uso | Transações | Classificação | Peso |
|---|---|---|---|
| UC001 Fazer login | 3 | Simples | 5 |
| UC002 Cadastrar ativo | 5 | Médio | 10 |
| UC003 Atualizar ativo | 6 | Médio | 10 |
| UC004 Remover ativo | 3 | Simples | 5 |
| UC005 Importar cotações | 8 | Complexo | 15 |
| UC006 Configurar regra de análise | 5 | Médio | 10 |
| UC007 Gerenciar usuários | 6 | Médio | 10 |
| UC008 Buscar ativos | 2 | Simples | 5 |
| UC009 Visualizar painel de análise | 8 | Complexo | 15 |
| UC010 Visualizar detalhes do ativo | 4 | Médio | 10 |
| UC011 Gerar recomendações | 9 | Complexo | 15 |
| UC012 Visualizar recomendações vigentes | 5 | Médio | 10 |
| UC013 Comprar ativo | 6 | Médio | 10 |
| UC014 Vender ativo | 6 | Médio | 10 |
| UC015 Visualizar resultado da carteira | 4 | Médio | 10 |
| UC016 Cadastrar alerta | 5 | Médio | 10 |
| UC017 Avaliar e disparar alertas | 7 | Complexo | 15 |
| UC018 Arquivar alerta | 3 | Simples | 5 |
| UC019 Sincronizar cotações online | 6 | Médio | 10 |
| UC020 Executar backtesting das regras | 6 | Médio | 10 |
| **UUCW** | | | **200** |

Com isso, os pontos de caso de uso não ajustados são
UUCP = UAW + UUCW = 6 + 200 = **206**.

**Passo 3 — Fatores técnicos (TCF).**

**Tabela {{T:tcf}}. Fatores de complexidade técnica (TCF).**

| Fator | Descrição | Peso | Valor (0–5) | Peso × Valor |
|---|---|---|---|---|
| F1 | Sistema distribuído | 2 | 0 | 0 |
| F2 | Desempenho / tempo de resposta | 1 | 3 | 3 |
| F3 | Eficiência do usuário final | 1 | 3 | 3 |
| F4 | Processamento interno complexo | 1 | 3 | 3 |
| F5 | Reusabilidade do código | 1 | 2 | 2 |
| F6 | Facilidade de instalação | 0,5 | 4 | 2 |
| F7 | Facilidade de uso | 0,5 | 4 | 2 |
| F8 | Portabilidade | 2 | 2 | 4 |
| F9 | Facilidade de mudança | 1 | 3 | 3 |
| F10 | Concorrência | 1 | 0 | 0 |
| F11 | Requisitos de segurança | 1 | 2 | 2 |
| F12 | Acesso por terceiros | 1 | 0 | 0 |
| F13 | Necessidade de treinamento especial | 1 | 1 | 1 |
| **TFactor** | | | | **25** |

TCF = 0,6 + (0,01 × 25) = **0,85**.

**Passo 4 — Fatores de ambiente (ECF).** Os fatores F7 e F8 têm peso negativo:
quanto maior o valor, mais eles reduzem a produtividade estimada.

**Tabela {{T:ecf}}. Fatores de ambiente (ECF).**

| Fator | Descrição | Peso | Valor (0–5) | Peso × Valor |
|---|---|---|---|---|
| F1 | Familiaridade com o processo de desenvolvimento | 1,5 | 3 | 4,5 |
| F2 | Experiência na aplicação (domínio financeiro) | 0,5 | 2 | 1,0 |
| F3 | Experiência em orientação a objetos | 1 | 4 | 4,0 |
| F4 | Capacidade do analista líder | 0,5 | 3 | 1,5 |
| F5 | Motivação | 1 | 5 | 5,0 |
| F6 | Estabilidade dos requisitos | 2 | 4 | 8,0 |
| F7 | Equipe em tempo parcial | −1 | 5 | −5,0 |
| F8 | Dificuldade da linguagem de programação | −1 | 3 | −3,0 |
| **EFactor** | | | | **16,0** |

ECF = 1,4 − (0,03 × 16,0) = **0,92**.

**Passo 5 — Cálculo final.**
UCP = UUCP × TCF × ECF = 206 × 0,85 × 0,92 = **161,09 pontos**.

Aplicando a taxa clássica de Karner, de 20 horas por ponto, o esforço seria de
3.222 horas — valor coerente com um projeto industrial completo (levantamento,
gestão, testes formais, implantação e garantia), conduzido por equipe alocada em
tempo integral. Para o recorte desta disciplina, a leitura relevante do UCP é
relativa: os casos de uso classificados como complexos (UC005, UC009, UC011 e
UC017) correspondem exatamente aos requisitos com as maiores cartas no Planning
Poker ([RF005], [RF009], [RF012] e [RF018]), o que dá consistência às duas
técnicas. Adotamos como compromisso operacional a estimativa do Planning Poker
(132 horas de implementação), usando o UCP como medida de tamanho funcional e como
verificação da classificação de esforço entre os casos de uso.

\newpage
