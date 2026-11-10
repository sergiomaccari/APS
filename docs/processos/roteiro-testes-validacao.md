# Roteiro — Testes de Validação (24/11/2026)

- **Versão:** sistema completo (`analisador-b3`), a mesma que acompanha o documento
  do 2º bimestre entregue em 17/11/2026 (branch `entrega-17-11`).
- **Valor:** 2,0 pts — "Requisitos atendidos pelos Casos de Uso" e "Diagramas de
  Seqüência" (`APS/txt do site.txt`).
- ⚠️ **A nota escrita (8,0 pts) só é computada se o sistema for apresentado
  funcionando.** Não há reavaliação no 2º bimestre.
- A ordem das equipes segue a ordem de entrega do trabalho: **entregar cedo =
  apresentar cedo**.
- **Referência no documento do 2º bimestre:** quadros UC001–UC020 (Quadros 1–20),
  diagramas de sequência (Figuras 20–39) e telas (Capítulo 4.1).

## A lógica da apresentação

Para cada caso de uso, a equipe faz três coisas, nesta ordem:

1. **Diz qual requisito o caso de uso atende** (coluna "RF").
2. **Executa o caso de uso no sistema** e mostra o resultado esperado.
3. **Aponta o diagrama de sequência** (coluna "Figura"), percorrendo as classes na
   ordem em que as mensagens aparecem: o que foi clicado na tela corresponde à
   primeira mensagem, e o resultado mostrado é o retorno do diagrama.

É exatamente o que os dois critérios cobram. Deixar o PDF do documento aberto numa
segunda janela, na página das Figuras 20–39, para alternar rápido.

## Antes da apresentação (checklist)

- [ ] `git fetch --prune && git checkout entrega-17-11` (ou a `main`).
- [ ] Compilar em Release: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j`.
- [ ] `ctest --test-dir build` — 5 conjuntos passando (78 casos de teste; o relatório do ctest soma 88 com a preparação de cada conjunto). Bom argumento se o
      professor perguntar sobre testes.
- [ ] **Começar com o banco limpo** (o banco fica fora da pasta do projeto):
      - Linux/WSL: `rm -rf ~/.local/share/"UTFPR - APS"/"Analisador B3"`
      - Windows: apagar `%APPDATA%\UTFPR - APS\Analisador B3`
- [ ] Abrir o sistema uma vez, conferir que sobe sem erro, fechar e **apagar o
      banco de novo** (para a apresentação começar do zero).
- [ ] Deixar a pasta `dados/` à mão no seletor de arquivos.
- [ ] Testar a internet do laboratório. Sem internet, o passo 23 (UC019) é
      apresentado pela mensagem de falha prevista no próprio quadro do caso de uso.
- [ ] No Windows: a sincronização online exige o OpenSSL disponível para o Qt;
      sem ele, aparece a mensagem do fluxo 6a do UC019 (também prevista).
- [ ] Combinar quem apresenta cada bloco (sugestão: um integrante por bloco; todos
      devem saber explicar qualquer diagrama).

Credenciais: administrador `admin@analisadorb3.com` / `admin123`; investidor
`investidor@analisadorb3.com` / `investidor123`.

## Roteiro (≈ 15 minutos)

### Bloco A — Acesso e cadastro (administrador, 4 min)

| # | RF | Caso de uso | Ação | Resultado esperado | Figura (sequência) |
|---|---|---|---|---|---|
| 1 | RF001 | UC001 Fazer login | Tentar uma senha errada; depois entrar como administrador | "E-mail ou senha inválidos."; em seguida a janela principal com a navegação de administração | 20 — TelaLogin → ServicoAutenticacao → RepositorioUsuario |
| 2 | RF008 | UC008 Buscar ativos | *Ativos* → digitar "PE" na busca | Lista filtrada por ticker ou nome | 27 — TelaAtivos → ServicoAtivo → RepositorioAtivo |
| 3 | RF002 | UC002 Cadastrar ativo | *Novo ativo* → cadastrar um ativo de teste (ex.: BBAS3) | Ativo aparece na lista; ticker repetido é recusado | 21 — TelaAtivos → DialogoAtivo → ServicoAtivo → RepositorioAtivo |
| 4 | RF003 | UC003 Atualizar ativo | *Editar* o ativo de teste (mudar o setor) | Alteração refletida na tabela | 22 |
| 5 | RF004 | UC004 Remover ativo | *Remover* o ativo de teste e confirmar | Confirmação pedida e ativo removido | 23 |
| 6 | RF007 | UC007 Gerenciar usuários | *Usuários* → *Novo usuário* (investidor de teste) → *Editar* → *Remover* | CRUD com papel; e-mail repetido é recusado | 26 — TelaUsuarios → ServicoAutenticacao → RepositorioUsuario |

### Bloco B — Dados e análise (administrador, 4 min)

| # | RF | Caso de uso | Ação | Resultado esperado | Figura (sequência) |
|---|---|---|---|---|---|
| 7 | RF005 | UC005 Importar cotações | *Importar cotações* → **Importar pasta inteira** → `dados/` | Os 5 CSVs são casados com os ativos pelo ticker: 650 cotações gravadas; o `INVALIDO-exemplo.csv` aparece como erro (não há ativo com esse ticker); nova linha no histórico de importações | 24 — TelaImportacao → ServicoImportacao → RepositorioCotacao (inserção em lote numa transação) |
| 8 | RF005 | UC005 (fluxo de exceção) | Escolher um ativo, **Importar arquivo** → `dados/INVALIDO-exemplo.csv` | "Importação rejeitada: N erro(s) encontrado(s). Nenhuma cotação foi gravada." e os erros linha a linha. **Explicar a transação:** o arquivo é rejeitado por inteiro | 24 (ramo de falha: `desfazer()` da transação) |
| 9 | RF006 | UC006 Configurar regra | *Regras de análise* → *Editar parâmetros* de uma regra (peso, períodos); *Ativar / desativar* | Parâmetros e peso atualizados; o critério é descrito pela própria regra (Strategy) | 25 — TelaRegras → DialogoRegra → RepositorioRegra |
| 10 | RF021 | UC020 Backtesting | **Backtesting…** → *Executar* | Sinais, taxa de acerto e retorno médio por regra, calculados sobre o histórico importado | 39 — TelaRegras → DialogoBacktest → Backtester → MotorAnalise |
| 11 | RF012 | UC011 Gerar recomendações | *Recomendações* → **Gerar para todos os ativos** | PETR4 **Compra** 0,72 · VALE3 **Compra** 0,45 · HGLG11 **Compra** 0,38 · ITUB4 **Neutro** −0,08 · MGLU3 **Venda** −0,37, cada uma com justificativa numérica | 30 — TelaRecomendacoes → ServicoRecomendacao → MotorAnalise (regras ativas) → RepositorioRecomendacao |

> Se no passo 9 alguma regra foi desativada ou teve o peso alterado, as notas do
> passo 11 mudam. Para mostrar exatamente os valores acima, restaurar a regra antes
> de gerar, ou fazer o passo 9 só como visualização e cancelar o diálogo.

### Bloco C — Investidor (5 min)

| # | RF | Caso de uso | Ação | Resultado esperado | Figura (sequência) |
|---|---|---|---|---|---|
| 12 | RF001 | UC001 | *Sistema → Trocar usuário* → entrar como investidor | Navegação sem o bloco de administração (perfil por papel) | 20 |
| 13 | RF009 · RF010 | UC009 Painel de análise | *Painel de análise* → PETR4 e período; **Analisar agora** | Velas, médias móveis 9/21, cartões (fechamento, variação, RSI, volatilidade) e selo da recomendação | 28 — TelaDashboard → RepositorioCotacao → CalculadoraIndicadores → GraficoCandlestick |
| 14 | RF011 | UC010 Detalhes do ativo | **Ver detalhes** | Fundamentos (P/L, DY), gráfico de linha, últimas cotações e parecer regra por regra | 29 — TelaDetalheAtivo → ServicoAtivo → RepositorioCotacao → ServicoRecomendacao |
| 15 | RF013 | UC012 Recomendações vigentes | *Recomendações* → selecionar MGLU3 → **Abrir no painel** | Recomendações vigentes com justificativa; o painel abre em MGLU3 | 31 — TelaRecomendacoes → ServicoRecomendacao → RepositorioRecomendacao |
| 16 | RF014 | UC013 Comprar ativo | *Minha carteira* → **Comprar** 10 PETR4 | Posição de PETR4 passa de 200 para 210; preço médio ponderado recalculado | 32 — TelaCarteira → DialogoOperacao → ServicoCarteira → RepositorioCarteira |
| 17 | RF015 | UC014 Vender ativo | **Vender** parte de uma posição (tentar vender mais do que tem, depois uma quantidade válida) | Venda acima do saldo é recusada; a válida reduz a posição | 33 |
| 18 | RF016 | UC015 Resultado da carteira | Observar o resumo consolidado | Valor de mercado e lucro/prejuízo por posição e total, recalculados após as operações | 34 — TelaCarteira → ServicoCarteira → RepositorioCarteira → RepositorioCotacao |
| 19 | RF017 | UC016 Cadastrar alerta | *Alertas* → **Novo alerta** → "Preço acima de" com valor menor que o último fechamento (o diálogo mostra o último fechamento) | Alerta cadastrado como ativo | 35 — TelaAlertas → DialogoAlerta → ServicoAlerta → RepositorioAlerta |
| 20 | RF018 | UC017 Avaliar e disparar alertas | **Avaliar agora** | O alerta dispara; notificação na barra de status (padrão Observer) | 36 — JanelaPrincipal → ServicoAlerta → RepositorioAlerta → RepositorioCotacao |
| 21 | RF019 | UC018 Arquivar alerta | **Arquivar** o alerta disparado | "Alerta arquivado." e estado atualizado na tabela | 37 — TelaAlertas → ServicoAlerta → RepositorioAlerta |
| 22 | — | Persistência | Fechar o sistema e abrir de novo como investidor | Compra, venda e alertas continuam lá: estão gravados no banco SQLite (diferença em relação ao protótipo do 1º bimestre) | — |

### Bloco D — Sincronização online (administrador, 1 min, por último)

| # | RF | Caso de uso | Ação | Resultado esperado | Figura (sequência) |
|---|---|---|---|---|---|
| 23 | RF020 | UC019 Sincronizar cotações | *Trocar usuário* → administrador → *Importar cotações* → **Sincronizar online**; clicar uma segunda vez | "Sincronização online: 5 ativo(s) verificado(s), N cotação(ões) nova(s)…"; na segunda vez, 0 novas (só o período faltante é baixado) | 38 — TelaImportacao → ServicoSincronizacao → RepositorioCotacao (última data) → FonteYahooFinance → RepositorioImportacao |

> **Por que por último:** a sincronização acrescenta as cotações reais até o dia da
> apresentação, e as recomendações geradas depois disso deixam de ser as do passo
> 11 (os CSVs foram calibrados para mostrar Compra, Neutro e Venda). Se o
> professor pedir, gere de novo e explique que o motor recalculou sobre os dados
> reais. Sem internet ou sem OpenSSL, a mensagem de falha é o fluxo de exceção
> previsto no quadro do UC019.

### Fechamento (1 min)

- *Ajuda → Sobre*: versão, equipe e os padrões de projeto aplicados (Strategy nas
  regras e na fonte de cotações, Observer nos alertas, Repository na persistência).
- Reforçar: os 21 requisitos funcionais foram demonstrados pelos 20 casos de uso,
  e cada caso de uso tem o seu diagrama de sequência fiel às classes do código.
- Aviso: as recomendações apoiam a decisão com regras explícitas e não constituem
  recomendação de investimento.

## Perguntas prováveis do professor

| Pergunta | Resposta curta |
|---|---|
| "Onde essa mensagem do diagrama está no código?" | Os nomes das classes e métodos do diagrama são os do código: ex. `ServicoImportacao::importarCsv` em `src/servicos/servicoimportacao.cpp` |
| "O que acontece se a importação falhar no meio?" | Tudo roda numa transação: qualquer erro desfaz a importação inteira (passo 8; testado em `testes/testeimportacao.cpp`) |
| "Quantas tabelas tem o banco?" | 9 tabelas, criadas por migrações versionadas (`recursos/sql/`), as mesmas do DER |
| "Como o sistema decide a recomendação?" | Cada regra ativa dá um parecer de −1 a +1; a média ponderada pelos pesos define Compra (> 0,25), Venda (< −0,25) ou Neutro |
| "E se não houver internet?" | A importação por CSV é o caminho principal; a sincronização é um complemento e falha com mensagem clara |

## Se algo der errado

| Sintoma | O que fazer |
|---|---|
| CMake não encontra o Qt | Instalar Qt 6.4+ com Widgets, Charts, Sql e Network, ou passar `-DCMAKE_PREFIX_PATH=<pasta do Qt>` |
| "Driver QSQLITE não disponível" | Linux: `sudo apt install libqt6sql6-sqlite`; Windows: usar o Qt instalado pelo instalador oficial (já traz o driver) |
| Gráfico vazio "Sem cotações importadas" | Fazer o passo 7 (importar a pasta `dados/`) |
| Dados diferentes do roteiro (ativo de teste sobrando, posição alterada) | Sobrou estado de um ensaio: fechar, apagar o banco (checklist) e abrir de novo |
| Recomendações diferentes das do passo 11 | Houve sincronização antes, ou uma regra foi alterada no passo 9: apagar o banco e refazer a partir do passo 7 |
| Sincronização falha | Mostrar a mensagem como fluxo de exceção do UC019 e seguir |
