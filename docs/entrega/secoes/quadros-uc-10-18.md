**Quadro {{Q}}. Descrição do caso de uso UC010.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC010: Visualizar detalhes do ativo |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita examinar os fundamentos, o histórico de cotações e o parecer de cada regra de análise de um ativo específico. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos um ativo cadastrado; recomendável haver cotações importadas para o ativo. |
| **Pós-condições** | Fundamentos, gráfico de fechamento, histórico de cotações e pareceres por regra do ativo escolhido apresentados ao investidor. Nenhuma informação é gravada. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Detalhes do ativo" na navegação lateral. (ator) | |
| | 2 - O sistema carrega a lista de ativos cadastrados e preenche o seletor com o rótulo completo, composto por ticker (classe Ativo) e nomeEmpresa (classe Ativo). (sistema) (2a) |
| 3 - O ator escolhe o ativo desejado no seletor. (ator) | |
| | 4 - O sistema apresenta o quadro de fundamentos com ticker (classe Ativo), tipo, setor (classe Ativo), precoLucro, dividendYield e valorMercado (classe Acao) ou dividendYield, valorPatrimonialPorCota e segmento (classe FundoImobiliario), além do índice de valor calculado e de sua classificação. (sistema) (4a) |
| | 5 - O sistema recupera o histórico completo de cotações do ativo e desenha o gráfico de linha da série de fechamento, identificada por "TICKER - fechamento" (classe Cotacao). (sistema) |
| | 6 - O sistema preenche a tabela com as 60 cotações mais recentes, da mais nova para a mais antiga, exibindo data, abertura, maxima, minima, fechamento (classe Cotacao), a variação percentual do dia e o volume (classe Cotacao), com a variação destacada em verde na alta e em vermelho na baixa. (sistema) |
| | 7 - O sistema solicita ao motor de análise a reavaliação individual das regras ativas para o ativo e apresenta, no quadro "Pareceres por regra (Strategy)", a numeração, a regraAplicada, o tipo, a pontuacao e a justificativa de cada parecer (classe Recomendacao). (sistema) (7a) |
| **Regras de negócio** | RN015 — A pontuação de uma recomendação varia de −1 a +1; os pareceres exibidos no passo 7 respeitam esse intervalo. |
| | RN018 — Somente regras ativas participam da análise; regras desativadas não geram parecer no quadro do passo 7. |
| | RN019 — Uma regra sem dados suficientes se abstém, sem pontuar, e por isso não aparece na lista de pareceres. |
| | REU002 — Ganho e perda devem ser distinguíveis por cor e por sinal numérico, o que orienta a formatação da coluna de variação no passo 6. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona outro ativo no seletor: o sistema repete os passos 4 a 7 para o ativo recém-escolhido. |
| **Fluxo alternativo 2** | * A qualquer momento o usuário chega a esta tela a partir do painel de análise, por meio da ação de detalhamento do ativo: o sistema posiciona o seletor no ativo recebido e executa os passos 4 a 7. |
| **Fluxo de exceção** | 2a - Caso não exista nenhum ativo cadastrado, o sistema exibe "Nenhum ativo cadastrado." no quadro de fundamentos, apresenta a mensagem "Sem ativo selecionado." na área do gráfico, mantém a tabela e o quadro de pareceres vazios e encerra o caso de uso. |
| | 4a - Caso o ativo selecionado tenha sido removido do cadastro por outro usuário entre os passos 2 e 4, o sistema exibe "Ativo não encontrado." e encerra o caso de uso, retornando ao passo 3 quando o ator escolher outro ativo. |
| | 7a - Caso nenhuma regra ativa consiga opinar sobre o ativo, o sistema exibe "Nenhuma regra ativa conseguiu opinar sobre TICKER. Verifique se há cotações importadas e fundamentos cadastrados." no quadro de pareceres e encerra o caso de uso. |

**Quadro {{Q}}. Descrição do caso de uso UC011.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC011: Gerar recomendações |
| **Atores** | Ator principal: Investidor (o Administrador também executa o caso de uso) |
| **Descrição** | Caso de uso executado quando o usuário solicita ao sistema o parecer consolidado das regras de análise ativas, publicando uma recomendação de compra, venda ou neutralidade para os ativos cadastrados. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos um ativo cadastrado; existir ao menos uma regra de análise ativa; recomendável haver cotações importadas e fundamentos preenchidos. |
| **Pós-condições** | Uma recomendação no estado Vigente publicada para cada ativo analisado, com pontuação, tipo, regras aplicadas e justificativa gravadas; as recomendações anteriores dos mesmos ativos passam ao estado Invalidada. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Recomendações" na navegação lateral. (ator) | |
| | 2 - O sistema apresenta a lista de recomendações vigentes e o esclarecimento de que a pontuação varia de −1 (venda) a +1 (compra), de que entre −0,25 e +0,25 não há sinal claro e de que o parecer é apoio à decisão, não recomendação de investimento (classe Recomendacao). (sistema) |
| 3 - O ator aciona a opção "Gerar para todos os ativos". (ator) | |
| | 4 - O sistema recupera todos os ativos cadastrados (classe Ativo). (sistema) (4a) |
| | 5 - Para cada ativo, o sistema recupera o histórico de cotações (classe Cotacao) e as regras de análise em estado ativo, com nomeRegra, parametroPrincipal e parametroSecundario (classe RegraConfigurada), conforme a Regra de Negócio RN018. (sistema) (5a) |
| | 6 - O sistema monta o motor de análise, instanciando a estratégia correspondente a cada regra configurada — Cruzamento de Médias Móveis, Índice de Força Relativa, Dividend Yield e Preço sobre Lucro — e transmite a cada uma os seus parâmetros (classe RegraConfigurada). (sistema) |
| | 7 - O sistema solicita o parecer individual de cada estratégia sobre o ativo e o seu histórico, obtendo tipo, pontuacao, regraAplicada e justificativa (classe Recomendacao), conforme a Regra de Negócio RN019. (sistema) (7a) |
| | 8 - O sistema calcula a pontuação consolidada como a média aritmética das pontuações dos pareceres obtidos, limita o resultado ao intervalo de −1 a +1 e classifica o tipo em Compra, Venda ou Neutro conforme as Regras de Negócio RN015 e RN016. (sistema) (atributo) |
| | 9 - O sistema registra a data de referência da recomendação como a data da última cotação do histórico, ou a data corrente quando não houver histórico (classe Cotacao). (sistema) (atributo) |
| | 10 - O sistema compõe a justificativa com o cabeçalho consolidado — quantidade de regras, ticker, data de referência, tipo, pontuação e limiar de decisão — seguido do parecer numerado de cada regra, e registra a lista das regras aplicadas (classe Recomendacao). (sistema) (atributo) |
| | 11 - O sistema invalida a recomendação vigente anterior do mesmo ativo, conforme a Regra de Negócio RN017. (sistema) (11a) |
| | 12 - O sistema transita a nova recomendação do estado Gerada para Vigente e grava ativoId, tipo, justificativa, regraAplicada, geradaEm, estado e pontuacao (classe Recomendacao). (sistema) (atributo) (12a) |
| | 13 - O sistema repete os passos 5 a 12 para os demais ativos, atualiza a listagem de recomendações vigentes e informa "N recomendação(ões) publicada(s)." (sistema) |
| **Regras de negócio** | RN015 — A pontuação de uma recomendação varia de −1 a +1; o valor consolidado é truncado a esse intervalo antes de ser gravado. |
| | RN016 — Pontuação maior ou igual a +0,25 é classificada como Compra; menor ou igual a −0,25 como Venda; entre os dois limiares, Neutro. |
| | RN017 — Ao publicar nova recomendação, a anterior do mesmo ativo é invalidada, de modo que nunca existam duas recomendações vigentes para o mesmo ativo. |
| | RN018 — Somente regras ativas participam da análise. |
| | RN019 — Uma regra sem dados suficientes se abstém, sem pontuar: a regra de cruzamento de médias se abstém quando não há dois pontos de cada média ou quando as médias estão praticamente coladas; a regra de força relativa se abstém quando o índice não pode ser calculado ou está na faixa neutra; a regra de dividend yield se abstém quando o indicador não está cadastrado; a regra de preço sobre lucro se abstém quando o ativo não é ação ou quando o indicador não é positivo. |
| | RNF006 — Toda recomendação deve exibir a regra e os números que a justificaram. |
| | REU004 — O sistema deve indicar claramente que as recomendações são apoio à decisão, não aconselhamento financeiro, conforme o texto exibido no passo 2. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário aciona a opção "Analisar agora" no painel de análise: o sistema executa os passos 5 a 12 apenas para o ativo selecionado naquela tela e redesenha o selo de recomendação e a justificativa correspondentes. |
| **Fluxo alternativo 2** | * A qualquer momento o usuário altera as regras de análise na tela de administração: o sistema informa na barra de status "Regras alteradas: gere as recomendações novamente para aplicá-las.", sem alterar as recomendações já publicadas. |
| **Fluxo de exceção** | 4a - Caso não exista nenhum ativo cadastrado, nenhuma recomendação é publicada e o sistema exibe "Nenhuma recomendação foi gerada. Verifique se há ativos cadastrados:" seguido do detalhamento do último erro registrado, encerrando o caso de uso. |
| | 5a - Caso o ativo tenha sido removido do cadastro durante o processamento, o sistema registra "Ativo não encontrado no cadastro.", descarta o ativo e retorna ao passo 5 para o próximo ativo da lista. |
| | 7a - Caso nenhuma regra ativa consiga opinar sobre o ativo, o sistema atribui pontuação 0,00, tipo Neutro, regra aplicada "Nenhuma regra aplicável" e a justificativa "Nenhuma das N regras ativas pôde opinar sobre TICKER. Verifique se o ativo possui histórico de cotações importado e fundamentos cadastrados.", prosseguindo no passo 11. |
| | 11a - Caso a invalidação da recomendação anterior falhe, o sistema registra a mensagem devolvida pelo banco de dados, não publica a nova recomendação — para não deixar duas vigentes para o mesmo ativo — e retorna ao passo 5 para o próximo ativo. |
| | 12a - Caso a transição de estado seja recusada, o sistema exibe "Não foi possível publicar a recomendação gerada." e retorna ao passo 5; caso a gravação falhe, o sistema registra a mensagem devolvida pelo banco de dados e retorna ao passo 5. |

**Quadro {{Q}}. Descrição do caso de uso UC012.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC012: Visualizar recomendações vigentes |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita consultar as recomendações vigentes e a justificativa que as sustenta. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos uma recomendação vigente publicada pelo caso de uso UC011. |
| **Pós-condições** | Recomendações vigentes e a justificativa da recomendação selecionada apresentadas ao investidor. Nenhuma informação é gravada. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Recomendações" na navegação lateral. (ator) | |
| | 2 - O sistema recupera as recomendações no estado Vigente e apresenta, para cada uma, o ticker (classe Ativo), o tipo, a pontuacao, a data de geração e as regras aplicadas (classe Recomendacao), destacando Compra em verde, Venda em vermelho e Neutro em cinza. (sistema) (2a) (2b) |
| 3 - O ator seleciona uma linha da tabela. (ator) | |
| | 4 - O sistema apresenta, no quadro de detalhes, a justificativa gravada com a recomendação (classe Recomendacao). (sistema) |
| | 5 - O sistema reavalia as regras ativas com os dados atuais e acrescenta ao quadro a seção "--- Reavaliação regra por regra (dados atuais) ---", com regraAplicada, tipo e pontuacao de cada parecer (classe Recomendacao). (sistema) |
| 6 - O ator aciona a opção "Abrir no painel", ou dá duplo clique sobre a linha. (ator) | |
| | 7 - O sistema apresenta o painel de análise já posicionado no ativo da recomendação selecionada (classe Ativo). (sistema) |
| **Regras de negócio** | RN016 — Pontuação maior ou igual a +0,25 é Compra; menor ou igual a −0,25 é Venda; entre elas, Neutro — critério refletido nas cores da coluna de recomendação. |
| | RN018 — Somente regras ativas participam da reavaliação apresentada no passo 5. |
| | RNF006 — Toda recomendação deve exibir a regra e os números que a justificaram. |
| | REU002 — Ganho e perda devem ser distinguíveis por cor e por sinal numérico. |
| | REU004 — O sistema deve indicar claramente que as recomendações são apoio à decisão, não aconselhamento financeiro. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário aciona a opção "Gerar para todos os ativos": o sistema executa o caso de uso UC011 e retorna a este fluxo no passo 2, com a listagem atualizada. |
| **Fluxo de exceção** | 2a - Caso não exista nenhuma recomendação vigente, o sistema apresenta a tabela vazia e a mensagem "Nenhuma recomendação vigente. Use "Gerar para todos os ativos"." e encerra o caso de uso. |
| | 2b - Caso o ativo de uma recomendação tenha sido removido do cadastro, o sistema exibe "(removido)" na coluna Ativo daquela linha e prossegue no passo 3; a ação "Abrir no painel" não localiza o ativo e o painel permanece inalterado. |

**Quadro {{Q}}. Descrição do caso de uso UC013.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC013: Comprar ativo |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita registrar na carteira a compra de um ativo, criando uma nova posição ou incorporando a compra a uma posição existente. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos um ativo cadastrado; o investidor possuir carteira principal, criada automaticamente no primeiro acesso à tela. |
| **Pós-condições** | Posição criada com quantidade e preço médio informados, ou posição existente atualizada com a nova quantidade e o preço médio ponderado recalculado; tabela e resumo da carteira atualizados. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Minha carteira" na navegação lateral e aciona a opção "Comprar". (ator) | |
| | 2 - O sistema localiza a carteira principal do usuário, criando-a com o nome "Carteira Principal" e a data corrente caso ainda não exista (classe Carteira). (sistema) (2a) |
| | 3 - O sistema monta a lista de ativos disponíveis, com ticker e nomeEmpresa (classe Ativo), e o preço sugerido de cada um a partir do fechamento da última cotação importada (classe Cotacao). (sistema) (3a) |
| | 4 - O sistema apresenta o formulário de compra com os campos: ativo (classe Ativo), quantidade (classe Posicao), preço unitário (classe Posicao) e data da compra (classe Posicao), preenchendo o preço com o valor sugerido e a data com a data corrente, limitada a não ser futura. (sistema) |
| 5 - O ator escolhe o ativo, informa a quantidade, o preço unitário e a data, e confirma acionando "Comprar". (ator) | |
| | 6 - O sistema valida as informações conforme as Regras de Negócio RN001 e RN013. (sistema) (6a) (6b) |
| | 7 - O sistema verifica se já existe posição do mesmo ativo na carteira. (sistema) |
| | 8 - Existindo posição, o sistema recalcula o precoMedio pela média ponderada entre o custo atual e o custo da nova compra, soma a quantidade e grava a posição atualizada (classe Posicao). (sistema) (atributo) (8a) |
| | 9 - Não existindo posição, o sistema grava uma nova posição com carteiraId, ativoId, quantidade, precoMedio e compradaEm (classe Posicao). (sistema) (atributo) (9a) |
| | 10 - O sistema atualiza a tabela e o resumo consolidado da carteira, conforme o caso de uso UC015. (sistema) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos. |
| | RN013 — A compra de ativo já existente recalcula o preço médio ponderado, somando o custo da posição atual ao custo da nova compra e dividindo pela quantidade total. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso. |
| **Fluxo alternativo 2** | * A qualquer momento o usuário troca o ativo selecionado no formulário: o sistema substitui o preço unitário pelo fechamento da última cotação do novo ativo e exibe "Preço sugerido: último fechamento importado." ou, quando não houver cotação, "Sem cotação importada: informe o preço pago." |
| **Fluxo de exceção** | 2a - Caso a carteira não possa ser localizada nem criada, o sistema exibe a mensagem de erro devolvida pelo serviço — "Usuário inválido." ou a mensagem do banco de dados — e encerra o caso de uso. |
| | 3a - Caso não exista nenhum ativo cadastrado, o sistema exibe "Nenhum ativo cadastrado para comprar." e encerra o caso de uso. |
| | 6a - Caso a carteira ou o ativo não estejam identificados, o sistema exibe "Selecione a carteira e o ativo." e retorna ao passo 4. |
| | 6b - Caso a quantidade informada não seja maior que zero, o sistema exibe "A quantidade comprada deve ser maior que zero."; caso o preço informado não seja maior que zero, exibe "O preço de compra deve ser maior que zero."; em ambos os casos retorna ao passo 4. |
| | 8a - Caso a incorporação à posição existente não possa ser concluída, o sistema exibe "Não foi possível incorporar a compra à posição existente." e retorna ao passo 4; caso a gravação falhe, exibe a mensagem devolvida pelo banco de dados e retorna ao passo 4. |
| | 9a - Caso a gravação da nova posição falhe, o sistema exibe a mensagem devolvida pelo banco de dados e retorna ao passo 4. |

**Quadro {{Q}}. Descrição do caso de uso UC014.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC014: Vender ativo |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita registrar a venda, total ou parcial, de uma posição existente na carteira. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos uma posição na carteira do investidor. |
| **Pós-condições** | Quantidade da posição reduzida pela quantidade vendida, mantido o preço médio; posição removida da carteira quando a quantidade chegar a zero; tabela e resumo da carteira atualizados. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Minha carteira" na navegação lateral e aciona a opção "Vender". (ator) | |
| | 2 - O sistema monta a lista de posições da carteira, com ticker e nomeEmpresa (classe Ativo), quantidade e preço atual de cada posição (classe Posicao). (sistema) (2a) |
| | 3 - O sistema apresenta o formulário de venda com os campos: ativo (classe Ativo) e quantidade (classe Posicao), limitando a quantidade máxima à posição existente e exibindo "Posição atual: N unidade(s)." (sistema) |
| 4 - O ator escolhe a posição, informa a quantidade a vender e confirma acionando "Vender". (ator) | |
| | 5 - O sistema valida as informações conforme as Regras de Negócio RN001 e RN012. (sistema) (5a) (5b) (5c) |
| | 6 - O sistema subtrai a quantidade vendida da quantidade da posição, mantendo o precoMedio inalterado (classe Posicao). (sistema) (atributo) (6a) |
| | 7 - Estando a posição zerada, o sistema a remove da carteira, conforme a Regra de Negócio RN014; caso contrário, grava a posição com a nova quantidade (classe Posicao). (sistema) (atributo) (7a) |
| | 8 - O sistema atualiza a tabela e o resumo consolidado da carteira, conforme o caso de uso UC015. (sistema) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos. |
| | RN012 — A quantidade vendida não pode exceder a posição existente. |
| | RN014 — Posição zerada é removida da carteira. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso. |
| **Fluxo alternativo 2** | * A qualquer momento o usuário troca a posição selecionada no formulário: o sistema reajusta o limite máximo de quantidade e atualiza o texto "Posição atual: N unidade(s)." |
| **Fluxo de exceção** | 2a - Caso a carteira não possua nenhuma posição, o sistema exibe "Não há posições na carteira." e encerra o caso de uso. |
| | 5a - Caso a quantidade informada não seja maior que zero, o sistema exibe "A quantidade vendida deve ser maior que zero." e retorna ao passo 3. |
| | 5b - Caso a posição escolhida não exista mais na carteira, o sistema exibe "Não há posição desse ativo na carteira." e retorna ao passo 3. |
| | 5c - Caso a quantidade informada exceda a posição existente, o sistema exibe "Você possui apenas N unidade(s); não é possível vender M." e retorna ao passo 3. |
| | 6a - Caso a baixa da quantidade não possa ser concluída, o sistema exibe "Não foi possível registrar a venda." e retorna ao passo 3. |
| | 7a - Caso a remoção ou a gravação da posição falhe, o sistema exibe a mensagem devolvida pelo banco de dados e retorna ao passo 3. |

**Quadro {{Q}}. Descrição do caso de uso UC015.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC015: Visualizar resultado da carteira |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita acompanhar o valor investido, o valor atual e o resultado de cada posição e do total da carteira. |
| **Pré-condições** | Usuário autenticado no sistema; recomendável haver posições registradas e cotações importadas para os ativos da carteira. |
| **Pós-condições** | Posições e resumo consolidado da carteira apresentados ao investidor. Nenhuma informação é gravada, exceto a criação automática da carteira principal no primeiro acesso. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Minha carteira" na navegação lateral. (ator) | |
| | 2 - O sistema localiza a carteira principal do usuário pelo usuarioId; não a encontrando, grava uma nova carteira com nome "Carteira Principal" e criadaEm igual à data corrente (classe Carteira). (sistema) (atributo) (2a) |
| | 3 - O sistema recupera as posições da carteira, com quantidade, precoMedio e o custo total de cada uma (classe Posicao). (sistema) |
| | 4 - Para cada posição, o sistema recupera o ticker e o nomeEmpresa do ativo (classe Ativo) e o fechamento da última cotação importada (classe Cotacao), adotando o próprio preço médio como preço atual quando não houver cotação. (sistema) (4a) (4b) |
| | 5 - O sistema calcula, por posição, o valor investido, o valor atual, o lucro ou prejuízo e a rentabilidade percentual (classe Posicao). (sistema) |
| | 6 - O sistema apresenta a tabela com ticker, nome do ativo, quantidade, preço médio, preço atual, valor investido, valor atual e resultado, destacando o resultado em verde quando positivo ou nulo e em vermelho quando negativo. (sistema) |
| | 7 - O sistema soma o valor investido e o valor atual de todas as posições, calcula o resultado consolidado e a rentabilidade percentual da carteira e apresenta a linha "Investido: R$ X   Valor atual: R$ Y   Resultado: R$ Z (W%)" com a mesma distinção de cor. (sistema) (7a) |
| **Regras de negócio** | RN013 — A compra de ativo já existente recalcula o preço médio ponderado, base do valor investido apresentado. |
| | RN014 — Posição zerada é removida da carteira e por isso não figura no resumo. |
| | RNF001 — O sistema deve funcionar sem conexão com a internet, usando as cotações já importadas — o preço atual provém sempre da última cotação importada. |
| | REU002 — Ganho e perda devem ser distinguíveis por cor e por sinal numérico. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário aciona a opção "Recarregar": o sistema repete os passos 3 a 7 com os dados mais recentes. |
| **Fluxo alternativo 2** | * A qualquer momento o sistema conclui a importação de cotações: as telas dependentes são atualizadas e este caso de uso é reexecutado a partir do passo 3. |
| **Fluxo de exceção** | 2a - Caso a carteira não possa ser localizada nem criada, o sistema exibe a mensagem de erro devolvida pelo serviço — "Usuário inválido." ou a mensagem do banco de dados — e encerra o caso de uso. |
| | 4a - Caso o ativo de uma posição tenha sido removido do cadastro, o sistema exibe "(removido)" na coluna Ticker e prossegue no passo 5. |
| | 4b - Caso o ativo não possua cotação importada, o sistema exibe "sem cotação" na coluna Preço atual, adota o preço médio como preço atual e prossegue no passo 5, resultando em lucro ou prejuízo nulo para aquela posição. |
| | 7a - Caso a carteira não possua nenhuma posição, o sistema exibe "Carteira vazia. Use "Comprar" para registrar a primeira posição.", desabilita a opção "Vender" e encerra o caso de uso. |

**Quadro {{Q}}. Descrição do caso de uso UC016.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC016: Cadastrar alerta |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita monitorar uma condição de preço ou de variação diária sobre um ativo, para ser notificado quando ela for satisfeita. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos um ativo cadastrado. |
| **Pós-condições** | Alerta gravado no estado Ativo, vinculado ao usuário e ao ativo, com a condição e o valor de referência informados, passando a ser avaliado pelo caso de uso UC017. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Alertas" na navegação lateral e aciona a opção "Novo alerta". (ator) | |
| | 2 - O sistema monta a lista de ativos disponíveis, com ticker e nomeEmpresa (classe Ativo), e o fechamento da última cotação importada de cada um (classe Cotacao). (sistema) (2a) |
| | 3 - O sistema apresenta o formulário com os campos: ativo (classe Ativo), condicao (classe Alerta) — com as opções "Preço acima de", "Preço abaixo de" e "Variação diária acima de" — e valorReferencia (classe Alerta). (sistema) |
| | 4 - O sistema sugere o valor de referência: para as condições de preço, 5 por cento acima ou abaixo do último fechamento, exibindo "Último fechamento de TICKER: R$ X."; para a condição de variação, o valor de 3 por cento, exibindo "O alerta dispara quando a variação do dia, em módulo, passar do valor informado." (sistema) (4a) |
| 5 - O ator escolhe o ativo, a condição e o valor de referência, e confirma acionando "Criar alerta". (ator) | |
| | 6 - O sistema valida as informações conforme as Regras de Negócio RN001 e RN022. (sistema) (6a) (6b) |
| | 7 - O sistema grava o alerta com usuarioId, ativoId, condicao, valorReferencia, estado igual a Ativo e criadoEm igual à data corrente (classe Alerta). (sistema) (atributo) (7a) |
| | 8 - O sistema atualiza a listagem de alertas e informa "Alerta criado e em monitoramento." (sistema) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos. |
| | RN020 — O alerta só dispara quando a condição é satisfeita pela última cotação; o alerta recém-criado nasce no estado Ativo, apenas monitorando. |
| | RN022 — O valor de referência do alerta deve ser maior que zero. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso. |
| **Fluxo alternativo 2** | * A qualquer momento o usuário troca o ativo ou a condição no formulário: o sistema recalcula a sugestão do valor de referência e ajusta a apresentação do campo, exibindo o prefixo "R$ " para as condições de preço e o sufixo " %" para a condição de variação. |
| **Fluxo alternativo 3** | * A qualquer momento o usuário seleciona um alerta na listagem e aciona "Remover": o sistema solicita a confirmação "Remover definitivamente o alerta selecionado?" e, confirmada, exclui o alerta e atualiza a listagem; recusada, mantém o alerta e encerra o caso de uso. |
| **Fluxo de exceção** | 2a - Caso não exista nenhum ativo cadastrado, o sistema exibe "Cadastre um ativo antes de criar alertas." e encerra o caso de uso. |
| | 4a - Caso o ativo escolhido não possua cotações importadas, o sistema exibe "Este ativo ainda não tem cotações importadas: o alerta só será avaliado depois da importação." e prossegue no passo 5. |
| | 6a - Caso o ativo do alerta não esteja identificado, o sistema exibe "Selecione o ativo do alerta." e retorna ao passo 3; caso o ativo não seja localizado no cadastro, exibe "Ativo não encontrado no cadastro." e retorna ao passo 3. |
| | 6b - Caso o valor de referência não seja maior que zero, o sistema exibe "O valor de referência deve ser maior que zero." e retorna ao passo 3. |
| | 7a - Caso a gravação do alerta falhe, o sistema exibe a mensagem devolvida pelo banco de dados e retorna ao passo 3. |

**Quadro {{Q}}. Descrição do caso de uso UC017.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC017: Avaliar e disparar alertas |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o sistema confronta os alertas em monitoramento com a última cotação importada de cada ativo, dispara os que tiveram a condição satisfeita e notifica os observadores registrados. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos um alerta no estado Ativo; existir cotação importada para o ativo monitorado. |
| **Pós-condições** | Alertas com condição satisfeita transitados do estado Ativo para Disparado, com a data e a hora do disparo gravadas; observadores notificados e notificação apresentada na barra de status; alertas não satisfeitos permanecem no estado Ativo. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Alertas" na navegação lateral e aciona a opção "Avaliar agora". (ator) | |
| | 2 - O sistema recupera os alertas do usuário que estão no estado Ativo, com ativoId, condicao e valorReferencia (classe Alerta). (sistema) (2a) |
| | 3 - Para cada alerta monitorado, o sistema recupera a última cotação importada do ativo (classe Cotacao). (sistema) (3a) |
| | 4 - O sistema determina o valor observado: o fechamento da última cotação para as condições "Preço acima de" e "Preço abaixo de", ou a variação percentual do dia para a condição "Variação diária acima de" (classe Cotacao) (classe Alerta). (sistema) |
| | 5 - O sistema compara o valor observado com o valorReferencia do alerta conforme a Regra de Negócio RN020, considerando o módulo do valor na condição de variação diária. (sistema) (5a) |
| | 6 - Satisfeita a condição, o sistema transita o alerta do estado Ativo para Disparado e registra disparadoEm com a data e a hora correntes (classe Alerta). (sistema) (atributo) (6a) |
| | 7 - O sistema grava o alerta atualizado e contabiliza o disparo. (sistema) (atributo) (7a) |
| | 8 - O sistema recupera o ativo do alerta (classe Ativo) e notifica todos os observadores registrados, transmitindo o alerta, o ativo e o valor observado, conforme o padrão Observer. (sistema) (8a) |
| | 9 - A janela principal, na condição de observador concreto, apresenta na barra de status, por 15 segundos, a mensagem "Alerta disparado: TICKER - CONDIÇÃO (valor observado: X)." (sistema) (classe Ativo) (classe Alerta) |
| | 10 - O sistema repete os passos 3 a 9 para os demais alertas monitorados. (sistema) |
| | 11 - O sistema atualiza a listagem de alertas, com o novo estado e a data e hora do disparo, e informa "N alerta(s) disparado(s)." (sistema) (10a) |
| **Regras de negócio** | RN020 — O alerta só dispara quando a condição é satisfeita pela última cotação: o valor observado deve ser maior que o valor de referência na condição "Preço acima de", menor na condição "Preço abaixo de", e maior em módulo na condição "Variação diária acima de", de modo que uma queda expressiva também satisfaça a condição de variação. |
| | RN021 — Somente alerta já disparado pode ser arquivado; o ciclo de vida da classe Alerta é Ativo → Disparado → Arquivado, e a transição para Disparado só é aceita a partir do estado Ativo. |
| | RNF001 — O sistema deve funcionar sem conexão com a internet, usando as cotações já importadas: a avaliação usa sempre a última cotação existente na base. |
| | RNF007 — O sistema deve responder às interações da interface em menos de 2 segundos com 5 anos de histórico. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário conclui o login e a janela principal é apresentada: 400 milissegundos após a exibição o sistema executa automaticamente os passos 2 a 10, atualiza a tela de alertas e, não havendo disparo, apresenta "Nenhum alerta pendente." na barra de status. |
| **Fluxo alternativo 2** | * A qualquer momento a janela principal é encerrada: o sistema remove a janela da lista de observadores, de modo que nenhuma notificação seja enviada a uma janela em destruição. |
| **Fluxo de exceção** | 2a - Caso o usuário não possua alertas no estado Ativo, nenhuma condição é avaliada e o sistema exibe "Nenhum alerta atingiu a condição nesta avaliação.", encerrando o caso de uso. |
| | 3a - Caso o ativo do alerta não possua cotação importada, o alerta permanece no estado Ativo, em monitoramento, e o sistema retorna ao passo 3 para o próximo alerta. |
| | 5a - Caso a condição não seja satisfeita pelo valor observado, o alerta permanece no estado Ativo e o sistema retorna ao passo 3 para o próximo alerta. |
| | 6a - Caso o alerta não esteja no estado Ativo no momento da transição, a máquina de estados recusa o disparo, nenhuma alteração é gravada e o sistema retorna ao passo 3 para o próximo alerta. |
| | 7a - Caso a gravação do alerta disparado falhe, o sistema registra a mensagem devolvida pelo banco de dados, não contabiliza o disparo nem notifica os observadores, e retorna ao passo 3 para o próximo alerta. |
| | 8a - Caso o ativo do alerta não seja localizado no cadastro, o disparo permanece gravado, porém nenhuma notificação é enviada, e o sistema retorna ao passo 3 para o próximo alerta. |
| | 10a - Caso nenhum alerta tenha sido disparado na avaliação, o sistema exibe "Nenhum alerta atingiu a condição nesta avaliação." em lugar da contagem de disparos. |

**Quadro {{Q}}. Descrição do caso de uso UC018.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC018: Arquivar alerta |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita retirar do acompanhamento corrente um alerta que já foi disparado, preservando o seu registro histórico. |
| **Pré-condições** | Usuário autenticado no sistema; existir ao menos um alerta do investidor no estado Disparado. |
| **Pós-condições** | Alerta transitado do estado Disparado para Arquivado, mantidos a condição, o valor de referência e a data e hora do disparo; listagem de alertas atualizada. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) seleciona a opção "Alertas" na navegação lateral. (ator) | |
| | 2 - O sistema apresenta os alertas do usuário com ticker (classe Ativo), descrição da condicao e do valorReferencia, estado, criadoEm e disparadoEm (classe Alerta), além do fechamento e da data da última cotação do ativo (classe Cotacao), destacando o estado Ativo em verde e o estado Disparado em laranja. (sistema) |
| 3 - O ator seleciona na tabela o alerta já disparado e aciona a opção "Arquivar". (ator) | |
| | 4 - O sistema valida a transição conforme a Regra de Negócio RN021. (sistema) (4a) (4b) |
| | 5 - O sistema altera o estado do alerta para Arquivado e grava a alteração (classe Alerta). (sistema) (atributo) (5a) |
| | 6 - O sistema atualiza a listagem e informa "Alerta arquivado." (sistema) |
| **Regras de negócio** | RN021 — Somente alerta já disparado pode ser arquivado: a transição para Arquivado é aceita apenas a partir do estado Disparado, conforme o ciclo de vida Ativo → Disparado → Arquivado da classe Alerta. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona o alerta e aciona "Remover": o sistema solicita a confirmação "Remover definitivamente o alerta selecionado?" e, confirmada, exclui o alerta em lugar de arquivá-lo, encerrando este caso de uso. |
| **Fluxo de exceção** | 4a - Caso o alerta não seja localizado, o sistema exibe "Alerta não encontrado." em vermelho na área de mensagens e retorna ao passo 2. |
| | 4b - Caso o alerta esteja no estado Ativo ou Arquivado, o sistema recusa a transição e exibe "Somente alertas já disparados podem ser arquivados (estado atual: ESTADO)." em vermelho, retornando ao passo 2. |
| | 5a - Caso a gravação do alerta arquivado falhe, o sistema exibe a mensagem devolvida pelo banco de dados e retorna ao passo 2. |

**Quadro {{Q}}. Descrição do caso de uso UC019.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC019: Sincronizar cotações online |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita atualizar as cotações dos ativos cadastrados diretamente da fonte online, baixando apenas o período que falta em cada ativo, sem depender do arquivo CSV. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); deve existir ao menos um ativo cadastrado (UC002); o computador deve possuir conexão com a internet e backend TLS disponível para o Qt. |
| **Pós-condições** | Cotações do período faltante gravadas para cada ativo atualizado, sem duplicar pregões já existentes; um registro de auditoria por tentativa de sincronização na classe Importacao; histórico de importações e telas dependentes das cotações atualizados. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) seleciona a opção "Importar cotações" na navegação lateral e aciona a opção "Sincronizar online". (ator) | |
| | 2 - O sistema desabilita as opções de importação e de sincronização e apresenta o cursor de espera, de modo que a consulta em curso não seja disparada novamente. (sistema) |
| | 3 - O sistema recupera todos os ativos cadastrados (classe Ativo). (sistema) (3a) |
| | 4 - Para cada ativo, o sistema recupera a última cotação gravada (classe Cotacao) e calcula o início da janela faltante: o dia seguinte à data da última cotação ou, quando o ativo ainda não possui histórico, 365 dias antes da data corrente, conforme a Regra de Negócio RN023. (sistema) (atributo) |
| | 5 - O sistema monta a consulta à fonte remota, acrescentando o sufixo ".SA" ao ticker (classe Ativo) e delimitando o intervalo entre o início da janela e a data corrente (classe FonteYahooFinance). (sistema) (5a) |
| | 6 - O sistema requisita à fonte remota as cotações diárias do intervalo, aguardando no máximo 15 segundos pela resposta (classe FonteCotacoesRemota). (sistema) (6a) (6b) |
| | 7 - O sistema converte a resposta recebida em cotações do domínio, arredondando abertura, maxima, minima e fechamento aos centavos e convertendo o volume (classe Cotacao), descartando os pregões com campo ausente ou com valores inconsistentes e ordenando a série por data crescente. (sistema) (atributo) (7a) (7b) |
| | 8 - O sistema abre uma transação e grava em lote as cotações baixadas, ignorando aquelas cuja data já exista para o mesmo ativo, conforme a Regra de Negócio RN010. (sistema) (atributo) (8a) |
| | 9 - O sistema registra a auditoria da sincronização, identificando a origem no campo arquivo como "Yahoo Finance (TICKER)", gravando linhasLidas e linhasInseridas e transitando o registro para o estado Concluida (classe Importacao). (sistema) (atributo) (9a) |
| | 10 - O sistema confirma a transação, acumula as cotações inseridas e as ignoradas e acrescenta ao relatório a linha "TICKER: N novas, M ignoradas". (sistema) (10a) |
| | 11 - O sistema repete os passos 4 a 10 para os demais ativos cadastrados, sem interromper a sincronização quando um ativo isolado falha, conforme o Requisito Não Funcional RNF008. (sistema) |
| | 12 - O sistema reabilita as opções, recarrega o histórico de importações e apresenta em verde o resumo "Sincronização online: X ativo(s) verificado(s), Y cotação(ões) nova(s), Z ignorada(s) por já existirem.", seguido do detalhamento por ativo. (sistema) (12a) |
| | 13 - Havendo ao menos uma cotação nova, o sistema avisa as demais telas para que recarreguem os dados dependentes das cotações. (sistema) |
| **Regras de negócio** | RN010 — Cotação de mesma data e mesmo ativo é ignorada, sem duplicar o registro já existente: a gravação em lote adota o mesmo critério da importação por arquivo, de modo que acionar a sincronização duas vezes seguidas não duplica pregões. |
| | RN023 — A sincronização busca apenas o período posterior à última cotação local — do dia seguinte à última cotação gravada até a data corrente — ou os últimos 365 dias quando o ativo ainda não possui cotação, e nunca duplica pregões. |
| | RNF001 — O sistema deve funcionar sem conexão com a internet: a importação por CSV permanece como caminho primário e independe deste caso de uso. |
| | RNF008 — A sincronização online é opcional: sem internet o sistema apenas informa e todas as demais funções seguem operando; a falha de um ativo nunca interrompe os demais. |
| **Fluxo alternativo 1** | * A qualquer momento a última cotação gravada de um ativo já alcança a data corrente: o sistema não gasta requisição com esse ativo, acrescenta ao relatório a linha "TICKER: já atualizado (última cotação em DD/MM/AAAA)." e retorna ao passo 4 para o próximo ativo. |
| **Fluxo alternativo 2** | * A qualquer momento o administrador prefere o caminho offline: aciona "Importar arquivo" ou "Importar pasta inteira" e executa o caso de uso UC005, que grava as mesmas cotações a partir de arquivos CSV, sem consultar a fonte online. |
| **Fluxo de exceção** | 3a - Caso não exista nenhum ativo cadastrado, a opção "Sincronizar online" permanece desabilitada; acionada a sincronização sem ativos, nenhuma requisição é feita e o sistema exibe o resumo "Nenhum ativo cadastrado para sincronizar." com o detalhamento "Nenhum ativo cadastrado: cadastre os ativos antes de sincronizar.", encerrando o caso de uso. |
| | 5a - Caso o ativo esteja sem ticker, o sistema registra "Ativo sem ticker: impossível consultar a fonte online."; caso o intervalo calculado seja inválido, registra "Intervalo de datas inválido para a consulta online."; em ambos os casos contabiliza o ativo com falha e retorna ao passo 4 para o próximo ativo. |
| | 6a - Caso o computador não possua backend TLS (OpenSSL) disponível para o Qt, o sistema não realiza a consulta e registra "Este computador não tem backend TLS (OpenSSL) disponível para o Qt; a sincronização online exige HTTPS. Use a importação por CSV.", contabiliza o ativo com falha e retorna ao passo 4, conforme o Requisito Não Funcional RNF008. |
| | 6b - Caso não haja conexão com a internet ou a fonte não responda, o sistema registra "A fonte online não respondeu em 15 segundos." no esgotamento do tempo limite, ou "Falha de rede ao consultar a fonte online: DESCRIÇÃO." nas demais falhas; contabiliza o ativo com falha, acrescenta ao relatório a linha "TICKER: falha — MOTIVO", grava a auditoria no estado Rejeitada com o motivo (classe Importacao) e retorna ao passo 4 para o próximo ativo. |
| | 7a - Caso a resposta esteja fora do formato esperado, o sistema registra "Resposta da fonte não é um JSON válido (DETALHE).", "Resposta da fonte fora do formato esperado (sem "chart").", "A fonte não devolveu nenhum resultado para o ativo." ou "Resposta da fonte sem o bloco de preços (indicators.quote)."; caso a própria fonte recuse a consulta sem esclarecer, registra a descrição devolvida por ela ou "a fonte recusou a consulta sem detalhar o motivo."; em todos os casos o ativo é contabilizado com falha, a auditoria é gravada no estado Rejeitada e o sistema retorna ao passo 4. |
| | 7b - Caso não tenha havido pregão no intervalo consultado — fim de semana ou feriado prolongado —, a fonte devolve a série vazia, nada é gravado nem auditado e o sistema acrescenta ao relatório a linha "TICKER: nenhuma cotação nova entre DD/MM/AAAA e DD/MM/AAAA.", retornando ao passo 4 para o próximo ativo. |
| | 8a - Caso a transação não possa ser iniciada ou a gravação em lote falhe, o sistema desfaz a transação, grava a auditoria no estado Rejeitada com o motivo — "Falha ao gravar as cotações baixadas." ou a mensagem devolvida pelo banco de dados —, acrescenta ao relatório a linha "TICKER: falha ao gravar — MOTIVO" e retorna ao passo 4 para o próximo ativo. |
| | 9a - Caso a gravação do próprio registro de auditoria falhe, o sistema desfaz a transação — para que as cotações do ativo não fiquem gravadas sem auditoria — e retorna ao passo 4 com a mensagem devolvida pelo repositório de importações. |
| | 10a - Caso a confirmação da transação seja recusada, o sistema desfaz a transação, registra a mensagem devolvida pelo banco de dados e retorna ao passo 4 para o próximo ativo. |
| | 12a - Caso ao menos um ativo não tenha podido ser atualizado, o sistema acrescenta ao resumo a frase "N ativo(s) não puderam ser atualizados." e apresenta o texto em vermelho, mantendo gravadas as cotações dos ativos bem-sucedidos. |

**Quadro {{Q}}. Descrição do caso de uso UC020.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC020: Executar backtesting das regras |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita medir o desempenho histórico das regras de análise, reaplicando-as em cada pregão do histórico importado e conferindo cada sinal emitido contra o que o preço fez no horizonte escolhido. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); deve existir ao menos um ativo cadastrado (UC002); deve existir ao menos uma regra de análise ativa; o ativo avaliado deve possuir histórico de cotações importado com pelo menos 30 pregões de formação mais o horizonte escolhido. |
| **Pós-condições** | Quantidade de sinais, quantidade de acertos, taxa de acerto e retorno médio apresentados para cada regra ativa e para o parecer consolidado de cada ativo avaliado. Nenhuma informação é gravada. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) seleciona a opção "Regras de análise" na navegação lateral e aciona a opção "Backtesting…". (ator) | |
| | 2 - O sistema apresenta a janela "Backtesting das regras" com a explicação "As regras ativas são reaplicadas em cada pregão do histórico importado, usando apenas os dados disponíveis naquela data. Cada sinal é conferido contra o que o preço fez no horizonte escolhido." e o rodapé "Backtesting reexecuta as regras em cada ponto do histórico e mede o resultado no horizonte escolhido. Apoio ao estudo, não é garantia de resultado futuro.", conforme o Requisito de Usabilidade REU004. (sistema) (2a) |
| | 3 - O sistema preenche o seletor de ativo com a opção "Todos os ativos" e com o rótulo completo de cada ativo cadastrado, composto por ticker e nomeEmpresa (classe Ativo), e apresenta o campo de horizonte com o valor padrão de 10 pregões, limitado ao intervalo de 1 a 60. (sistema) |
| 4 - O ator escolhe o ativo — um em particular ou todos —, informa o horizonte de avaliação e aciona a opção "Executar backtesting". (ator) | |
| | 5 - O sistema recupera as regras de análise em estado ativo, com nomeRegra, parametroPrincipal, parametroSecundario e peso (classe RegraConfigurada), conforme as Regras de Negócio RN018 e RN025. (sistema) |
| | 6 - O sistema monta o motor de análise uma única vez, instanciando a estratégia de cada regra configurada, e prepara uma linha de apuração por regra registrada, além da linha "Consolidado (média ponderada)" (classe Backtester). (sistema) |
| | 7 - Para cada ativo selecionado, o sistema recupera o histórico completo de cotações, ordenado por data crescente (classe Cotacao). (sistema) |
| | 8 - O sistema percorre o histórico a partir do 30º pregão até o último pregão que ainda comporta o horizonte informado e, em cada ponto, recorta a série com tudo o que estava disponível naquela data, de modo que nenhum dado futuro participe da decisão das regras. (sistema) (8a) (8b) |
| | 9 - O sistema calcula o retorno realizado entre o fechamento do pregão do sinal e o fechamento do pregão situado ao fim do horizonte (classe Cotacao). (sistema) (atributo) |
| | 10 - O sistema reaplica o motor sobre o recorte, obtendo o parecer consolidado e o parecer individual de cada regra ativa, com tipo, pontuacao e regraAplicada (classe Recomendacao), conforme a Regra de Negócio RN019. (sistema) |
| | 11 - O sistema contabiliza cada parecer conforme a Regra de Negócio RN024: pareceres Neutro não constituem sinal; pareceres Compra e Venda acrescentam um sinal, somam o retorno de quem seguisse o sinal — com o valor invertido na venda — e contam um acerto quando a direção se confirmou. (sistema) (atributo) |
| | 12 - O sistema repete os passos 9 a 11 para os demais pregões do histórico e os passos 7 a 11 para os demais ativos selecionados. (sistema) |
| | 13 - O sistema apresenta a tabela com Ativo, Regra, Sinais, Acertos, Taxa de acerto e Retorno médio, uma linha por regra e a linha consolidada em negrito, com a taxa em percentual inteiro e o retorno médio com sinal, destacado em verde quando positivo e em vermelho quando negativo, conforme o Requisito de Usabilidade REU002. (sistema) |
| | 14 - O sistema informa "N ativo(s) avaliado(s) com M regra(s) ativa(s) e horizonte de H pregão(ões)." (sistema) (14a) |
| **Regras de negócio** | RN016 — Pontuação maior ou igual a +0,25 é Compra; menor ou igual a −0,25 é Venda; entre os dois limiares, Neutro — critério que define se o parecer consolidado de cada pregão constitui sinal. |
| | RN018 — Somente regras ativas participam da análise; regras desativadas não recebem linha na apuração. |
| | RN019 — Uma regra sem dados suficientes se abstém, sem pontuar: naquele pregão ela não emite parecer e, portanto, nenhum sinal lhe é contabilizado. |
| | RN024 — No backtesting, Compra acerta quando o retorno no horizonte é positivo e Venda acerta quando é negativo; pareceres Neutro não geram sinal. O retorno acumulado é o de quem seguiu o sinal, de modo que operar vendido em uma queda conta como ganho. |
| | RN025 — O peso de uma regra deve ser maior que zero e a pontuação consolidada é a média ponderada pelos pesos, base da linha "Consolidado (média ponderada)". |
| | REU002 — Ganho e perda devem ser distinguíveis por cor e por sinal numérico, o que orienta a formatação da coluna Retorno médio no passo 13. |
| | REU004 — O sistema deve indicar claramente que as recomendações são apoio à decisão, não aconselhamento financeiro, conforme o rodapé apresentado no passo 2. |
| **Fluxo alternativo 1** | * A qualquer momento o administrador troca o ativo ou o horizonte e aciona novamente "Executar backtesting": o sistema limpa a tabela e repete os passos 5 a 14 com a nova configuração. |
| **Fluxo alternativo 2** | * A qualquer momento o administrador edita os parâmetros de uma regra ou a ativa e desativa na tela de regras e reabre o backtesting: o sistema utiliza a configuração que está valendo naquele momento, permitindo comparar o efeito do ajuste sobre o mesmo histórico. |
| **Fluxo de exceção** | 2a - Caso não exista nenhum ativo cadastrado, o sistema desabilita a opção "Executar backtesting" e exibe "Nenhum ativo cadastrado: cadastre ativos e importe cotações para usar o backtesting.", encerrando o caso de uso. |
| | 8a - Caso o histórico do ativo não alcance os 30 pregões de formação mais o horizonte informado, nenhum pregão é avaliado e o sistema apresenta, em linha única para aquele ativo, a observação "Histórico com apenas N pregão(ões). O backtesting exige M: 30 para as regras se formarem e mais H para medir o resultado de cada sinal.", prosseguindo no passo 7 para o próximo ativo. |
| | 8b - Caso nenhum pregão possa ser avaliado por não haver preço de fechamento válido no período, o sistema apresenta, em linha única para aquele ativo, a observação "Nenhum pregão pôde ser avaliado: as cotações do período não têm preços válidos.", prosseguindo no passo 7 para o próximo ativo. |
| | 14a - Caso nenhum ativo tenha sido avaliado — por exemplo, quando o ativo escolhido foi removido do cadastro entre os passos 3 e 4 —, o sistema apresenta a tabela vazia com a mensagem "Nenhum ativo selecionado para o backtesting." e encerra o caso de uso. |
