# Modelo canônico dos documentos — Analisador B3

> **Fonte única de verdade** para os dois documentos entregáveis. Todo requisito,
> caso de uso, classe e diagrama deve referenciar exatamente os identificadores
> daqui. Alterou algo? Altere primeiro este arquivo.
>
> Extraído do sistema **implementado** (branch `teste-de-desenvolvimento`), não de
> intenções: os atributos vêm de `src/dominio/`, as regras de `src/analise/` e os
> fluxos dos serviços em `src/servicos/`.

## Atores

| Ator | Tipo (Use Case Points) | Peso | Descrição |
|---|---|---|---|
| Administrador | Complexo (interface gráfica) | 3 | Mantém ativos, importa cotações, configura regras e gerencia usuários |
| Investidor | Complexo (interface gráfica) | 3 | Consulta análises, mantém carteira e alertas |

UAW = 2 × 3 = **6**

## Requisitos funcionais e rastreabilidade RF → UC → classes

| RF | Descrição | UC | Classes participantes (implementadas) |
|---|---|---|---|
| RF001 | O usuário deve ser capaz de realizar login no sistema | UC001 | TelaLogin, ServicoAutenticacao, RepositorioUsuario, Usuario |
| RF002 | O administrador deve ser capaz de cadastrar ativos | UC002 | TelaAtivos, DialogoAtivo, ServicoAtivo, RepositorioAtivo, Acao, FundoImobiliario |
| RF003 | O administrador deve ser capaz de atualizar ativos | UC003 | TelaAtivos, DialogoAtivo, ServicoAtivo, RepositorioAtivo, Ativo |
| RF004 | O administrador deve ser capaz de remover ativos | UC004 | TelaAtivos, ServicoAtivo, RepositorioAtivo |
| RF005 | O administrador deve ser capaz de importar cotações de arquivo CSV | UC005 | TelaImportacao, ServicoImportacao, RepositorioCotacao, RepositorioImportacao, Cotacao, Importacao |
| RF006 | O administrador deve ser capaz de configurar as regras de análise | UC006 | TelaRegras, DialogoRegra, RepositorioRegra, RegraConfigurada, MotorAnalise |
| RF007 | O administrador deve ser capaz de gerenciar usuários | UC007 | TelaUsuarios, DialogoUsuario, ServicoAutenticacao, RepositorioUsuario, Usuario |
| RF008 | O investidor deve ser capaz de buscar ativos por ticker ou nome | UC008 | TelaAtivos, TelaDashboard, ServicoAtivo, RepositorioAtivo |
| RF009 | O investidor deve ser capaz de visualizar o gráfico de cotações com médias móveis | UC009 | TelaDashboard, GraficoCandlestick, CalculadoraIndicadores, RepositorioCotacao |
| RF010 | O investidor deve ser capaz de visualizar os indicadores técnicos do período | UC009 | TelaDashboard, CartaoIndicador, CalculadoraIndicadores |
| RF011 | O investidor deve ser capaz de visualizar os fundamentos e o histórico de um ativo | UC010 | TelaDetalheAtivo, GraficoLinha, ServicoAtivo, RepositorioCotacao |
| RF012 | O sistema deve gerar recomendação de compra, venda ou neutralidade para um ativo | UC011 | ServicoRecomendacao, MotorAnalise, RegraAnalise e implementações, Recomendacao |
| RF013 | O investidor deve ser capaz de visualizar as recomendações vigentes com sua justificativa | UC012 | TelaRecomendacoes, ServicoRecomendacao, RepositorioRecomendacao |
| RF014 | O investidor deve ser capaz de registrar a compra de um ativo na carteira | UC013 | TelaCarteira, DialogoOperacao, ServicoCarteira, RepositorioCarteira, Posicao |
| RF015 | O investidor deve ser capaz de registrar a venda de um ativo da carteira | UC014 | TelaCarteira, DialogoOperacao, ServicoCarteira, RepositorioCarteira, Posicao |
| RF016 | O investidor deve ser capaz de visualizar o resultado consolidado da carteira | UC015 | TelaCarteira, ServicoCarteira, RepositorioCotacao, Carteira, Posicao |
| RF017 | O investidor deve ser capaz de cadastrar alertas de preço | UC016 | TelaAlertas, DialogoAlerta, ServicoAlerta, RepositorioAlerta, Alerta |
| RF018 | O sistema deve disparar e notificar os alertas cuja condição foi satisfeita | UC017 | ServicoAlerta, ObservadorAlerta, JanelaPrincipal, Alerta |
| RF019 | O investidor deve ser capaz de arquivar alertas já disparados | UC018 | TelaAlertas, ServicoAlerta, RepositorioAlerta, Alerta |

**19 requisitos funcionais → 18 casos de uso** (RF009 e RF010 são atendidos pelo mesmo
UC009, como o exemplo do professor faz com RF014/RF017).

## Casos de uso

| UC | Nome | Ator principal | Complexidade (UCP) | Transações | Objetos/entidades |
|---|---|---|---|---|---|
| UC001 | Fazer login | Administrador / Investidor | Simples (5) | 3 | 1 |
| UC002 | Cadastrar ativo | Administrador | Médio (10) | 5 | 3 |
| UC003 | Atualizar ativo | Administrador | Médio (10) | 6 | 3 |
| UC004 | Remover ativo | Administrador | Simples (5) | 3 | 2 |
| UC005 | Importar cotações | Administrador | Complexo (15) | 8 | 12 |
| UC006 | Configurar regra de análise | Administrador | Médio (10) | 5 | 4 |
| UC007 | Gerenciar usuários | Administrador | Médio (10) | 6 | 2 |
| UC008 | Buscar ativos | Investidor | Simples (5) | 2 | 2 |
| UC009 | Visualizar painel de análise do ativo | Investidor | Complexo (15) | 8 | 11 |
| UC010 | Visualizar detalhes do ativo | Investidor | Médio (10) | 4 | 6 |
| UC011 | Gerar recomendações | Investidor / Administrador | Complexo (15) | 9 | 14 |
| UC012 | Visualizar recomendações vigentes | Investidor | Médio (10) | 5 | 6 |
| UC013 | Comprar ativo | Investidor | Médio (10) | 6 | 5 |
| UC014 | Vender ativo | Investidor | Médio (10) | 6 | 4 |
| UC015 | Visualizar resultado da carteira | Investidor | Médio (10) | 4 | 7 |
| UC016 | Cadastrar alerta | Investidor | Médio (10) | 5 | 3 |
| UC017 | Avaliar e disparar alertas | Investidor | Complexo (15) | 7 | 10 |
| UC018 | Arquivar alerta | Investidor | Simples (5) | 3 | 2 |

UUCW = 4 simples (20) + 9 médios (90) + 5 complexos (75)... **conferir na seção 2.6**:
Simples: UC001, UC004, UC008, UC018 → 4 × 5 = 20
Médios: UC002, UC003, UC006, UC007, UC010, UC012, UC013, UC014, UC015, UC016 → 10 × 10 = 100
Complexos: UC005, UC009, UC011, UC017 → 4 × 15 = 60
**UUCW = 180** · UAW = 6 · **UUCP = 186**

## Requisitos não funcionais, restrições e experiência do usuário

| Código | Descrição |
|---|---|
| RNF001 | O sistema deve funcionar sem conexão com a internet, usando as cotações já importadas |
| RNF002 | A importação de cotações deve ser atômica: um arquivo com erro estrutural não grava nenhuma linha |
| RNF003 | Reimportar o mesmo arquivo não deve duplicar cotações |
| RNF004 | As senhas devem ser armazenadas apenas como resumo criptográfico (SHA-256), nunca em texto puro |
| RNF005 | Somente o administrador deve acessar as telas de administração |
| RNF006 | Toda recomendação deve exibir a regra e os números que a justificaram |
| RNF007 | O sistema deve responder às interações da interface em menos de 2 segundos com 5 anos de histórico |
| RP001 | O sistema será implementado em C++ com paradigma orientado a objetos |
| RP002 | A interface gráfica utilizará o framework Qt 6 (Widgets e Charts) |
| RP003 | O banco de dados será relacional (SQLite) com no mínimo 5 tabelas |
| RP004 | A compilação será feita com CMake, sem dependências externas ao Qt |
| REU001 | A navegação deve exibir somente as opções permitidas ao papel do usuário |
| REU002 | Ganho e perda devem ser distinguíveis por cor e por sinal numérico |
| REU003 | Mensagens de erro devem indicar a linha e o motivo, em português |
| REU004 | O sistema deve indicar claramente que as recomendações são apoio à decisão, não aconselhamento financeiro |

## Regras de negócio (numeração global usada nos quadros de UC)

| RN | Descrição | Onde está implementada |
|---|---|---|
| RN001 | Todos os campos obrigatórios do formulário devem estar preenchidos | validações das telas e serviços |
| RN002 | O e-mail deve ser único no sistema | ServicoAutenticacao::cadastrarUsuario |
| RN003 | A senha deve ter no mínimo 6 caracteres | ServicoAutenticacao::TAMANHO_MINIMO_SENHA |
| RN004 | A senha é comparada pelo resumo SHA-256 de (sal + senha) | ServicoAutenticacao::gerarHash |
| RN005 | O ticker deve ter de 4 a 6 caracteres alfanuméricos e ser único | ServicoAtivo::tickerValido / tickerEmUso |
| RN006 | O último administrador não pode ser removido nem rebaixado | ServicoAutenticacao |
| RN007 | O arquivo de cotações deve ter o cabeçalho e as 6 colunas esperadas | ServicoImportacao::cabecalhoValido |
| RN008 | A data deve estar no formato dd/mm/aaaa e os valores no padrão decimal brasileiro | ServicoImportacao::converterLinha |
| RN009 | Em cada cotação, mínima ≤ abertura e fechamento ≤ máxima, e todos os preços > 0 | ServicoImportacao::converterLinha |
| RN010 | Cotação de mesma data e mesmo ativo é ignorada, sem duplicar | índice UNIQUE cotacao(ativo_id, data) |
| RN011 | Qualquer erro estrutural rejeita o arquivo inteiro | transação em ServicoImportacao::importarCsv |
| RN012 | A quantidade vendida não pode exceder a posição existente | ServicoCarteira::vender |
| RN013 | A compra de ativo já existente recalcula o preço médio ponderado | Posicao::incorporarCompra |
| RN014 | Posição zerada é removida da carteira | ServicoCarteira::vender |
| RN015 | A pontuação de uma recomendação varia de −1 a +1 | Recomendacao::definirPontuacao |
| RN016 | Pontuação ≥ +0,25 é Compra; ≤ −0,25 é Venda; entre elas, Neutro | Recomendacao::LIMIAR_DECISAO |
| RN017 | Ao publicar nova recomendação, a anterior do mesmo ativo é invalidada | ServicoRecomendacao::gerarParaAtivo |
| RN018 | Somente regras ativas participam da análise | RepositorioRegra::listarAtivas |
| RN019 | Uma regra sem dados suficientes se abstém, sem pontuar | RegraAnalise::avaliar devolve std::nullopt |
| RN020 | O alerta só dispara quando a condição é satisfeita pela última cotação | ServicoAlerta::avaliarAlertas |
| RN021 | Somente alerta já disparado pode ser arquivado | Alerta::arquivar |
| RN022 | O valor de referência do alerta deve ser maior que zero | ServicoAlerta::criarAlerta |

## Máquinas de estado (base dos Diagramas de Estados do 2º bimestre)

| Classe | Estados | Transições |
|---|---|---|
| Alerta | Ativo, Disparado, Arquivado | Ativo →`disparar()`→ Disparado →`arquivar()`→ Arquivado |
| Recomendacao | Gerada, Vigente, Expirada, Invalidada | Gerada →`marcarComoVigente()`→ Vigente →`expirar()`→ Expirada; Gerada\|Vigente →`invalidar()`→ Invalidada |
| Importacao | Pendente, Validando, Concluida, Rejeitada | Pendente →`iniciarValidacao()`→ Validando →`concluir()`→ Concluida; Pendente\|Validando →`rejeitar()`→ Rejeitada |

## Classes de domínio implementadas (atributos reais)

| Classe | Atributos |
|---|---|
| Usuario | id, nome, email, senhaHash, papel |
| Ativo *(abstrata)* | id, ticker, nomeEmpresa, setor |
| Acao *(herda Ativo)* | precoLucro, dividendYield, valorMercado |
| FundoImobiliario *(herda Ativo)* | dividendYield, valorPatrimonialPorCota, segmento |
| Cotacao | id, ativoId, data, abertura, maxima, minima, fechamento, volume |
| Carteira | id, usuarioId, nome, criadaEm, posicoes |
| Posicao | id, carteiraId, ativoId, quantidade, precoMedio, compradaEm |
| Alerta | id, usuarioId, ativoId, condicao, valorReferencia, estado, criadoEm, disparadoEm |
| Recomendacao | id, ativoId, tipo, justificativa, regraAplicada, geradaEm, estado, pontuacao |
| RegraConfigurada | id, nomeRegra, ativa, parametroPrincipal, parametroSecundario |
| Importacao | id, ativoId, arquivo, estado, linhasLidas, linhasInseridas, mensagemErro, executadaEm |
| Indicador *(struct)* | nome, data, valor |

Total de atributos das classes de domínio: **62** (a meta observada nos trabalhos de
veteranos é 30; o dicionário completo inclui ainda serviços, repositórios e telas).

## Camadas e padrões de projeto (para o capítulo de modelagem)

| Padrão | Participantes |
|---|---|
| Strategy | RegraAnalise (interface); RegraCruzamentoMedias, RegraRsi, RegraDividendYield, RegraPrecoLucro; MotorAnalise (contexto) |
| Observer | ObservadorAlerta (interface), ServicoAlerta (sujeito), JanelaPrincipal (observador concreto) |
| Repository | RepositorioUsuario, RepositorioAtivo, RepositorioCotacao, RepositorioCarteira, RepositorioAlerta, RepositorioRecomendacao, RepositorioRegra, RepositorioImportacao |
| Singleton | BancoDeDados |
| Herança e polimorfismo | Ativo → Acao, FundoImobiliario (métodos `tipo()` e `indiceValor()`) |

## Cliente da entrevista

🟦 **[VALIDAR c/ equipe]** Assumido para o documento: **Clube de Investimentos
Neoville**, grupo de estudos de investimentos formado por alunos e servidores da
UTFPR Campus Curitiba, representado pelo coordenador **Prof. Marcos R. Andrade**.
O clube reúne cerca de 40 participantes que hoje acompanham seus ativos em planilhas
individuais.

Esse é o mesmo arranjo do exemplo do professor (MUTEC/MUTEQ, com o responsável pelo
museu como cliente) e do trabalho dos veteranos (a própria UTFPR como cliente). Se a
equipe entrevistar um investidor real, basta substituir as respostas da seção 2.1–2.4
do documento — o restante não muda.

## Tabelas do banco (base do DER do 2º bimestre)

usuario, ativo, cotacao, carteira, posicao, alerta, recomendacao, regra_configurada,
importacao (9 tabelas; o professor exige no mínimo 5).
