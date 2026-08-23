### Especificação dos casos de uso

Esta seção detalha, um a um, os casos de uso identificados no Diagrama de Casos de Uso,
seguindo o formato de especificação adotado na disciplina: identificação do caso de uso,
atores, descrição, pré-condições, pós-condições, fluxo básico numerado com a função de
cada elemento indicada entre parênteses — `(ator)`, `(sistema)`, `(classe X)` e
`(atributo)` —, regras de negócio, fluxos alternativos e fluxos de exceção. Os passos, os
campos e as mensagens reproduzidos nos quadros a seguir correspondem exatamente ao
comportamento implementado no sistema Analisador B3.

**Quadro {{Q}}. Descrição do caso de uso UC001.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC001: Fazer login |
| **Atores** | Ator principal: Administrador / Investidor |
| **Descrição** | Caso de uso executado quando o usuário precisa se identificar no sistema para obter acesso às funcionalidades correspondentes ao seu papel. |
| **Pré-condições** | O usuário deve possuir cadastro no sistema; o banco de dados deve estar acessível. |
| **Pós-condições** | Sessão aberta com o usuário autenticado e janela principal exibindo apenas as opções permitidas ao seu papel. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador ou investidor) inicia o Analisador B3. (ator) | |
| | 2 - O sistema apresenta a tela de entrada com os campos: e-mail (classe Usuario), senha (classe Usuario), e com as opções Entrar e Cancelar. (sistema) |
| 3 - O ator informa o e-mail e a senha e seleciona Entrar. (ator) | |
| | 4 - O sistema verifica as informações conforme as Regras de Negócio RN001 e RN004. (sistema) (4a)(4b) |
| | 5 - O sistema recupera o usuário correspondente ao e-mail informado e compara o resumo SHA-256 da senha com o valor armazenado. (sistema) (classe Usuario) |
| | 6 - O sistema abre a sessão, registra o usuário logado com seu papel (classe Usuario) e apresenta a janela principal com as opções permitidas ao papel. (sistema) (atributo) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos: o e-mail e a senha são obrigatórios. |
| | RN004 — A senha é comparada pelo resumo SHA-256 de (sal + senha); a senha em texto puro nunca é armazenada nem trafegada. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso, finalizando a aplicação. |
| **Fluxo de exceção** | 4a - Caso o e-mail ou a senha não tenham sido preenchidos, o sistema exibe a mensagem "Informe e-mail e senha.", limpa o campo de senha e retorna ao passo 3 do fluxo básico. |
| | 4b - Caso não exista usuário com o e-mail informado ou o resumo da senha não confira, o sistema exibe a mensagem "E-mail ou senha inválidos.", limpa o campo de senha, posiciona o cursor nesse campo e retorna ao passo 3 do fluxo básico. A mensagem é propositalmente genérica, para não revelar se o e-mail existe no cadastro. |

**Quadro {{Q}}. Descrição do caso de uso UC002.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC002: Cadastrar ativo |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita incluir no cadastro uma nova ação ou um novo fundo imobiliário a ser acompanhado pelo sistema. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); o usuário autenticado deve possuir o papel Administrador. |
| **Pós-condições** | Ativo gravado no cadastro e apresentado na listagem de ativos. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) acessa a opção Ativos e seleciona Novo ativo. (ator) | |
| | 2 - O sistema apresenta o formulário de cadastro com os campos: tipo (Ação ou Fundo Imobiliário), ticker (classe Ativo), nome (classe Ativo), setor (classe Ativo), preço/lucro (classe Acao), dividend yield (classe Acao / classe FundoImobiliario), valor de mercado (classe Acao), valor patrimonial por cota (classe FundoImobiliario) e segmento (classe FundoImobiliario). (sistema) |
| 3 - O ator seleciona o tipo do ativo. (ator) | |
| | 4 - O sistema exibe apenas os campos de fundamentos pertinentes ao tipo escolhido: preço/lucro e valor de mercado para ação (classe Acao); valor patrimonial por cota e segmento para fundo imobiliário (classe FundoImobiliario). (sistema) |
| 5 - O ator preenche os campos e seleciona Salvar. (ator) | |
| | 6 - O sistema verifica as informações conforme as Regras de Negócio RN001 e RN005. (sistema) (6a)(6b)(6c)(6d)(6e) |
| | 7 - O sistema normaliza o ticker para caixa alta, grava o ativo com seus atributos ticker, nome da empresa, setor e fundamentos (atributo) (classe Acao / classe FundoImobiliario), atualiza a listagem e exibe a mensagem "Ativo <ticker> cadastrado com sucesso.". (sistema) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos: ticker, nome e setor são obrigatórios. |
| | RN005 — O ticker deve ter de 4 a 6 caracteres alfanuméricos (letras e números) e ser único no cadastro. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso. |
| **Fluxo de exceção** | 6a - Caso o ticker ou o nome não tenham sido preenchidos, o sistema exibe a mensagem "Preencha ao menos o ticker e o nome do ativo." e retorna ao passo 5 do fluxo básico. |
| | 6b - Caso o ticker não tenha de 4 a 6 caracteres entre letras e números, o sistema exibe a mensagem "Ticker inválido: use de 4 a 6 caracteres entre letras e números (ex.: PETR4)." e retorna ao passo 5 do fluxo básico. |
| | 6c - Caso o nome da empresa ou do fundo esteja vazio, o sistema exibe a mensagem "Informe o nome da empresa ou do fundo." e retorna ao passo 5 do fluxo básico. |
| | 6d - Caso o setor esteja vazio, o sistema exibe a mensagem "Informe o setor do ativo." e retorna ao passo 5 do fluxo básico. |
| | 6e - Caso já exista ativo cadastrado com o mesmo ticker, o sistema exibe a mensagem "Já existe um ativo cadastrado com o ticker <ticker>." e retorna ao passo 5 do fluxo básico. |

**Quadro {{Q}}. Descrição do caso de uso UC003.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC003: Atualizar ativo |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita corrigir ou atualizar os dados cadastrais e os fundamentos de um ativo já existente. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); deve existir ao menos um ativo cadastrado. |
| **Pós-condições** | Ativo atualizado no cadastro e listagem reapresentada com os novos valores. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) acessa a opção Ativos. (ator) | |
| | 2 - O sistema apresenta a listagem com as colunas: identificador (classe Ativo), ticker (classe Ativo), nome (classe Ativo), setor (classe Ativo), tipo (classe Acao / classe FundoImobiliario), dividend yield (classe Acao / classe FundoImobiliario), índice de valor (classe Ativo) e quantidade de cotações (classe Cotacao). (sistema) |
| 3 - O ator seleciona um ativo da listagem e escolhe Editar, ou aplica duplo clique sobre a linha. (ator) | |
| | 4 - O sistema recupera o ativo selecionado e apresenta o formulário preenchido com o tipo, o ticker (classe Ativo), o nome (classe Ativo), o setor (classe Ativo) e os fundamentos correspondentes ao tipo (classe Acao / classe FundoImobiliario). (sistema) (4a) |
| 5 - O ator altera os campos desejados e seleciona Salvar. (ator) | |
| | 6 - O sistema verifica as informações conforme as Regras de Negócio RN001 e RN005. (sistema) (6a)(6b)(6c)(6d) |
| | 7 - O sistema grava as alterações no ativo (atributo), atualiza a listagem e exibe a mensagem "Ativo <ticker> atualizado.". (sistema) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos: ticker, nome e setor são obrigatórios. |
| | RN005 — O ticker deve ter de 4 a 6 caracteres alfanuméricos e ser único; na atualização, o próprio ativo em edição é desconsiderado na verificação de unicidade. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso. |
| **Fluxo de exceção** | 4a - Caso o ativo selecionado não seja mais encontrado no cadastro, o sistema exibe a mensagem "O ativo selecionado não foi encontrado.", recarrega a listagem e encerra o caso de uso. |
| | 6a - Caso o ticker ou o nome tenham sido apagados, o sistema exibe a mensagem "Preencha ao menos o ticker e o nome do ativo." e retorna ao passo 5 do fluxo básico. |
| | 6b - Caso o ticker não tenha de 4 a 6 caracteres entre letras e números, o sistema exibe a mensagem "Ticker inválido: use de 4 a 6 caracteres entre letras e números (ex.: PETR4)." e retorna ao passo 5 do fluxo básico. |
| | 6c - Caso o setor esteja vazio, o sistema exibe a mensagem "Informe o setor do ativo." e retorna ao passo 5 do fluxo básico. |
| | 6d - Caso o ticker informado já pertença a outro ativo, o sistema exibe a mensagem "Já existe um ativo cadastrado com o ticker <ticker>." e retorna ao passo 5 do fluxo básico. |

**Quadro {{Q}}. Descrição do caso de uso UC004.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC004: Remover ativo |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita excluir do cadastro um ativo que não será mais acompanhado pelo sistema. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); o ativo a ser removido deve existir no cadastro. |
| **Pós-condições** | Ativo removido do cadastro, juntamente com as cotações, posições, alertas e recomendações a ele vinculados. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) acessa a opção Ativos, seleciona um ativo da listagem e escolhe Remover. (ator) | |
| | 2 - O sistema apresenta a confirmação "Remover <ticker> (<nome>)? Esta ação também apaga as cotações, posições, alertas e recomendações vinculadas e não pode ser desfeita.", exibindo o ticker (classe Ativo) e o nome da empresa (classe Ativo). (sistema) |
| 3 - O ator confirma a remoção. (ator) | |
| | 4 - O sistema exclui o ativo e os registros dependentes, atualiza a listagem e exibe a mensagem "Ativo <ticker> removido.". (sistema) (atributo) (4a)(4b) |
| **Regras de negócio** | RN001 — A remoção só é habilitada quando há um ativo selecionado na listagem; sem seleção, os botões Editar e Remover permanecem desabilitados. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona Não na confirmação: o sistema descarta a operação e encerra o caso de uso, mantendo o ativo no cadastro. |
| **Fluxo de exceção** | 4a - Caso nenhum ativo válido esteja selecionado, o sistema exibe a mensagem "Selecione um ativo válido para remover." e encerra o caso de uso. |
| | 4b - Caso o banco de dados recuse a exclusão, o sistema exibe a mensagem de erro devolvida pela camada de persistência, mantém o ativo no cadastro e retorna ao passo 1 do fluxo básico. |

**Quadro {{Q}}. Descrição do caso de uso UC005.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC005: Importar cotações |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita carregar no sistema o histórico de cotações de um ativo a partir de um arquivo CSV, de forma transacional. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); deve existir ao menos um ativo cadastrado (UC002); o arquivo CSV deve estar acessível no sistema de arquivos. |
| **Pós-condições** | Cotações do arquivo gravadas para o ativo escolhido, sem duplicidades, e registro da importação incluído no histórico com seu estado, linhas lidas e linhas inseridas. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) acessa a opção Importar cotações. (ator) | |
| | 2 - O sistema apresenta a tela de importação com o formato esperado ("Data;Abertura;Maxima;Minima;Fechamento;Volume", data em dd/mm/aaaa e decimal com vírgula), a lista de ativos cadastrados (classe Ativo), o campo do caminho do arquivo (classe Importacao) e o histórico das últimas importações com arquivo (classe Importacao), estado (classe Importacao), linhas lidas (classe Importacao), linhas inseridas (classe Importacao), data e hora de execução (classe Importacao) e mensagem de erro (classe Importacao). (sistema) (2a) |
| 3 - O ator seleciona o ativo de destino e escolhe o arquivo por meio da opção Escolher..., filtrando arquivos com extensão .csv. (ator) | |
| | 4 - O sistema exibe o caminho escolhido e, quando o nome do arquivo corresponde ao ticker de um ativo cadastrado (classe Ativo), seleciona automaticamente esse ativo na lista. (sistema) |
| 5 - O ator seleciona Importar arquivo. (ator) | |
| | 6 - O sistema abre o arquivo, cria o registro de importação com o arquivo (classe Importacao) e o ativo (classe Importacao) e passa o registro ao estado Validando. (sistema) (atributo) (6a)(6b)(6c) |
| | 7 - O sistema valida a primeira linha como cabeçalho e, em seguida, converte cada linha subsequente em uma cotação com data (classe Cotacao), abertura (classe Cotacao), máxima (classe Cotacao), mínima (classe Cotacao), fechamento (classe Cotacao) e volume (classe Cotacao), conforme as Regras de Negócio RN007, RN008 e RN009. (sistema) (7a)(7b)(7c)(7d)(7e)(7f)(7g)(7h) |
| | 8 - O sistema abre uma transação e grava em lote as cotações validadas, ignorando aquelas cuja data já exista para o mesmo ativo, conforme as Regras de Negócio RN010 e RN011. (sistema) (atributo) (8a)(8b) |
| | 9 - O sistema conclui o registro de importação com as linhas lidas (atributo) e as linhas inseridas (atributo) (classe Importacao), confirma a transação, atualiza o histórico e exibe o resumo "Importação concluída: <n> linha(s) lida(s), <n> inserida(s), <n> ignorada(s) por duplicidade.". (sistema) |
| **Regras de negócio** | RN007 — O arquivo de cotações deve ter o cabeçalho e as 6 colunas esperadas, separadas por ponto e vírgula. |
| | RN008 — A data deve estar no formato dd/mm/aaaa e os valores no padrão decimal brasileiro, com vírgula como separador decimal e ponto como separador de milhar. |
| | RN009 — Em cada cotação, a mínima deve ser menor ou igual à abertura e ao fechamento, que por sua vez devem ser menores ou iguais à máxima, e todos os preços devem ser maiores que zero. |
| | RN010 — Cotação de mesma data e mesmo ativo é ignorada, sem duplicar o registro já existente. |
| | RN011 — Qualquer erro estrutural rejeita o arquivo inteiro: nenhuma linha é gravada. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário cancela a janela de seleção de arquivo: o sistema descarta a escolha, mantém o caminho anterior e encerra o caso de uso sem importar. |
| **Fluxo alternativo 2** | * No passo 3, o ator seleciona Importar pasta inteira e escolhe um diretório: o sistema percorre todos os arquivos .csv do diretório em ordem alfabética, associa cada arquivo ao ativo cujo ticker corresponde ao nome do arquivo (classe Ativo) e executa os passos 6 a 9 para cada um deles, acumulando o total de linhas lidas, inseridas e ignoradas e relacionando ao final os erros de cada arquivo. Caso o diretório não exista, o sistema exibe "Diretório <caminho> não encontrado."; caso não haja arquivos .csv, exibe "Nenhum arquivo .csv encontrado em <caminho>."; e, para cada arquivo sem ativo correspondente, acrescenta "<arquivo>: nenhum ativo cadastrado com o ticker <ticker>.". |
| **Fluxo de exceção** | 2a - Caso não exista nenhum ativo cadastrado, o sistema desabilita a opção Importar arquivo, exibe a mensagem "Cadastre um ativo antes de importar cotações." e encerra o caso de uso. |
| | 6a - Caso o campo do arquivo esteja vazio, o sistema exibe a mensagem "Escolha um arquivo .csv para importar." e retorna ao passo 3 do fluxo básico. |
| | 6b - Caso nenhum ativo esteja selecionado, o sistema exibe a mensagem "Selecione o ativo de destino." e retorna ao passo 3 do fluxo básico. |
| | 6c - Caso o ativo informado não exista mais no cadastro, ou o arquivo não possa ser aberto para leitura, o sistema registra a importação como Rejeitada com a mensagem "Ativo informado não existe no cadastro." ou "Não foi possível abrir o arquivo <caminho>.", conforme o caso, e retorna ao passo 3 do fluxo básico. |
| | 7a - Caso a primeira linha não seja um cabeçalho reconhecível, o sistema interrompe a leitura, registra o erro "Linha 1: cabeçalho inválido. Esperado "Data;Abertura;Maxima;Minima;Fechamento;Volume"." e desvia para o passo de rejeição descrito em 7h. |
| | 7b - Caso uma linha não possua exatamente 6 colunas, o sistema registra o erro "Linha <n>: esperadas 6 colunas separadas por ';', encontradas <n>." e prossegue a leitura das demais linhas para relatar todos os erros do arquivo. |
| | 7c - Caso a data de uma linha não esteja no formato dd/MM/aaaa, o sistema registra o erro "Linha <n>: data "<valor>" fora do formato dd/MM/aaaa." e prossegue a leitura. |
| | 7d - Caso alguma coluna de preço ou de volume não contenha um número válido, o sistema registra o erro "Linha <n>: valor numérico inválido em uma das colunas de preço ou volume." e prossegue a leitura. |
| | 7e - Caso algum preço seja menor ou igual a zero, o sistema registra o erro "Linha <n>: preços devem ser maiores que zero."; caso a máxima seja menor que a mínima, registra "Linha <n>: máxima (<valor>) menor que a mínima (<valor>)."; caso a abertura ou o fechamento fiquem fora do intervalo entre a mínima e a máxima, registra "Linha <n>: abertura e fechamento devem ficar entre a mínima e a máxima."; caso o volume seja negativo, registra "Linha <n>: volume negativo.". Em todos os casos o sistema prossegue a leitura. |
| | 7f - Caso a mesma data apareça duas vezes no próprio arquivo, o sistema registra o erro "Linha <n>: data <dd/MM/aaaa> repetida no próprio arquivo." e prossegue a leitura. |
| | 7g - Caso o arquivo não contenha nenhuma cotação válida, o sistema registra a importação como Rejeitada com a mensagem "O arquivo não contém nenhuma cotação válida." e encerra o caso de uso. |
| | 7h - Caso tenha sido registrado ao menos um erro nos desvios 7a a 7f, o sistema rejeita o arquivo inteiro sem gravar nenhuma cotação, grava o registro de importação no estado Rejeitada com a primeira mensagem de erro (atributo) (classe Importacao), apresenta a lista completa de erros e exibe o resumo "Importação rejeitada: <n> erro(s) encontrado(s). Nenhuma cotação foi gravada.", retornando ao passo 3 do fluxo básico. |
| | 8a - Caso a transação não possa ser iniciada, o sistema registra a importação como Rejeitada com a mensagem devolvida pelo banco de dados e retorna ao passo 3 do fluxo básico. |
| | 8b - Caso a gravação em lote falhe, o sistema desfaz a transação, zera as linhas inseridas e ignoradas, registra a importação como Rejeitada com a mensagem "Falha ao gravar as cotações do arquivo." ou com o erro devolvido pelo banco e retorna ao passo 3 do fluxo básico. |

**Quadro {{Q}}. Descrição do caso de uso UC006.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC006: Configurar regra de análise |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita ajustar os parâmetros de uma regra de análise ou ativá-la e desativá-la, definindo quais estratégias participam da geração das recomendações. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); as regras de análise devem estar registradas no banco de dados. |
| **Pós-condições** | Regra gravada com os novos parâmetros e com o novo estado de ativação, passando a valer nas próximas análises. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) acessa a opção Regras de análise. (ator) | |
| | 2 - O sistema apresenta a listagem das regras com o nome da regra (classe RegraConfigurada), o estado ativa ou inativa (classe RegraConfigurada), o primeiro parâmetro (classe RegraConfigurada), o segundo parâmetro (classe RegraConfigurada) e o critério descrito pela própria estratégia já configurada (classe RegraAnalise). (sistema) (2a) |
| 3 - O ator seleciona uma regra e escolhe Editar parâmetros, ou aplica duplo clique sobre a linha. (ator) | |
| | 4 - O sistema apresenta o formulário de configuração com o nome da regra (classe RegraConfigurada), a descrição do critério vigente (classe RegraAnalise), a opção "Regra ativa (participa da análise)" (classe RegraConfigurada), o primeiro parâmetro (classe RegraConfigurada) e o segundo parâmetro (classe RegraConfigurada). (sistema) (4a) |
| 5 - O ator altera os parâmetros e o estado de ativação e seleciona Salvar. (ator) | |
| | 6 - O sistema verifica as informações conforme as Regras de Negócio RN001 e RN018. (sistema) (6a)(6b) |
| | 7 - O sistema grava a regra com o novo parâmetro principal (atributo), o novo parâmetro secundário (atributo) e o novo estado de ativação (atributo) (classe RegraConfigurada), atualiza a listagem, exibe a mensagem "Regra <nome> atualizada." e notifica as telas de análise para recalcular com a nova configuração. (sistema) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos: a regra deve possuir nome e os dois parâmetros devem ter valor informado. |
| | RN018 — Somente regras ativas participam da análise; regras inativas permanecem cadastradas, mas são ignoradas pelo motor de análise. |
| | RN019 — Uma regra sem dados suficientes se abstém, sem pontuar; a alteração de parâmetros pode fazer com que a regra passe a se abster para determinados ativos. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso. |
| **Fluxo alternativo 2** | * No passo 3, o ator seleciona uma regra e escolhe Ativar / desativar: o sistema inverte o estado de ativação da regra (classe RegraConfigurada), grava a alteração (atributo), atualiza a listagem e exibe a mensagem "Regra <nome> agora está ativa." ou "Regra <nome> agora está inativa.", conforme o novo estado. |
| **Fluxo de exceção** | 2a - Caso nenhuma regra esteja cadastrada, o sistema exibe a mensagem "Nenhuma regra cadastrada: o motor usará a configuração padrão embutida." e mantém a tela sem itens selecionáveis. |
| | 4a - Caso a regra selecionada não seja mais encontrada, o sistema recarrega a listagem e encerra o caso de uso. |
| | 6a - Caso os parâmetros informados sejam inválidos, isto é, negativos ou com nome de regra ausente, o sistema exibe a mensagem "Parâmetros inválidos: use valores não negativos." e retorna ao passo 5 do fluxo básico. |
| | 6b - Caso o banco de dados recuse a gravação, o sistema exibe a mensagem de erro devolvida pela camada de persistência e retorna ao passo 5 do fluxo básico. |

**Quadro {{Q}}. Descrição do caso de uso UC007.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC007: Gerenciar usuários |
| **Atores** | Ator principal: Administrador |
| **Descrição** | Caso de uso executado quando o administrador necessita cadastrar, alterar ou remover usuários do sistema, definindo o papel de cada um. |
| **Pré-condições** | O administrador deve estar autenticado no sistema (UC001); o usuário autenticado deve possuir o papel Administrador. |
| **Pós-condições** | Cadastro de usuários atualizado, preservada a existência de ao menos um administrador no sistema. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (administrador) acessa a opção Usuários. (ator) | |
| | 2 - O sistema apresenta a listagem de usuários com o identificador (classe Usuario), o nome (classe Usuario), o e-mail (classe Usuario) e o papel (classe Usuario). (sistema) |
| 3 - O ator seleciona Novo usuário. (ator) | |
| | 4 - O sistema apresenta o formulário com os campos: nome (classe Usuario), e-mail (classe Usuario), senha (classe Usuario) e papel, com as opções Investidor e Administrador (classe Usuario), além da orientação "A senha deve ter ao menos 6 caracteres.". (sistema) |
| 5 - O ator preenche os campos e seleciona Salvar. (ator) | |
| | 6 - O sistema verifica as informações conforme as Regras de Negócio RN001, RN002 e RN003. (sistema) (6a)(6b)(6c)(6d) |
| | 7 - O sistema normaliza o e-mail para caixa baixa, calcula o resumo SHA-256 da senha conforme a Regra de Negócio RN004, grava o usuário (atributo) (classe Usuario), atualiza a listagem e exibe a mensagem "Usuário <e-mail> cadastrado.". (sistema) |
| **Regras de negócio** | RN001 — Todos os campos obrigatórios do formulário devem estar preenchidos: nome, e-mail e senha são obrigatórios no cadastro. |
| | RN002 — O e-mail deve ser único no sistema. |
| | RN003 — A senha deve ter no mínimo 6 caracteres. |
| | RN004 — A senha é comparada e armazenada apenas pelo resumo SHA-256 de (sal + senha). |
| | RN006 — O último administrador não pode ser removido nem rebaixado ao papel de investidor. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona cancelar: o sistema descarta as informações digitadas e encerra o caso de uso. |
| **Fluxo alternativo 2** | * No passo 3, o ator seleciona um usuário da listagem e escolhe Editar: o sistema apresenta o formulário preenchido com o nome (classe Usuario), o e-mail (classe Usuario) e o papel (classe Usuario), com o campo de senha vazio e a orientação "A senha só é alterada se o campo for preenchido (mínimo de 6 caracteres)."; ao confirmar, o sistema grava as alterações (atributo), altera a senha somente se o campo tiver sido preenchido e exibe a mensagem "Usuário atualizado.". |
| **Fluxo alternativo 3** | * No passo 3, o ator seleciona um usuário da listagem e escolhe Remover: o sistema apresenta a confirmação "Remover o usuário selecionado? As carteiras e alertas dele também serão apagados."; confirmada a remoção, o sistema exclui o usuário e seus registros dependentes, atualiza a listagem e exibe a mensagem "Usuário removido."; caso o ator responda Não, o sistema descarta a operação e encerra o caso de uso. |
| **Fluxo de exceção** | 6a - Caso o nome esteja vazio, o sistema exibe a mensagem "Informe o nome do usuário." e retorna ao passo 5 do fluxo básico. |
| | 6b - Caso o e-mail não tenha formato válido, o sistema exibe a mensagem "Informe um e-mail válido." e retorna ao passo 5 do fluxo básico. |
| | 6c - Caso a senha tenha menos de 6 caracteres, o sistema exibe a mensagem "A senha deve ter ao menos 6 caracteres." e retorna ao passo 5 do fluxo básico. |
| | 6d - Caso já exista usuário com o e-mail informado, o sistema exibe a mensagem "Já existe um usuário com o e-mail <e-mail>." no cadastro, ou "O e-mail <e-mail> já pertence a outro usuário." na edição, e retorna ao passo 5 do fluxo básico. |
| | 6e - Caso o ator tente rebaixar o único administrador do sistema para o papel Investidor, o sistema exibe a mensagem "Este é o único administrador: o papel não pode ser alterado." e retorna ao passo 5 do fluxo básico, conforme a Regra de Negócio RN006. |
| | 6f - Caso o ator tente remover o único administrador do sistema, o sistema exibe a mensagem "O último administrador do sistema não pode ser removido." e encerra a remoção, conforme a Regra de Negócio RN006. |
| | 6g - Caso o ator tente remover o próprio usuário com o qual está autenticado, o sistema exibe a mensagem "Você não pode remover o usuário com o qual está logado." e encerra a remoção. |
| | 6h - Caso o usuário selecionado não seja mais encontrado, o sistema exibe a mensagem "Usuário não encontrado.", recarrega a listagem e encerra o caso de uso. |

**Quadro {{Q}}. Descrição do caso de uso UC008.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC008: Buscar ativos |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o usuário necessita localizar um ativo no cadastro informando parte do seu ticker ou do seu nome. |
| **Pré-condições** | O usuário deve estar autenticado no sistema (UC001); deve existir ao menos um ativo cadastrado. |
| **Pós-condições** | Listagem apresentada somente com os ativos que atendem ao termo pesquisado. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) digita no campo de busca parte do ticker ou do nome do ativo procurado. (ator) | |
| | 2 - O sistema consulta o cadastro comparando o termo informado com o ticker (classe Ativo) e com o nome da empresa (classe Ativo), sem diferenciar maiúsculas de minúsculas, e reapresenta a listagem apenas com os ativos correspondentes, exibindo ticker (classe Ativo), nome (classe Ativo), setor (classe Ativo), tipo (classe Acao / classe FundoImobiliario), dividend yield (classe Acao / classe FundoImobiliario), índice de valor (classe Ativo) e quantidade de cotações (classe Cotacao). (sistema) (2a)(2b) |
| **Regras de negócio** | RN001 — Quando o campo de busca está vazio, o sistema apresenta a listagem completa dos ativos cadastrados. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário limpa o campo de busca: o sistema descarta o termo informado e reapresenta a listagem completa dos ativos. |
| **Fluxo alternativo 2** | * A qualquer momento o usuário seleciona Recarregar: o sistema consulta novamente o cadastro e reapresenta a listagem aplicando o termo de busca vigente. |
| **Fluxo de exceção** | 2a - Caso nenhum ativo atenda ao termo informado, o sistema exibe a mensagem "Nenhum ativo encontrado para "<termo>"." e apresenta a listagem vazia, retornando ao passo 1 do fluxo básico. |
| | 2b - Caso não exista nenhum ativo cadastrado, o sistema exibe a mensagem "Nenhum ativo cadastrado. Use "Novo ativo" para começar." e encerra o caso de uso. |

**Quadro {{Q}}. Descrição do caso de uso UC009.**

| | |
|----------------------|--------------------------------------------|
| **Nome** | UC009: Visualizar painel de análise do ativo |
| **Atores** | Ator principal: Investidor |
| **Descrição** | Caso de uso executado quando o investidor necessita acompanhar a evolução das cotações de um ativo, com médias móveis, indicadores técnicos do período e a recomendação vigente. |
| **Pré-condições** | O usuário deve estar autenticado no sistema (UC001); deve existir ao menos um ativo cadastrado (UC002); devem existir cotações importadas para o ativo escolhido (UC005). |
| **Pós-condições** | Gráfico de cotações, indicadores do período e recomendação vigente apresentados para o ativo e o período selecionados. |
| **Fluxo básico — Ações dos atores** | **Ações do sistema** |
| 1 - O ator (investidor) acessa a opção Painel de análise. (ator) | |
| | 2 - O sistema apresenta a lista dos ativos cadastrados, identificados por ticker e nome (classe Ativo), e a lista de períodos com as opções Últimos 30 pregões, Últimos 90 pregões, Últimos 180 pregões, Último ano e Série completa. (sistema) (2a) |
| 3 - O ator seleciona o ativo e o período desejados. (ator) | |
| | 4 - O sistema recupera o histórico de cotações do ativo no período, com data (classe Cotacao), abertura (classe Cotacao), máxima (classe Cotacao), mínima (classe Cotacao) e fechamento (classe Cotacao). (sistema) (4a)(4b) |
| | 5 - O sistema calcula as médias móveis simples de 9 e de 21 pregões (classe CalculadoraIndicadores) e desenha o gráfico de candles com as duas médias sobrepostas. (sistema) |
| | 6 - O sistema calcula e apresenta os cartões de indicadores do período: último fechamento com a respectiva data (classe Cotacao), variação percentual do período (classe CalculadoraIndicadores), índice de força relativa de 14 pregões com a leitura sobrevendido, faixa neutra ou sobrecomprado (classe CalculadoraIndicadores) e volatilidade anualizada (classe CalculadoraIndicadores). (sistema) (6a) |
| | 7 - O sistema consulta a recomendação vigente do ativo e apresenta o selo com o tipo Compra, Venda ou Neutro (classe Recomendacao) e a pontuação (classe Recomendacao), acompanhado da justificativa que cita as regras e os números que a sustentaram (classe Recomendacao). (sistema) (7a) |
| **Regras de negócio** | RN015 — A pontuação de uma recomendação varia de −1 a +1. |
| | RN016 — Pontuação maior ou igual a +0,25 corresponde a Compra; menor ou igual a −0,25 corresponde a Venda; entre esses limites, Neutro. |
| | RN019 — Uma regra ou um indicador sem dados suficientes se abstém, sem pontuar; nesse caso o cartão correspondente é apresentado vazio. |
| **Fluxo alternativo 1** | * A qualquer momento o usuário seleciona Analisar agora: o sistema executa o caso de uso UC011 para o ativo selecionado, invalida a recomendação anterior conforme a Regra de Negócio RN017, publica a nova recomendação (classe Recomendacao) e redesenha o painel com o novo selo e a nova justificativa. |
| **Fluxo alternativo 2** | * A qualquer momento o usuário seleciona Ver detalhes: o sistema encerra este caso de uso e abre o caso de uso UC010 para o ativo selecionado. |
| **Fluxo de exceção** | 2a - Caso não exista nenhum ativo cadastrado, o sistema desabilita as opções Analisar agora e Ver detalhes, apresenta a mensagem "Nenhum ativo cadastrado." na área do gráfico, limpa os cartões de indicadores e encerra o caso de uso. |
| | 4a - Caso o ativo selecionado não seja encontrado no cadastro, o sistema apresenta a mensagem "Ativo não encontrado." na área do gráfico e encerra o caso de uso. |
| | 4b - Caso não existam cotações importadas para o ativo no período, o sistema apresenta a mensagem "Sem cotações importadas para este ativo." na área do gráfico e retorna ao passo 3 do fluxo básico. |
| | 6a - Caso a série seja curta demais para o cálculo de um indicador, o sistema apresenta o cartão correspondente vazio, sem interromper a exibição dos demais. |
| | 7a - Caso não exista recomendação vigente para o ativo, o sistema apresenta o selo "Sem recomendação" e o texto "Nenhuma recomendação vigente para <ticker>. Use "Analisar agora" para gerar uma com base nas regras ativas.", retornando ao passo 3 do fluxo básico. |
| | 7b - Caso o ator selecione Analisar agora e a geração falhe, o sistema exibe em uma caixa de aviso a mensagem devolvida pelo serviço de recomendação, como "Ativo não encontrado no cadastro." ou "Não foi possível publicar a recomendação gerada.", e retorna ao passo 3 do fluxo básico. |
