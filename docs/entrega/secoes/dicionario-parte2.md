**Quadro {{Q}}. Dicionário de informações da classe ServicoAutenticacao.**

*Concentra as regras de negócio de autenticação e de gestão de usuários: valida credenciais, aplica o resumo criptográfico SHA-256 sobre o sal concatenado à senha (RN004), garante a unicidade do e-mail (RN002), o tamanho mínimo da senha (RN003) e impede a remoção ou o rebaixamento do último administrador (RN006).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| SAL | Constante de classe com o sal fixo da aplicação, concatenado à senha antes do cálculo do resumo criptográfico. Valor `aps-b3-2026` | 11 | QString | Alfanumérico | N/A |
| TAMANHO_MINIMO_SENHA | Constante de classe com o número mínimo de caracteres aceito para a senha (RN003). Valor 6 | 4 | int | Numérico | Discreto |
| m_repositorio | Referência ao repositório de usuários, injetada pelo Contexto; é por ela que o serviço consulta e grava os dados persistidos | N/A | RepositorioUsuario& | Objeto | N/A |
| m_ultimoErro | Mensagem do último erro de validação ou de persistência, recuperada pela interface gráfica por meio de `ultimoErro()` | 255 | QString | Alfanumérico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe ServicoAtivo.**

*Concentra as regras de negócio do cadastro de ativos, validando o formato e a unicidade do ticker (RN005) e os campos obrigatórios (RN001); devolve os ativos como ponteiros para a classe base Ativo, preservando o polimorfismo entre Acao e FundoImobiliario.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_repositorio | Referência ao repositório de ativos, injetada pelo Contexto; realiza a leitura e a gravação dos ativos no banco de dados | N/A | RepositorioAtivo& | Objeto | N/A |
| m_ultimoErro | Mensagem do último erro de validação ou de persistência, exibida pela tela que solicitou a operação | 255 | QString | Alfanumérico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe ServicoImportacao.**

*Responsável pela carga de cotações a partir de arquivos CSV no padrão brasileiro. A operação é transacional: o arquivo inteiro é lido e validado em memória antes de qualquer escrita, de modo que um único erro estrutural rejeita todo o arquivo (RN011); cotações de mesma data e mesmo ativo são ignoradas, tornando a importação idempotente (RN010).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| COLUNAS_ESPERADAS | Constante de classe com o número de colunas exigidas no arquivo (Data, Abertura, Máxima, Mínima, Fechamento e Volume), conforme RN007. Valor 6 | 4 | int | Numérico | Discreto |
| SEPARADOR | Constante de classe com o caractere separador de campos do arquivo exportado pela B3. Valor ponto e vírgula | 2 | QChar | Caractere | N/A |
| m_repositorioCotacao | Referência ao repositório de cotações, destino das linhas validadas | N/A | RepositorioCotacao& | Objeto | N/A |
| m_repositorioAtivo | Referência ao repositório de ativos, usada para casar o nome do arquivo com o ticker cadastrado na importação de diretório | N/A | RepositorioAtivo& | Objeto | N/A |
| m_repositorioImportacao | Referência ao repositório de importações, onde é registrado o histórico de cada execução com o respectivo estado | N/A | RepositorioImportacao& | Objeto | N/A |
| m_ultimoErro | Mensagem do último erro ocorrido na importação | 255 | QString | Alfanumérico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe ResultadoImportacao.**

*Estrutura de dados devolvida pelo ServicoImportacao ao término de cada carga; reúne o desfecho da operação e os números que alimentam o relatório apresentado na TelaImportacao.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| sucesso | Indica se o arquivo foi aceito e gravado; falso quando houve erro estrutural e nada foi persistido | 1 | bool | Lógico | Discreto |
| linhasLidas | Quantidade de linhas de dados lidas do arquivo, excluído o cabeçalho | 4 | int | Numérico | Discreto |
| linhasInseridas | Quantidade de cotações efetivamente gravadas no banco de dados | 4 | int | Numérico | Discreto |
| linhasIgnoradas | Quantidade de cotações descartadas por já existirem para o mesmo ativo e a mesma data (RN010) | 4 | int | Numérico | Discreto |
| erros | Lista das mensagens de erro, cada uma indicando o número da linha e o motivo da recusa (REU003) | N/A | QStringList | Lista de texto | N/A |

**Quadro {{Q}}. Dicionário de informações da classe ServicoCarteira.**

*Concentra as regras de negócio da carteira de investimentos: a compra recalcula o preço médio ponderado da posição (RN013), a venda nunca excede a quantidade existente (RN012) e a posição zerada é removida (RN014). O resumo consolidado cruza as posições com a última cotação de cada ativo para apurar o lucro ou o prejuízo.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_repositorioCarteira | Referência ao repositório de carteiras e posições, injetada pelo Contexto | N/A | RepositorioCarteira& | Objeto | N/A |
| m_repositorioCotacao | Referência ao repositório de cotações, usada para obter o último fechamento de cada ativo no cálculo do resumo | N/A | RepositorioCotacao& | Objeto | N/A |
| m_repositorioAtivo | Referência ao repositório de ativos, usada para obter o ticker e o nome exibidos em cada linha do resumo | N/A | RepositorioAtivo& | Objeto | N/A |
| m_ultimoErro | Mensagem do último erro de validação, exibida pela TelaCarteira | 255 | QString | Alfanumérico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe ResumoCarteira.**

*Estrutura de dados com o resultado consolidado da carteira, calculada por ServicoCarteira::calcularResumo e apresentada no rodapé da TelaCarteira (RF016).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| valorInvestido | Soma do custo de aquisição de todas as posições, isto é, quantidade multiplicada pelo preço médio | 8 | double | Decimal (R$) | Contínuo |
| valorAtual | Soma do valor de mercado de todas as posições, avaliado pela última cotação de cada ativo | 8 | double | Decimal (R$) | Contínuo |
| lucroPrejuizo | Diferença entre o valor atual e o valor investido; positivo indica lucro e negativo indica prejuízo (REU002) | 8 | double | Decimal (R$) | Contínuo |
| rentabilidadePercentual | Relação percentual entre o lucro ou prejuízo e o valor investido | 8 | double | Decimal (%) | Contínuo |
| linhas | Coleção das linhas detalhadas do resumo, uma por posição da carteira | N/A | QVector\<LinhaResumoCarteira\> | Lista de objetos | N/A |

**Quadro {{Q}}. Dicionário de informações da classe LinhaResumoCarteira.**

*Estrutura de dados que representa uma linha do resumo da carteira: reúne os dados da posição e os valores calculados a partir da última cotação do ativo.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| ativoId | Identificador do ativo a que a posição se refere | 8 | qint64 | Numérico | Discreto |
| ticker | Código de negociação do ativo, exibido na primeira coluna da tabela | 6 | QString | Alfanumérico | Discreto |
| nomeAtivo | Nome da empresa ou do fundo correspondente ao ativo | 120 | QString | Alfanumérico | Contínuo |
| quantidade | Quantidade de cotas ou ações mantidas na posição | 4 | int | Numérico | Discreto |
| precoMedio | Preço médio ponderado de aquisição da posição (RN013) | 8 | double | Decimal (R$) | Contínuo |
| precoAtual | Preço de fechamento da última cotação importada para o ativo | 8 | double | Decimal (R$) | Contínuo |
| valorInvestido | Custo de aquisição da posição: quantidade multiplicada pelo preço médio | 8 | double | Decimal (R$) | Contínuo |
| valorAtual | Valor de mercado da posição: quantidade multiplicada pelo preço atual | 8 | double | Decimal (R$) | Contínuo |
| lucroPrejuizo | Diferença entre o valor atual e o valor investido da posição | 8 | double | Decimal (R$) | Contínuo |
| rentabilidadePercentual | Relação percentual entre o lucro ou prejuízo e o valor investido da posição | 8 | double | Decimal (%) | Contínuo |
| temCotacao | Indica se existe cotação importada para o ativo; quando falso, os valores de mercado não são apurados | 1 | bool | Lógico | Discreto |

**Quadro {{Q}}. Dicionário de informações da classe ServicoAlerta.**

*Responsável pelo monitoramento dos alertas de preço e sujeito do padrão Observer: compara cada alerta monitorado com a última cotação do ativo, dispara os que satisfazem a condição (RN020) e notifica os observadores registrados, sem conhecer a janela concreta que exibirá a mensagem.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_repositorioAlerta | Referência ao repositório de alertas, injetada pelo Contexto | N/A | RepositorioAlerta& | Objeto | N/A |
| m_repositorioCotacao | Referência ao repositório de cotações, usada para obter a última cotação de cada ativo monitorado | N/A | RepositorioCotacao& | Objeto | N/A |
| m_repositorioAtivo | Referência ao repositório de ativos, usada para identificar o ativo na notificação enviada ao observador | N/A | RepositorioAtivo& | Objeto | N/A |
| m_observadores | Coleção dos observadores registrados que serão notificados a cada disparo de alerta (padrão Observer) | N/A | std::vector\<ObservadorAlerta\*\> | Lista de ponteiros | N/A |
| m_ultimoErro | Mensagem do último erro de validação, como o valor de referência não positivo (RN022) | 255 | QString | Alfanumérico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe ObservadorAlerta.**

*Interface abstrata do padrão Observer. Não possui atributos: declara apenas a operação de notificação que as classes interessadas em receber os disparos de alerta devem implementar — no sistema, a JanelaPrincipal.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| alertaDisparado(alerta, ativo, valorObservado) | Método virtual puro chamado uma vez por alerta disparado, recebendo o alerta, o ativo correspondente e o valor que satisfez a condição | N/A | void | Método | N/A |
| ~ObservadorAlerta() | Destrutor virtual, necessário para a destruição correta dos observadores concretos por meio da interface | N/A | virtual | Método | N/A |

**Quadro {{Q}}. Dicionário de informações da classe ServicoRecomendacao.**

*Orquestra o motor de análise: monta o MotorAnalise com as regras ativas do banco (RN018), gera a recomendação do ativo e administra o seu ciclo de vida, publicando a nova recomendação como vigente e invalidando a anterior do mesmo ativo (RN017).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| COTACOES_MINIMAS | Constante de classe com a quantidade mínima de cotações necessária para que o motor consiga emitir parecer técnico. Valor 2 | 4 | int | Numérico | Discreto |
| m_repositorioRecomendacao | Referência ao repositório de recomendações, onde as recomendações são persistidas e consultadas | N/A | RepositorioRecomendacao& | Objeto | N/A |
| m_repositorioCotacao | Referência ao repositório de cotações, fonte da série histórica submetida às regras | N/A | RepositorioCotacao& | Objeto | N/A |
| m_repositorioAtivo | Referência ao repositório de ativos, usada para recuperar os fundamentos avaliados pelas regras e para gerar recomendações para todos os ativos | N/A | RepositorioAtivo& | Objeto | N/A |
| m_repositorioRegra | Referência ao repositório de regras configuradas, de onde são lidas as regras ativas e os seus parâmetros | N/A | RepositorioRegra& | Objeto | N/A |
| m_ultimoErro | Mensagem do último erro ocorrido na geração da recomendação | 255 | QString | Alfanumérico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe Contexto.**

*Contexto da aplicação: é o proprietário dos repositórios e dos serviços e torna explícita a injeção de dependência entre as camadas, pois as telas recebem uma referência ao Contexto e nunca criam repositórios. Os repositórios são declarados antes dos serviços para garantir a ordem correta de inicialização e destruição.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| usuarioLogado | Usuário autenticado na sessão corrente; o seu papel determina as telas visíveis na navegação (REU001, RNF005) | N/A | Usuario | Objeto | N/A |
| repositorioUsuario | Repositório de acesso à tabela de usuários | N/A | RepositorioUsuario | Objeto | N/A |
| repositorioAtivo | Repositório de acesso à tabela de ativos, com a construção polimórfica de Acao e FundoImobiliario | N/A | RepositorioAtivo | Objeto | N/A |
| repositorioCotacao | Repositório de acesso à tabela de cotações | N/A | RepositorioCotacao | Objeto | N/A |
| repositorioCarteira | Repositório de acesso às tabelas de carteira e de posição | N/A | RepositorioCarteira | Objeto | N/A |
| repositorioAlerta | Repositório de acesso à tabela de alertas | N/A | RepositorioAlerta | Objeto | N/A |
| repositorioRecomendacao | Repositório de acesso à tabela de recomendações | N/A | RepositorioRecomendacao | Objeto | N/A |
| repositorioRegra | Repositório de acesso à tabela de regras configuradas | N/A | RepositorioRegra | Objeto | N/A |
| repositorioImportacao | Repositório de acesso à tabela de importações | N/A | RepositorioImportacao | Objeto | N/A |
| servicoAutenticacao | Serviço de autenticação e de gestão de usuários | N/A | ServicoAutenticacao | Objeto | N/A |
| servicoAtivo | Serviço de manutenção do cadastro de ativos | N/A | ServicoAtivo | Objeto | N/A |
| servicoImportacao | Serviço de importação de cotações a partir de arquivos CSV | N/A | ServicoImportacao | Objeto | N/A |
| servicoCarteira | Serviço de operações e de consolidação da carteira | N/A | ServicoCarteira | Objeto | N/A |
| servicoAlerta | Serviço de monitoramento e disparo de alertas | N/A | ServicoAlerta | Objeto | N/A |
| servicoRecomendacao | Serviço de geração e publicação de recomendações | N/A | ServicoRecomendacao | Objeto | N/A |

**Quadro {{Q}}. Dicionário de informações da classe JanelaPrincipal.**

*Janela principal do sistema, composta por uma navegação lateral e por uma pilha de páginas com as telas; os itens exibidos dependem do papel do usuário autenticado (REU001, RNF005). Implementa a interface ObservadorAlerta e, por isso, é o observador concreto do padrão Observer: recebe do ServicoAlerta cada disparo e o apresenta na barra de estado (RF018).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação, repassada a todas as telas construídas pela janela | N/A | Contexto& | Objeto | N/A |
| m_navegacao | Lista da navegação lateral, com um item por tela permitida ao papel do usuário | N/A | Objeto (QListWidget\*) | Componente gráfico | N/A |
| m_paginas | Pilha de páginas que exibe uma tela por vez, sincronizada com o item selecionado na navegação | N/A | Objeto (QStackedWidget\*) | Componente gráfico | N/A |
| m_rotuloUsuario | Rótulo com o nome e o papel do usuário autenticado, exibido no cabeçalho | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_telaDashboard | Página do painel de análise do ativo (UC009) | N/A | Objeto (TelaDashboard\*) | Componente gráfico | N/A |
| m_telaDetalhe | Página de detalhe do ativo (UC010) | N/A | Objeto (TelaDetalheAtivo\*) | Componente gráfico | N/A |
| m_telaRecomendacoes | Página das recomendações vigentes (UC012) | N/A | Objeto (TelaRecomendacoes\*) | Componente gráfico | N/A |
| m_telaCarteira | Página da carteira do investidor (UC013, UC014 e UC015) | N/A | Objeto (TelaCarteira\*) | Componente gráfico | N/A |
| m_telaAlertas | Página de alertas de preço (UC016, UC017 e UC018) | N/A | Objeto (TelaAlertas\*) | Componente gráfico | N/A |
| m_telaAtivos | Página de manutenção do cadastro de ativos, exclusiva do administrador (UC002 a UC004 e UC008) | N/A | Objeto (TelaAtivos\*) | Componente gráfico | N/A |
| m_telaImportacao | Página de importação de cotações, exclusiva do administrador (UC005) | N/A | Objeto (TelaImportacao\*) | Componente gráfico | N/A |
| m_telaRegras | Página de configuração das regras de análise, exclusiva do administrador (UC006) | N/A | Objeto (TelaRegras\*) | Componente gráfico | N/A |
| m_telaUsuarios | Página de gestão de usuários, exclusiva do administrador (UC007) | N/A | Objeto (TelaUsuarios\*) | Componente gráfico | N/A |
| m_desejaTrocarUsuario | Indica que a janela foi fechada pela ação "Trocar usuário", para que o programa principal reabra a tela de login | 1 | bool | Lógico | Discreto |

**Quadro {{Q}}. Dicionário de informações da classe TelaLogin.**

*Tela de autenticação, apresentada como diálogo modal antes da janela principal (UC001); se o usuário cancelar, a aplicação encerra sem abrir o sistema. A mensagem de erro não revela qual dos dois campos falhou.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_servico | Referência ao serviço de autenticação, responsável por validar as credenciais informadas | N/A | ServicoAutenticacao& | Objeto | N/A |
| m_campoEmail | Campo de entrada do endereço de e-mail do usuário | N/A | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_campoSenha | Campo de entrada da senha, com o conteúdo mascarado na digitação | N/A | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_botaoEntrar | Botão que aciona a tentativa de autenticação | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_rotuloErro | Rótulo que exibe a mensagem de credenciais inválidas | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_usuarioAutenticado | Usuário devolvido pelo serviço após a autenticação bem-sucedida; válido somente depois do aceite do diálogo | N/A | Usuario | Objeto | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaDashboard.**

*Painel principal do investidor (UC009): apresenta o gráfico de candles com as médias móveis sobrepostas (RF009), os cartões com os indicadores técnicos do período (RF010) e a recomendação vigente do ativo com a justificativa completa (RNF006).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| PERIODOS | Constante de classe com os períodos de histórico oferecidos no seletor, em dias. Valores 30, 90, 180, 365 e 0, sendo 0 a série inteira | 20 | int[] | Vetor numérico | Discreto |
| QUANTIDADE_PERIODOS | Constante de classe com a quantidade de opções do seletor de período. Valor 5 | 4 | int | Numérico | Discreto |
| m_contexto | Referência ao contexto da aplicação, por onde a tela alcança os serviços e os repositórios | N/A | Contexto& | Objeto | N/A |
| m_campoAtivo | Seletor do ativo em exibição, recarregado a cada entrada na tela com preservação da seleção | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_campoPeriodo | Seletor do período de histórico apresentado no gráfico e usado no cálculo dos indicadores | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_botaoAnalisar | Botão que solicita ao ServicoRecomendacao a geração de nova recomendação para o ativo em exibição | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_botaoDetalhar | Botão que solicita a abertura da tela de detalhe do ativo em exibição | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_grafico | Gráfico de candles com as médias móveis curta e longa sobrepostas | N/A | Objeto (GraficoCandlestick\*) | Componente gráfico | N/A |
| m_cartaoFechamento | Cartão com o último preço de fechamento do período | N/A | Objeto (CartaoIndicador\*) | Componente gráfico | N/A |
| m_cartaoVariacao | Cartão com a variação do período, colorida conforme o sinal (REU002) | N/A | Objeto (CartaoIndicador\*) | Componente gráfico | N/A |
| m_cartaoRsi | Cartão com o índice de força relativa calculado para o período | N/A | Objeto (CartaoIndicador\*) | Componente gráfico | N/A |
| m_cartaoVolatilidade | Cartão com a volatilidade apurada no período | N/A | Objeto (CartaoIndicador\*) | Componente gráfico | N/A |
| m_selo | Rótulo em destaque com o tipo da recomendação vigente: Compra, Venda ou Neutro (RN016) | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_justificativa | Área de texto com a justificativa da recomendação, incluindo a regra aplicada e os números que a sustentam (RNF006) | N/A | Objeto (QTextBrowser\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaDetalheAtivo.**

*Tela de detalhe do ativo (UC010): exibe os fundamentos cadastrados, o gráfico de linha do preço de fechamento, a tabela com as cotações mais recentes e o parecer de cada regra de análise, detalhando o padrão Strategy regra por regra (RF011).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| LINHAS_TABELA | Constante de classe com a quantidade de cotações apresentadas na tabela, das mais recentes para as mais antigas. Valor 60 | 4 | int | Numérico | Discreto |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_campoAtivo | Seletor do ativo detalhado | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_rotuloFundamentos | Rótulo com os fundamentos do ativo, variáveis conforme se trate de ação ou de fundo imobiliário | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_grafico | Gráfico de linha do preço de fechamento do ativo | N/A | Objeto (GraficoLinha\*) | Componente gráfico | N/A |
| m_tabelaCotacoes | Tabela com data, abertura, máxima, mínima, fechamento e volume das cotações importadas | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |
| m_pareceres | Área de texto com o parecer individual de cada regra de análise aplicada ao ativo | N/A | Objeto (QTextBrowser\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaRecomendacoes.**

*Tela das recomendações vigentes (UC012): apresenta a lista consolidada por ativo e, no painel lateral, o detalhamento do parecer de cada regra do padrão Strategy, atendendo à exigência de que toda recomendação exiba a regra e os números que a justificaram (RNF006).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_tabela | Tabela das recomendações vigentes, com ticker, tipo, pontuação, regra aplicada e data de geração | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |
| m_detalhes | Área de texto com o detalhamento regra a regra da recomendação selecionada | N/A | Objeto (QTextBrowser\*) | Componente gráfico | N/A |
| m_rotuloMensagem | Rótulo de retorno das operações, informando êxito ou erro da geração das recomendações | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_botaoAbrir | Botão que solicita a abertura do painel de análise no ativo selecionado | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaCarteira.**

*Tela da carteira do investidor (UC013, UC014 e UC015): lista as posições com preço médio, preço atual e lucro ou prejuízo destacado por cor e por sinal (REU002), e apresenta o resumo consolidado no rodapé.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_carteiraId | Identificador da carteira do usuário autenticado, obtido ou criado na primeira entrada na tela | 8 | qint64 | Numérico | Discreto |
| m_tabela | Tabela das posições, uma linha por ativo em carteira | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |
| m_rotuloResumo | Rótulo do rodapé com valor investido, valor atual, lucro ou prejuízo e rentabilidade percentual | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloMensagem | Rótulo de retorno das operações de compra e de venda | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_botaoVender | Botão de venda, habilitado somente quando há uma posição selecionada | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaAlertas.**

*Tela de alertas do investidor (UC016, UC017 e UC018): permite cadastrar, remover e arquivar alertas e acionar a avaliação, que percorre os alertas monitorados e dispara os que atingiram a condição; as notificações chegam à JanelaPrincipal pelo padrão Observer.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_tabela | Tabela dos alertas do usuário, com ativo, condição, valor de referência, estado e datas | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |
| m_botaoArquivar | Botão de arquivamento, aplicável somente a alerta já disparado (RN021) | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_botaoRemover | Botão de exclusão do alerta selecionado | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_rotuloMensagem | Rótulo de retorno das operações, com o número de alertas disparados na avaliação | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaAtivos.**

*Tela de manutenção do cadastro de ativos, exclusiva do administrador (UC002, UC003, UC004 e UC008): lista os ativos com os seus fundamentos e a quantidade de cotações importadas, permitindo cadastrar, editar, remover e filtrar por ticker ou por nome.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_campoBusca | Campo de filtro por ticker ou por parte do nome da empresa (RF008) | N/A | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_tabela | Tabela dos ativos cadastrados, com tipo, ticker, nome, setor, fundamentos e número de cotações | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |
| m_botaoEditar | Botão de edição do ativo selecionado | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_botaoRemover | Botão de remoção do ativo selecionado | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_rotuloMensagem | Rótulo de retorno das operações de cadastro, edição e remoção | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaImportacao.**

*Tela de importação de cotações, exclusiva do administrador (UC005): permite importar um arquivo CSV para um ativo específico ou um diretório inteiro, casando o nome TICKER.csv com o cadastro, e exibe o relatório completo da operação e o histórico das importações anteriores.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_campoAtivo | Seletor do ativo de destino das cotações do arquivo | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_campoArquivo | Campo com o caminho do arquivo CSV escolhido | 255 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_botaoImportar | Botão que aciona a importação do arquivo selecionado | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_rotuloResumo | Rótulo com o resumo da operação: linhas lidas, inseridas e ignoradas | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_areaErros | Área de texto com a relação dos erros, indicando a linha e o motivo de cada recusa (REU003) | N/A | Objeto (QPlainTextEdit\*) | Componente gráfico | N/A |
| m_tabelaHistorico | Tabela do histórico das importações, com arquivo, estado, contagens e data de execução | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaRegras.**

*Tela de configuração das regras de análise, exclusiva do administrador (UC006): permite ativar, desativar e reparametrizar as estratégias sem recompilar o sistema, uma vez que o MotorAnalise lê essas configurações a cada análise (RN018).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_tabela | Tabela das regras configuradas, com nome, situação de ativação e parâmetros | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |
| m_botaoEditar | Botão de edição dos parâmetros da regra selecionada | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_botaoAlternar | Botão que ativa ou desativa a regra selecionada | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_rotuloMensagem | Rótulo de retorno das operações de configuração | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe TelaUsuarios.**

*Tela de gestão de usuários, exclusiva do administrador (UC007): apresenta os usuários cadastrados e aciona o cadastro, a edição e a remoção; as regras de negócio de unicidade do e-mail, tamanho mínimo da senha e proteção do último administrador permanecem no ServicoAutenticacao.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_contexto | Referência ao contexto da aplicação | N/A | Contexto& | Objeto | N/A |
| m_tabela | Tabela dos usuários cadastrados, com nome, e-mail e papel | N/A | Objeto (QTableWidget\*) | Componente gráfico | N/A |
| m_botaoEditar | Botão de edição do usuário selecionado | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_botaoRemover | Botão de remoção do usuário selecionado, sujeito à proteção do último administrador (RN006) | N/A | Objeto (QPushButton\*) | Componente gráfico | N/A |
| m_rotuloMensagem | Rótulo de retorno das operações de cadastro, edição e remoção | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe DialogoAtivo.**

*Diálogo de cadastro e de edição de ativos: o tipo escolhido determina quais campos de fundamentos ficam habilitados e qual subclasse concreta de Ativo é construída, evidenciando o polimorfismo entre Acao e FundoImobiliario.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| INDICE_ACAO | Constante de classe com o índice da opção "Ação" no seletor de tipo. Valor 0 | 4 | int | Numérico | Discreto |
| INDICE_FUNDO | Constante de classe com o índice da opção "Fundo Imobiliário" no seletor de tipo. Valor 1 | 4 | int | Numérico | Discreto |
| m_campoTipo | Seletor do tipo do ativo, que determina a subclasse construída | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_campoTicker | Campo do código de negociação, de 4 a 6 caracteres alfanuméricos (RN005) | 6 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_campoNome | Campo do nome da empresa ou do fundo | 120 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_campoSetor | Campo do setor de atuação do ativo | 60 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_campoPrecoLucro | Campo do índice preço/lucro, habilitado apenas para ações | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_campoDividendYield | Campo do rendimento de dividendos em percentual, comum aos dois tipos | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_campoValorMercado | Campo do valor de mercado, habilitado apenas para ações | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_campoValorPatrimonial | Campo do valor patrimonial por cota, habilitado apenas para fundos imobiliários | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_campoSegmento | Campo do segmento do fundo imobiliário, habilitado apenas para esse tipo | 60 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_rotuloPrecoLucro | Rótulo do campo de preço/lucro, oculto quando o tipo escolhido é fundo imobiliário | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloValorMercado | Rótulo do campo de valor de mercado, oculto quando o tipo escolhido é fundo imobiliário | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloValorPatrimonial | Rótulo do campo de valor patrimonial por cota, oculto quando o tipo escolhido é ação | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloSegmento | Rótulo do campo de segmento, oculto quando o tipo escolhido é ação | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_idEmEdicao | Identificador do ativo em edição, preservado na construção do objeto; vale zero no cadastro de ativo novo | 8 | qint64 | Numérico | Discreto |

**Quadro {{Q}}. Dicionário de informações da classe DialogoOperacao.**

*Diálogo de compra e de venda de ativos na carteira: o mesmo formulário atende às duas operações; no modo Compra solicita preço e data, e no modo Venda limita a quantidade à posição existente e dispensa o preço, pois a venda apenas reduz a posição registrada (RN012).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_modo | Modo de operação do diálogo: Compra ou Venda | 4 | Modo (enumeração) | Enumerado | Discreto |
| m_opcoes | Coleção dos ativos oferecidos no diálogo, com os limites aplicáveis a cada operação | N/A | QVector\<OpcaoOperacao\> | Lista de objetos | N/A |
| m_campoAtivo | Seletor do ativo objeto da operação | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_campoQuantidade | Campo da quantidade negociada, limitado à posição existente no modo Venda | N/A | Objeto (QSpinBox\*) | Componente gráfico | N/A |
| m_campoPreco | Campo do preço unitário de compra, sugerido a partir da última cotação | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_campoData | Campo da data da compra | N/A | Objeto (QDateEdit\*) | Componente gráfico | N/A |
| m_rotuloPreco | Rótulo do campo de preço, oculto no modo Venda | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloData | Rótulo do campo de data, oculto no modo Venda | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloLimite | Rótulo informativo com a quantidade máxima disponível para venda do ativo escolhido | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe OpcaoOperacao.**

*Estrutura de dados que descreve um ativo oferecido no DialogoOperacao, com o preço sugerido e o limite de quantidade aplicáveis à operação.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| ativoId | Identificador do ativo correspondente à opção | 8 | qint64 | Numérico | Discreto |
| rotulo | Texto apresentado no seletor, formado pelo ticker e pelo nome do ativo | 130 | QString | Alfanumérico | Contínuo |
| precoSugerido | Preço proposto para a compra, obtido da última cotação importada do ativo | 8 | double | Decimal (R$) | Contínuo |
| quantidadeMaxima | Quantidade máxima passível de venda, igual à posição existente na carteira | 4 | int | Numérico | Discreto |

**Quadro {{Q}}. Dicionário de informações da classe DialogoAlerta.**

*Diálogo de cadastro de alerta de preço: reúne o ativo monitorado, a condição de disparo e o valor de referência, que deve ser maior que zero (RN022). As condições oferecidas correspondem aos valores da enumeração TipoCondicaoAlerta do domínio.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_opcoes | Coleção dos ativos oferecidos no diálogo, com o último fechamento de cada um | N/A | QVector\<OpcaoAlerta\> | Lista de objetos | N/A |
| m_campoAtivo | Seletor do ativo a ser monitorado pelo alerta | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_campoCondicao | Seletor da condição de disparo: preço acima de, preço abaixo de ou variação diária acima de | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_campoValor | Campo do valor de referência comparado à última cotação do ativo (RN020) | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_rotuloAjuda | Rótulo auxiliar com a sugestão baseada no último fechamento do ativo escolhido | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe OpcaoAlerta.**

*Estrutura de dados que descreve um ativo oferecido no DialogoAlerta, com o último fechamento usado como valor sugerido para a condição.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| ativoId | Identificador do ativo correspondente à opção | 8 | qint64 | Numérico | Discreto |
| rotulo | Texto apresentado no seletor, formado pelo ticker e pelo nome do ativo | 130 | QString | Alfanumérico | Contínuo |
| ultimoFechamento | Preço de fechamento da última cotação importada, proposto como valor de referência | 8 | double | Decimal (R$) | Contínuo |

**Quadro {{Q}}. Dicionário de informações da classe DialogoRegra.**

*Diálogo de configuração de uma regra de análise do padrão Strategy. O nome da regra não é editável, pois é a chave que o MotorAnalise utiliza para instanciar a estratégia concreta; são ajustáveis apenas os parâmetros e a situação de ativação.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_regra | Cópia da regra recebida, base do objeto devolvido com os valores informados no formulário | N/A | RegraConfigurada | Objeto | N/A |
| m_campoAtiva | Caixa de seleção que define se a regra participa das análises (RN018) | N/A | Objeto (QCheckBox\*) | Componente gráfico | N/A |
| m_campoPrincipal | Campo do primeiro parâmetro da regra, com duas casas decimais e faixa de 0,00 a 100.000,00 | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_campoSecundario | Campo do segundo parâmetro da regra, com duas casas decimais e faixa de 0,00 a 100.000,00 | N/A | Objeto (QDoubleSpinBox\*) | Componente gráfico | N/A |
| m_rotuloDescricao | Rótulo explicativo do significado dos parâmetros da regra em configuração | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe DialogoUsuario.**

*Diálogo de cadastro e de edição de usuários, exclusivo do administrador: no cadastro a senha é obrigatória e, na edição, deixar o campo em branco mantém a senha atual.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_ehEdicao | Indica se o diálogo foi aberto em modo de edição de usuário existente | 1 | bool | Lógico | Discreto |
| m_original | Cópia do usuário recebido, cujo identificador e resumo de senha são preservados na edição | N/A | Usuario | Objeto | N/A |
| m_campoNome | Campo do nome do usuário | 120 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_campoEmail | Campo do endereço de e-mail, que deve ser único no sistema (RN002) | 120 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_campoSenha | Campo da senha, com o conteúdo mascarado e mínimo de 6 caracteres no cadastro (RN003) | 60 | Objeto (QLineEdit\*) | Componente gráfico | N/A |
| m_campoPapel | Seletor do papel do usuário: Administrador ou Investidor | N/A | Objeto (QComboBox\*) | Componente gráfico | N/A |
| m_rotuloAjuda | Rótulo auxiliar que informa as regras de preenchimento da senha conforme o modo do diálogo | N/A | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe CartaoIndicador.**

*Componente gráfico compacto para a apresentação de um indicador no painel: exibe o título, o valor em destaque e uma linha de detalhe que pode ser colorida conforme o sinal do número, atendendo à exigência de distinguir ganho e perda por cor e por sinal (REU002).*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| m_rotuloTitulo | Rótulo com o nome do indicador apresentado no cartão | 40 | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloValor | Rótulo com o valor principal do indicador, em destaque tipográfico | 20 | Objeto (QLabel\*) | Componente gráfico | N/A |
| m_rotuloDetalhe | Rótulo com a informação complementar, colorido em verde para valores positivos, em vermelho para negativos e em cinza para zero | 40 | Objeto (QLabel\*) | Componente gráfico | N/A |

**Quadro {{Q}}. Dicionário de informações da classe GraficoCandlestick.**

*Componente gráfico que desenha o gráfico de candles (abertura, máxima, mínima e fechamento) com as médias móveis sobrepostas, construído com o módulo Qt Charts (RF009). Ao instalar um gráfico novo, destrói explicitamente o anterior, pois a troca do gráfico apenas libera a posse do objeto substituído.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| MAXIMO_ROTULOS_EIXO | Constante de classe com o número máximo de rótulos exibidos no eixo de datas, para preservar a legibilidade. Valor 12 | 4 | int | Numérico | Discreto |

**Quadro {{Q}}. Dicionário de informações da classe GraficoLinha.**

*Componente gráfico que desenha a linha do preço de fechamento do ativo, utilizado na tela de detalhe e em visualizações secundárias (RF011). Assim como o gráfico de candles, destrói o gráfico anterior a cada redesenho, evitando o vazamento de memória.*

| Atributo | Descrição | Tamanho | Tipo | Formato | Domínio |
| :---- | :---- | :---- | :---- | :---- | :---- |
| definirDados(titulo, cotacoes) | Método que recebe a série de cotações já ordenada por data e redesenha a linha de fechamento. A classe não declara atributos próprios: herda o estado do componente QChartView e mantém o gráfico corrente sob a posse dele | N/A | void | Método | N/A |
| mostrarMensagem(mensagem) | Método que substitui o gráfico por uma mensagem, empregado quando não há cotações suficientes para o desenho | N/A | void | Método | N/A |
