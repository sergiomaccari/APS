### Dicionário de informações

O Dicionário de Informações é a listagem organizada e rigorosa de todos os elementos de
dados pertinentes ao sistema, de modo que usuário e desenvolvedor conheçam com precisão o
significado, o tamanho, o tipo, o formato e o domínio de cada atributo das classes
modeladas. Ele constitui, portanto, o conjunto de metadados do Analisador B3: enquanto o
Diagrama de Classes mostra a estrutura e os relacionamentos, o Dicionário de Informações
detalha o conteúdo admissível de cada atributo, servindo de contrato entre a análise, o
projeto do banco de dados e a implementação em C++.

Os quadros desta seção descrevem as classes das camadas de **domínio**, de **análise** e de
**persistência**, exatamente como foram implementadas no código-fonte (diretórios
`src/dominio/`, `src/analise/` e `src/persistencia/`). Os nomes dos atributos são os nomes
reais dos membros das classes, com o prefixo `m_` adotado como convenção do projeto para
atributos de instância; constantes de classe aparecem em maiúsculas, sem prefixo.

Convenções adotadas nas colunas dos quadros, conforme a notação apresentada em aula:

- **Tamanho** — número de bytes ocupados pelo tipo, no caso dos tipos numéricos e lógicos,
  ou o limite máximo de caracteres adotado, no caso dos tipos textuais e de data.
- **Formato** — `9` representa dígito numérico, `X` caractere alfanumérico e `A` caractere
  alfabético; as iterações seguem a notação `{X}60` (no máximo 60), `1{X}60` (de 1 a 60) e
  `1{9}` (mínimo de um dígito, sem máximo); datas usam `dd/mm/aaaa` e as seleções são
  representadas entre colchetes, como `[Sim | Não]`.
- **Domínio** — *Contínuo* quando o atributo admite qualquer valor de um intervalo, e
  *Discreto* quando admite apenas valores determinados, caso em que são listados os valores
  e os respectivos significados.
- Atributos que referenciam outras classes do sistema (agregações, composições e
  estratégias) são declarados com tipo **Objeto (Classe)** e domínio **Discreto**, limitado
  às instâncias válidas da classe referenciada.

Em conformidade com a regra de herança aplicável ao Dicionário de Informações, as classes
especializadas `Acao` e `FundoImobiliario` repetem a descrição dos atributos herdados da
superclasse `Ativo`, acrescentando os seus atributos próprios. O quadro da classe abstrata
`Ativo` é apresentado apenas a título de documentação da raiz da hierarquia, uma vez que
essa classe não é instanciada diretamente pelo sistema.

---

**Quadro {{Q}}. Dicionário de informações da classe Usuario.**

*Usuario: identifica quem acessa o sistema, define o seu perfil de acesso e é o proprietário das carteiras e dos alertas.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único do usuário, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo: 1 a 9.223.372.036.854.775.807 |
| m_nome | Nome completo do usuário | 60 | Alfanumérico (QString) | 1{X}60 | Contínuo |
| m_email | Endereço de correio eletrônico, chave natural usada no login | 80 | Alfanumérico (QString) | 1{X}80 | Contínuo, no padrão `usuario@dominio.ext` |
| m_senhaHash | Resumo criptográfico SHA-256 do sal concatenado à senha; a senha em texto puro nunca é armazenada | 64 | Alfanumérico (QString) | {X}64 | Contínuo: dígitos hexadecimais de 0 a f |
| m_papel | Perfil de acesso do usuário no sistema | 4 | Enumerado (PapelUsuario) | {A}13 | Discreto: Investidor = consulta análises, mantém carteira e alertas; Administrador = também mantém ativos, importa cotações, configura regras e gerencia usuários |

---

**Quadro {{Q}}. Dicionário de informações da classe Ativo (abstrata).**

*Ativo: raiz abstrata da hierarquia de ativos negociados na B3; concentra os dados de identificação comuns a ações e a fundos imobiliários.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único do ativo, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_ticker | Código de negociação do ativo na bolsa | 6 | Alfanumérico (QString) | 4{X}6 | Contínuo: de 4 a 6 caracteres alfanuméricos, único no sistema (RN005) |
| m_nomeEmpresa | Razão social ou nome de pregão do emissor do ativo | 80 | Alfanumérico (QString) | 1{X}80 | Contínuo |
| m_setor | Setor econômico de atuação do emissor | 40 | Alfanumérico (QString) | {X}40 | Contínuo |

---

**Quadro {{Q}}. Dicionário de informações da classe Acao.**

*Acao: ação negociada na B3; especializa Ativo acrescentando os fundamentos Preço/Lucro, Dividend Yield e Valor de Mercado.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único do ativo, herdado de Ativo | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_ticker | Código de negociação do ativo na bolsa, herdado de Ativo | 6 | Alfanumérico (QString) | 4{X}6 | Contínuo: de 4 a 6 caracteres alfanuméricos, único no sistema |
| m_nomeEmpresa | Razão social da companhia emissora, herdado de Ativo | 80 | Alfanumérico (QString) | 1{X}80 | Contínuo |
| m_setor | Setor econômico de atuação, herdado de Ativo | 40 | Alfanumérico (QString) | {X}40 | Contínuo |
| m_precoLucro | Índice Preço/Lucro: cotação dividida pelo lucro por ação; valores menores indicam ação relativamente mais barata | 8 | Numérico (double) | {9}4,99 | Contínuo: 0,00 a 9.999,99 |
| m_dividendYield | Percentual de proventos distribuídos nos últimos doze meses | 8 | Numérico (double) | {9}3,99 | Contínuo: 0,00 a 100,00 |
| m_valorMercado | Valor de mercado da companhia, em reais | 8 | Numérico (double) | {9}15,99 | Contínuo: R$ 0,00 a R$ 999.999.999.999.999,99 |
| PRECO_LUCRO_REFERENCIA | Constante de classe: índice Preço/Lucro de referência (10,0) acima do qual a pontuação fundamentalista é reduzida à metade no cálculo de indiceValor() | 8 | Numérico (double, constante) | {9}4,99 | Contínuo: valor fixo 10,00 |
| DIVIDEND_YIELD_MAXIMO | Constante de classe: Dividend Yield (8,0%) que já garante pontuação máxima no critério de proventos | 8 | Numérico (double, constante) | {9}3,99 | Contínuo: valor fixo 8,00 |

---

**Quadro {{Q}}. Dicionário de informações da classe FundoImobiliario.**

*FundoImobiliario: fundo de investimento imobiliário negociado na B3; especializa Ativo acrescentando Dividend Yield, Valor Patrimonial por Cota e segmento de atuação.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único do ativo, herdado de Ativo | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_ticker | Código de negociação do fundo na bolsa, herdado de Ativo | 6 | Alfanumérico (QString) | 4{X}6 | Contínuo: de 4 a 6 caracteres alfanuméricos, único no sistema |
| m_nomeEmpresa | Nome do fundo, herdado de Ativo | 80 | Alfanumérico (QString) | 1{X}80 | Contínuo |
| m_setor | Setor econômico de atuação, herdado de Ativo | 40 | Alfanumérico (QString) | {X}40 | Contínuo |
| m_dividendYield | Percentual de rendimentos distribuídos nos últimos doze meses | 8 | Numérico (double) | {9}3,99 | Contínuo: 0,00 a 100,00 |
| m_valorPatrimonialPorCota | Valor patrimonial de cada cota do fundo, em reais | 8 | Numérico (double) | {9}6,99 | Contínuo: R$ 0,00 a R$ 999.999,99 |
| m_segmento | Segmento imobiliário de atuação do fundo | 30 | Alfanumérico (QString) | {X}30 | Discreto: Logística; Lajes Corporativas; Papel; Shoppings; Renda Urbana; Híbrido |
| DIVIDEND_YIELD_MAXIMO | Constante de classe: Dividend Yield (12,0%) que já garante pontuação máxima no critério de rendimentos | 8 | Numérico (double, constante) | {9}3,99 | Contínuo: valor fixo 12,00 |
| VALOR_PATRIMONIAL_REFERENCIA | Constante de classe: valor patrimonial por cota de referência (R$ 200,00) para pontuação máxima de solidez | 8 | Numérico (double, constante) | {9}6,99 | Contínuo: valor fixo 200,00 |

---

**Quadro {{Q}}. Dicionário de informações da classe Cotacao.**

*Cotacao: cotação diária de um ativo no formato candle OHLCV (abertura, máxima, mínima, fechamento e volume); é a base de todos os cálculos técnicos do sistema.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único da cotação, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_ativoId | Identificador do ativo a que a cotação pertence | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Ativo |
| m_data | Data do pregão a que se refere a cotação | 10 | Data (QDate) | dd/mm/aaaa | Contínuo: 01/01/1990 até a data corrente |
| m_abertura | Preço do primeiro negócio do pregão, em reais | 8 | Numérico (double) | {9}9,99 | Contínuo: maior que R$ 0,00 |
| m_maxima | Maior preço negociado no pregão, em reais | 8 | Numérico (double) | {9}9,99 | Contínuo: maior ou igual à abertura e ao fechamento (RN009) |
| m_minima | Menor preço negociado no pregão, em reais | 8 | Numérico (double) | {9}9,99 | Contínuo: menor ou igual à abertura e ao fechamento (RN009) |
| m_fechamento | Preço do último negócio do pregão, em reais; é o valor usado nos indicadores técnicos | 8 | Numérico (double) | {9}9,99 | Contínuo: maior que R$ 0,00 |
| m_volume | Quantidade total de títulos negociados no pregão | 8 | Numérico (qint64) | {9}18 | Contínuo: maior ou igual a zero |

---

**Quadro {{Q}}. Dicionário de informações da classe Indicador.**

*Indicador: valor de um indicador técnico calculado para uma data específica da série histórica, produzido pela CalculadoraIndicadores.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| nome | Nome do indicador, formado pela sigla e pelo período utilizado | 20 | Alfanumérico (QString) | 1{X}20 | Discreto: MMS n = média móvel simples de n períodos; MME n = média móvel exponencial de n períodos; RSI n = índice de força relativa de n períodos |
| data | Data do pregão a que o valor calculado se refere | 10 | Data (QDate) | dd/mm/aaaa | Contínuo |
| valor | Valor numérico calculado do indicador na data | 8 | Numérico (double) | {9}9,99 | Contínuo: preços em reais para as médias móveis; 0,00 a 100,00 para o RSI |

---

**Quadro {{Q}}. Dicionário de informações da classe Carteira.**

*Carteira: carteira de investimentos de um usuário; agrega, por composição, as posições dos ativos que ele possui.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único da carteira, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_usuarioId | Identificador do usuário proprietário da carteira | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Usuario |
| m_nome | Nome atribuído à carteira pelo usuário | 40 | Alfanumérico (QString) | 1{X}40 | Contínuo |
| m_criadaEm | Data de criação da carteira | 10 | Data (QDate) | dd/mm/aaaa | Contínuo |
| m_posicoes | Coleção das posições que compõem a carteira, uma por ativo distinto | Variável | Objeto (Posicao) | — | Discreto: instâncias válidas da classe Posicao vinculadas a esta carteira |

---

**Quadro {{Q}}. Dicionário de informações da classe Posicao.**

*Posicao: posição de um ativo dentro de uma carteira, com a quantidade detida, o preço médio ponderado de aquisição e a data da compra.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único da posição, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_carteiraId | Identificador da carteira a que a posição pertence | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Carteira |
| m_ativoId | Identificador do ativo detido na posição | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Ativo |
| m_quantidade | Quantidade de títulos do ativo atualmente detida | 4 | Numérico (int) | {9}9 | Contínuo: maior que zero; posição zerada é removida da carteira (RN014) |
| m_precoMedio | Preço médio ponderado de aquisição do ativo, em reais, recalculado a cada nova compra (RN013) | 8 | Numérico (double) | {9}9,99 | Contínuo: maior que R$ 0,00 |
| m_compradaEm | Data da compra que originou a posição | 10 | Data (QDate) | dd/mm/aaaa | Contínuo |

---

**Quadro {{Q}}. Dicionário de informações da classe Alerta.**

*Alerta: condição de preço ou de variação cadastrada por um usuário sobre um ativo, monitorada pelo sistema a cada nova cotação.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único do alerta, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_usuarioId | Identificador do usuário que cadastrou o alerta | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Usuario |
| m_ativoId | Identificador do ativo monitorado pelo alerta | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Ativo |
| m_condicao | Tipo de condição monitorada | 4 | Enumerado (TipoCondicaoAlerta) | {A}21 | Discreto: PrecoAcimaDe = dispara quando o fechamento supera o valor de referência; PrecoAbaixoDe = dispara quando o fechamento fica abaixo do valor de referência; VariacaoDiariaAcimaDe = dispara quando a variação percentual do dia supera o valor de referência |
| m_valorReferencia | Valor com que a cotação é comparada: preço em reais ou variação percentual, conforme a condição | 8 | Numérico (double) | {9}9,99 | Contínuo: maior que zero (RN022) |
| m_estado | Estado do alerta na sua máquina de estados | 4 | Enumerado (EstadoAlerta) | {A}9 | Discreto: Ativo = ainda em monitoração; Disparado = condição satisfeita e usuário notificado; Arquivado = encerrado pelo usuário após o disparo |
| m_criadoEm | Data de cadastro do alerta | 10 | Data (QDate) | dd/mm/aaaa | Contínuo |
| m_disparadoEm | Data e hora do disparo do alerta; permanece nulo enquanto o alerta não dispara | 19 | Data e hora (QDateTime) | dd/mm/aaaa hh:mm:ss | Contínuo, admitindo valor nulo |

---

**Quadro {{Q}}. Dicionário de informações da classe Recomendacao.**

*Recomendacao: parecer de compra, venda ou neutralidade gerado pelo MotorAnalise para um ativo, acompanhado da pontuação e da justificativa numérica que o sustentam.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único da recomendação, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_ativoId | Identificador do ativo a que a recomendação se refere | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Ativo |
| m_tipo | Tipo do parecer emitido | 4 | Enumerado (TipoRecomendacao) | {A}7 | Discreto: Compra = pontuação maior ou igual a +0,25; Venda = pontuação menor ou igual a −0,25; Neutro = pontuação entre os dois limiares (RN016) |
| m_justificativa | Texto que apresenta, regra a regra, os números que levaram à conclusão (RNF006) | 500 | Alfanumérico (QString) | {X}500 | Contínuo |
| m_regraAplicada | Nome canônico da regra ou das regras que produziram o parecer | 120 | Alfanumérico (QString) | {X}120 | Discreto: Cruzamento de Medias Moveis; Indice de Forca Relativa; Dividend Yield; Preco sobre Lucro; ou a combinação delas |
| m_geradaEm | Data em que a recomendação foi produzida | 10 | Data (QDate) | dd/mm/aaaa | Contínuo |
| m_estado | Estado da recomendação na sua máquina de estados | 4 | Enumerado (EstadoRecomendacao) | {A}10 | Discreto: Gerada = produzida e ainda não publicada; Vigente = publicada ao investidor; Expirada = perdeu validade pelo decurso do tempo; Invalidada = substituída por análise mais recente (RN017) |
| m_pontuacao | Pontuação agregada das regras que opinaram: negativa indica venda, positiva indica compra e próxima de zero indica ausência de sinal claro | 8 | Numérico (double) | −9,99 a 9,99 | Contínuo: −1,00 a +1,00 (RN015) |
| LIMIAR_DECISAO | Constante de classe: limiar de pontuação (0,25) que separa o parecer Neutro dos pareceres de Compra e de Venda | 8 | Numérico (double, constante) | 9,99 | Contínuo: valor fixo 0,25 |

---

**Quadro {{Q}}. Dicionário de informações da classe RegraConfigurada.**

*RegraConfigurada: configuração persistida de uma regra de análise; permite ligar, desligar e reparametrizar as estratégias do motor sem recompilar o sistema.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único da configuração, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_nomeRegra | Nome canônico da estratégia, que deve coincidir com o nome devolvido por RegraAnalise::nome() | 60 | Alfanumérico (QString) | 1{X}60 | Discreto: Cruzamento de Medias Moveis; Indice de Forca Relativa; Dividend Yield; Preco sobre Lucro |
| m_ativa | Indica se a regra participa da análise; somente as regras ativas são executadas (RN018) | 1 | Lógico (bool) | [Sim \| Não] | Discreto: 1 = Sim, regra em uso; 0 = Não, regra desligada |
| m_parametroPrincipal | Primeiro parâmetro da regra; o significado depende da estratégia: período da média curta, limite de sobrevenda, Dividend Yield mínimo ou Preço/Lucro atrativo | 8 | Numérico (double) | {9}4,99 | Contínuo: maior ou igual a zero |
| m_parametroSecundario | Segundo parâmetro da regra: período da média longa, limite de sobrecompra, Dividend Yield excelente ou Preço/Lucro elevado | 8 | Numérico (double) | {9}4,99 | Contínuo: maior ou igual a zero |
| m_peso | Peso da regra na média ponderada com que o MotorAnalise consolida os pareceres; deve ser maior que zero (RN025) | 8 | Numérico (double) | 9,9 | Contínuo: maior que 0,0, de 0,1 a 5,0 na tela de configuração; valor padrão 1,0 |

---

**Quadro {{Q}}. Dicionário de informações da classe Importacao.**

*Importacao: registro de auditoria de uma carga de cotações a partir de arquivo CSV, com o resultado da validação e as contagens de linhas processadas.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_id | Identificador único da importação, gerado pelo banco de dados | 8 | Numérico (qint64) | 1{9} | Contínuo |
| m_ativoId | Identificador do ativo cujas cotações foram importadas | 8 | Numérico (qint64) | 1{9} | Discreto: identificadores existentes na classe Ativo |
| m_arquivo | Caminho completo do arquivo CSV processado | 255 | Alfanumérico (QString) | 1{X}255 | Contínuo |
| m_estado | Estado da importação na sua máquina de estados | 4 | Enumerado (EstadoImportacao) | {A}9 | Discreto: Pendente = registrada e ainda não processada; Validando = arquivo em conferência; Concluida = cotações gravadas com sucesso; Rejeitada = arquivo recusado, sem gravar nenhuma linha (RN011) |
| m_linhasLidas | Quantidade de linhas de dados lidas do arquivo | 4 | Numérico (int) | {9}9 | Contínuo: maior ou igual a zero |
| m_linhasInseridas | Quantidade de cotações efetivamente gravadas; é menor que as linhas lidas quando há cotações já existentes (RN010) | 4 | Numérico (int) | {9}9 | Contínuo: de zero até o valor de m_linhasLidas |
| m_mensagemErro | Motivo da rejeição, indicando a linha e a causa do problema, em português (REU003) | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a importação é concluída com sucesso |
| m_executadaEm | Data e hora em que a importação foi processada | 19 | Data e hora (QDateTime) | dd/mm/aaaa hh:mm:ss | Contínuo, admitindo valor nulo enquanto pendente |

---

**Quadro {{Q}}. Dicionário de informações da interface RegraAnalise.**

*RegraAnalise: interface do padrão de projeto Strategy; define o contrato que toda regra de análise deve cumprir para ser combinada pelo MotorAnalise. Por ser uma interface, não possui atributos, sendo descrita por suas operações.*

| Método | Descrição | Retorno | Parâmetros | Domínio do retorno |
|---|---|---|---|---|
| nome() | Nome canônico da regra; deve ser idêntico ao gravado em RegraConfigurada para que a fábrica do motor localize a estratégia | Alfanumérico (QString) | — | Discreto: Cruzamento de Medias Moveis; Indice de Forca Relativa; Dividend Yield; Preco sobre Lucro |
| descricao() | Explicação do critério aplicado pela regra, exibida ao administrador na tela de configuração de regras | Alfanumérico (QString) | — | Contínuo: até 255 caracteres |
| configurar() | Ajusta os parâmetros da estratégia com os valores lidos de RegraConfigurada | Sem retorno (void) | parametroPrincipal, parametroSecundario (double) | — |
| avaliar() | Avalia o ativo diante da sua série histórica e devolve a recomendação justificada, ou nenhuma opinião quando os dados são insuficientes ou o critério não se aplica ao tipo do ativo (RN019) | Objeto opcional (Recomendacao) | ativo (Ativo), historico (coleção de Cotacao) | Discreto: instância de Recomendacao com pontuação de −1,00 a +1,00, ou ausência de parecer |

---

**Quadro {{Q}}. Dicionário de informações da classe RegraCruzamentoMedias.**

*RegraCruzamentoMedias: estratégia técnica que compara duas médias móveis; sinaliza compra quando a média curta cruza para cima da média longa e venda no cruzamento inverso.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_periodoCurto | Quantidade de pregões da média móvel curta, configurada pelo parâmetro principal da regra | 4 | Numérico (int) | {9}3 | Contínuo: 2 a 200 pregões; valor padrão 9 |
| m_periodoLongo | Quantidade de pregões da média móvel longa, configurada pelo parâmetro secundário da regra | 4 | Numérico (int) | {9}3 | Contínuo: maior que m_periodoCurto, até 200 pregões; valor padrão 21 |

---

**Quadro {{Q}}. Dicionário de informações da classe RegraRsi.**

*RegraRsi: estratégia técnica baseada no Índice de Força Relativa de Wilder; indica compra em situação de sobrevenda e venda em situação de sobrecompra.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_limiteSobrevenda | Valor do RSI abaixo do qual o ativo é considerado descontado, gerando sinal de compra | 8 | Numérico (double) | {9}3,99 | Contínuo: 0,00 a 100,00; valor padrão 30,00 |
| m_limiteSobrecompra | Valor do RSI acima do qual o ativo é considerado esticado, gerando sinal de venda | 8 | Numérico (double) | {9}3,99 | Contínuo: 0,00 a 100,00, maior que m_limiteSobrevenda; valor padrão 70,00 |
| m_periodo | Quantidade de pregões usada no cálculo do RSI | 4 | Numérico (int) | {9}3 | Contínuo: 2 a 200 pregões; valor padrão 14 |

---

**Quadro {{Q}}. Dicionário de informações da classe RegraDividendYield.**

*RegraDividendYield: estratégia fundamentalista que avalia o percentual de proventos distribuídos; aplica-se tanto a ações quanto a fundos imobiliários, lendo o fundamento de forma polimórfica.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_dividendYieldMinimo | Dividend Yield a partir do qual o ativo passa a ser considerado atrativo | 8 | Numérico (double) | {9}3,99 | Contínuo: 0,00 a 100,00; valor padrão 6,00 |
| m_dividendYieldExcelente | Dividend Yield a partir do qual o ativo recebe a pontuação máxima do critério | 8 | Numérico (double) | {9}3,99 | Contínuo: 0,00 a 100,00, maior que m_dividendYieldMinimo; valor padrão 10,00 |

---

**Quadro {{Q}}. Dicionário de informações da classe RegraPrecoLucro.**

*RegraPrecoLucro: estratégia fundamentalista que avalia o índice Preço/Lucro; aplica-se somente a ações e abstém-se de opinar quando o ativo é um fundo imobiliário.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_precoLucroAtrativo | Índice Preço/Lucro até o qual a ação é considerada barata, gerando sinal de compra | 8 | Numérico (double) | {9}4,99 | Contínuo: maior que 0,00; valor padrão 8,00 |
| m_precoLucroElevado | Índice Preço/Lucro a partir do qual a ação é considerada cara, gerando sinal de venda | 8 | Numérico (double) | {9}4,99 | Contínuo: maior que m_precoLucroAtrativo; valor padrão 20,00 |

---

**Quadro {{Q}}. Dicionário de informações da classe MotorAnalise.**

*MotorAnalise: contexto do padrão Strategy; guarda as estratégias ativas, executa cada uma delas sobre o ativo e agrega os pareceres em uma única recomendação justificada.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_regras | Coleção das estratégias de análise registradas no motor, na ordem de execução; o motor assume a propriedade dos objetos | Variável | Objeto (RegraAnalise) | — | Discreto: instâncias de RegraCruzamentoMedias, RegraRsi, RegraDividendYield e RegraPrecoLucro correspondentes às regras ativas |

---

**Quadro {{Q}}. Dicionário de informações da classe CalculadoraIndicadores.**

*CalculadoraIndicadores: classe utilitária de cálculos técnicos sobre séries históricas ordenadas por data; todas as suas operações são estáticas, de modo que ela mantém apenas a constante de anualização.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| PREGOES_POR_ANO | Constante de classe: quantidade de pregões considerada em um ano (252), usada na anualização da volatilidade | 4 | Numérico (int, constante) | {9}3 | Contínuo: valor fixo 252 |

---

**Quadro {{Q}}. Dicionário de informações da classe Backtester.**

*Backtester: reexecuta o MotorAnalise em cada ponto da série histórica já importada, como se aquele pregão fosse o mais recente, e confere o parecer emitido contra o comportamento do preço nos pregões seguintes (RF021); é uma classe pura, que não conhece banco de dados nem interface gráfica.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_regras | Coleção das configurações de regras com que o motor é remontado a cada ponto do histórico avaliado | Variável | Objeto (RegraConfigurada) | — | Discreto: instâncias válidas da classe RegraConfigurada |
| HORIZONTE_PADRAO | Constante de classe: quantidade de pregões à frente considerada para medir o resultado de um sinal (10) | 4 | Numérico (int, constante) | {9}2 | Contínuo: valor fixo 10 |
| MINIMO_DE_PREGOES | Constante de classe: quantidade de pregões de histórico exigida antes do primeiro sinal (30), para que as médias móveis e o RSI tenham série suficiente para se formar | 4 | Numérico (int, constante) | {9}2 | Contínuo: valor fixo 30 |
| NOME_CONSOLIDADO | Constante de classe: rótulo da linha do parecer consolidado nos resultados apurados | 30 | Alfanumérico (QString, constante) | {X}30 | Discreto: valor fixo `Consolidado (média ponderada)` |

---

**Quadro {{Q}}. Dicionário de informações da classe ResultadoBacktestRegra.**

*ResultadoBacktestRegra: estrutura de dados com o desempenho histórico de uma única regra, ou do parecer consolidado, apurado pelo Backtester ao longo de todo o período avaliado.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| regra | Nome canônico da regra a que a apuração se refere, ou o rótulo do parecer consolidado | 60 | Alfanumérico (QString) | 1{X}60 | Discreto: Cruzamento de Medias Moveis; Indice de Forca Relativa; Dividend Yield; Preco sobre Lucro; Consolidado (média ponderada) |
| sinais | Quantidade de pareceres diferentes de Neutro emitidos ao longo do histórico; pareceres Neutro não são sinal e não entram na contabilidade (RN024) | 4 | Numérico (int) | {9}9 | Contínuo: maior ou igual a zero |
| acertos | Quantidade de sinais cuja direção se confirmou no horizonte avaliado: Compra com retorno futuro positivo e Venda com retorno futuro negativo (RN024) | 4 | Numérico (int) | {9}9 | Contínuo: de zero até o valor de sinais |
| somaDosRetornos | Soma dos retornos obtidos ao seguir cada sinal, expressa em fração e não em percentual; a venda contribui com o retorno de sinal invertido | 8 | Numérico (double) | −{9}9,99 a {9}9,99 | Contínuo |
| taxaDeAcerto() | Operação derivada: proporção de acertos sobre os sinais emitidos; devolve zero quando não houve sinal | 8 | Numérico (double, calculado) | 9,99 | Contínuo: 0,00 a 1,00 |
| retornoMedio() | Operação derivada: retorno médio por sinal, em fração, obtido da divisão da soma dos retornos pelo número de sinais; devolve zero quando não houve sinal | 8 | Numérico (double, calculado) | −9,99 a 9,99 | Contínuo |

---

**Quadro {{Q}}. Dicionário de informações da classe ResultadoBacktest.**

*ResultadoBacktest: estrutura de dados com o resultado completo do backtesting de um ativo, reunindo a apuração consolidada e a apuração de cada regra ativa, apresentadas no DialogoBacktest.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| ticker | Código de negociação do ativo submetido ao backtesting | 6 | Alfanumérico (QString) | 4{X}6 | Discreto: tickers cadastrados na classe Ativo |
| pregoesAvaliados | Quantidade de pregões em que houve reavaliação das regras ao longo do histórico | 4 | Numérico (int) | {9}9 | Contínuo: maior ou igual a zero |
| horizonte | Quantidade de pregões à frente usada para medir o resultado de cada sinal | 4 | Numérico (int) | {9}2 | Contínuo: 1 a 60 pregões; valor padrão 10 |
| consolidado | Apuração do parecer consolidado do motor, isto é, da média ponderada de todas as regras ativas (RN025) | Variável | Objeto (ResultadoBacktestRegra) | — | Discreto: instância válida de ResultadoBacktestRegra |
| porRegra | Coleção com uma apuração por regra ativa, na ordem em que o motor as registrou | Variável | Objeto (ResultadoBacktestRegra) | — | Discreto: instâncias válidas de ResultadoBacktestRegra |
| observacao | Motivo pelo qual o ativo não pôde ser avaliado, como histórico curto demais; permanece vazio quando a avaliação foi realizada | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando o ativo foi avaliado |

---

**Quadro {{Q}}. Dicionário de informações da classe BancoDeDados.**

*BancoDeDados: fachada única de acesso ao banco SQLite (padrão Singleton); abre a conexão, aplica as migrações versionadas e oferece controle explícito de transação às operações que precisam ser atômicas.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_conexao | Conexão aberta com o banco de dados, compartilhada por todos os repositórios | Variável | Objeto (QSqlDatabase) | — | Discreto: conexão aberta ou conexão inválida |
| m_caminhoBanco | Caminho do arquivo de banco de dados em uso | 255 | Alfanumérico (QString) | 1{X}255 | Contínuo |
| m_ultimoErro | Mensagem do último erro ocorrido na abertura, na migração ou no controle de transação | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando não há erro |
| NOME_CONEXAO | Constante de classe: nome lógico da conexão Qt utilizada pela aplicação | 20 | Alfanumérico (QString, constante) | {X}20 | Discreto: valor fixo `analisador-b3` |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioUsuario.**

*RepositorioUsuario: repositório da tabela usuario; converte objetos Usuario em linhas do banco e vice-versa, isolando o restante do sistema do SQL e utilizando sempre consultas parametrizadas.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido nas operações de gravação, remoção ou consulta de usuários | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioAtivo.**

*RepositorioAtivo: repositório da tabela ativo; grava e recupera a hierarquia Ativo, Acao e FundoImobiliario em uma única relação, usando a coluna tipo como discriminador e devolvendo a subclasse concreta correspondente.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido nas operações sobre ativos, inclusive na verificação de unicidade do ticker | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioCotacao.**

*RepositorioCotacao: repositório da tabela cotacao; grava as séries históricas em lote de forma idempotente, apoiando-se no índice de unicidade por ativo e data, e devolve os históricos ordenados por data crescente.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido na inserção em lote ou nas consultas de série histórica | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioCarteira.**

*RepositorioCarteira: repositório das tabelas carteira e posicao; por serem uma composição no domínio, toda carteira recuperada já vem com as suas posições carregadas, e a gravação de posição atualiza a existente em vez de duplicá-la.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido nas operações sobre carteiras e posições | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioAlerta.**

*RepositorioAlerta: repositório da tabela alerta; preserva no banco os textos canônicos do estado e da condição, mantendo válida a máquina de estados do domínio.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido nas operações sobre alertas | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioRecomendacao.**

*RepositorioRecomendacao: repositório da tabela recomendacao; guarda a saída do MotorAnalise e invalida as recomendações anteriores do ativo quando uma nova análise é publicada.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido na gravação, na consulta ou na invalidação de recomendações | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioRegra.**

*RepositorioRegra: repositório da tabela regra_configurada; é a fonte dos parâmetros com que o MotorAnalise monta as estratégias, permitindo ligar, desligar e reparametrizar regras sem recompilar o sistema.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido nas operações sobre as configurações de regras | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |

---

**Quadro {{Q}}. Dicionário de informações da classe RepositorioImportacao.**

*RepositorioImportacao: repositório da tabela importacao; mantém o histórico de auditoria das cargas de arquivos CSV, com o estado final e as contagens de linhas de cada execução.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
|---|---|---|---|---|---|
| m_ultimoErro | Mensagem do último erro de banco ocorrido na gravação ou na consulta do histórico de importações | 255 | Alfanumérico (QString) | {X}255 | Contínuo; vazio quando a última operação teve sucesso |
