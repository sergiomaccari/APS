# Roteiro — Apresentação 1: Testes do Protótipo (20/10/2026)

- **Branch:** `entrega-13-10` (versão que acompanha o documento do 1º bimestre
  entregue em 13/10/2026).
- **Executável:** `analisador-b3-prototipo`.
- **Valor:** 2,0 pts — "telas gráficas funcionais representando os casos de uso".
- **Referência no documento:** capítulo 4 (Resultados → Protótipo das telas,
  Figuras 2 a 13) e quadros UC001–UC020.

## O que é esta versão

É o **protótipo de telas** do Analisador B3: as mesmas telas descritas no
documento, navegáveis nos 20 casos de uso, funcionando sobre **dados de
demonstração fixos** (2 usuários, 5 ativos, 4 regras de análise, a carteira e os
alertas do investidor, 5 recomendações e as cotações dos CSVs de `dados/`).

Pela classificação da Aula 03, convém dizer na abertura que é um protótipo
**horizontal** (cobre os 20 casos de uso) e de **alta fidelidade visual**
(construído na própria ferramenta de desenvolvimento, C++ com Qt 6), mas sem a
lógica de negócio definitiva. A "regra do jogo" combinada com o cliente: o
protótipo serve para validar requisitos e fluxos de tela; o cálculo real, a
persistência e a sincronização online chegam na versão final (2º bimestre).

O que acontece de verdade e o que é resultado pré-pronto:

| Comportamento | No protótipo |
|---|---|
| Login, navegação por papel, troca de usuário | Funciona (só as duas contas de demonstração) |
| Cadastrar/editar/remover ativo, usuário e regra; comprar e vender; criar e arquivar alerta | Funciona **em memória**: as telas refletem a mudança até fechar o programa |
| Disparo de alertas | Funciona: o alerta é avaliado contra as cotações de demonstração e notificado na barra de status |
| Gráfico de velas, médias móveis 9/21, IFR e volatilidade | Desenhados a partir das cotações de demonstração |
| Importação de cotações | **Resultado pré-pronto**: resumo fixo de sucesso e nova linha no histórico; nenhuma cotação muda |
| Gerar recomendações | **Resultado pré-pronto**: as 5 recomendações de demonstração |
| Backtesting | **Resultado pré-pronto**: tabela fixa por regra |
| Sincronização online | **Prevista para a versão final (2º bimestre)**: o botão informa isso, sem acessar a rede |

Nada é gravado em disco. **Ao fechar o programa, tudo volta ao estado inicial** —
útil para ensaiar quantas vezes for preciso e para começar a apresentação sempre
do mesmo ponto.

Quando um passo for resultado pré-pronto, a fala sugerida é: **"o protótipo mostra
o resultado esperado; o cálculo real chega no 2º bimestre"**.

## Antes da apresentação (checklist)

- [ ] Fazer checkout da branch: `git fetch --prune && git checkout entrega-13-10`.
- [ ] Compilar em Release:
      `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DAPENAS_PROTOTIPO=ON && cmake --build build -j`.
- [ ] Abrir `./build/analisador-b3-prototipo` uma vez na máquina do laboratório,
      conferir que o título da janela termina em "(protótipo)" e fechar (o estado
      volta ao inicial).
- [ ] Deixar a pasta `dados/` à mão no seletor de arquivos (passos 6 e 7).
- [ ] Ensaiar o roteiro inteiro ao menos uma vez; fechar o programa ao final para
      a apresentação começar limpa.
- [ ] Combinar quem apresenta cada bloco (sugestão: um integrante por bloco).

## Roteiro (≈ 12 minutos)

Credenciais: administrador `admin@analisadorb3.com` / `admin123`; investidor
`investidor@analisadorb3.com` / `investidor123`.

### Bloco A — Acesso (1 min)

| # | Ação | Caso de uso | O que se mostra |
|---|---|---|---|
| 1 | Abrir `./build/analisador-b3-prototipo`, tentar uma senha errada e depois entrar como administrador | UC001 | Mensagem "E-mail ou senha inválidos."; em seguida a janela principal com a navegação de administração (separador "— Administração —") |

### Bloco B — Administração (5 min, perfil administrador)

| # | Ação | Caso de uso | O que se mostra |
|---|---|---|---|
| 2 | *Ativos* → digitar "PE" na busca | UC008 | Lista filtrada por ticker/nome |
| 3 | *Novo ativo* → cadastrar um ativo de teste | UC002 | Ativo aparece na lista (em memória) |
| 4 | *Editar* o ativo de teste (ex.: mudar o setor) | UC003 | Alteração refletida na tabela |
| 5 | *Remover* o ativo de teste (confirmar) | UC004 | Confirmação pedida e ativo removido |
| 6 | *Importar cotações* → **Importar pasta inteira** → `dados/` | UC005 | **Pré-pronto:** "Importação concluída: 650 cotações em 5 arquivos; 1 arquivo rejeitado (INVALIDO-exemplo.csv)." e nova linha no histórico de importações. Explicar a regra que o resumo ilustra: um arquivo com erro é rejeitado por inteiro e os demais seguem. "O protótipo mostra o resultado esperado; a leitura e a validação real dos CSVs chegam no 2º bimestre." |
| 7 | Apontar no resumo o arquivo `INVALIDO-exemplo.csv` rejeitado | UC005 (fluxo alternativo) | Fluxo de exceção do quadro UC005: arquivo rejeitado, nenhuma cotação gravada. Mostrar o arquivo no seletor para a turma ver que ele existe |
| 8 | *Regras de análise* → *Editar parâmetros* de uma regra; *Ativar / desativar* | UC006 | Parâmetros e peso alterados na tabela (em memória) |
| 9 | **Backtesting…** → *Executar backtesting* | UC020 | **Pré-pronto:** tabela de sinais, taxa de acerto e retorno médio por regra. "O protótipo mostra o resultado esperado; o backtesting real sobre o histórico chega no 2º bimestre." |
| 10 | *Importar cotações* → **Sincronizar online** | UC019 | Mensagem "Sincronização online prevista para a versão final (2º bimestre). Nenhuma cotação alterada." Dizer que a tela e o fluxo já estão definidos e que o download do período faltante entra na versão final |
| 11 | *Usuários* → *Novo usuário* / *Editar* / *Remover* | UC007 | CRUD de usuários com papel (em memória) |
| 12 | *Recomendações* → **Gerar para todos os ativos** | UC011 | **Pré-pronto:** PETR4 **Compra**, VALE3 **Compra**, HGLG11 **Compra**, ITUB4 **Neutro**, MGLU3 **Venda**, cada uma com justificativa. "O protótipo mostra o resultado esperado; o motor de análise real chega no 2º bimestre." |

### Bloco C — Investidor (5 min)

| # | Ação | Caso de uso | O que se mostra |
|---|---|---|---|
| 13 | *Sistema → Trocar usuário* → entrar como investidor | UC001 · UC017 | Navegação sem o bloco de administração. Logo ao abrir, o alerta de PETR4 dispara e a barra de status mostra "Alerta disparado: …" (Observer) |
| 14 | *Painel de análise* → escolher PETR4 e o período; **Analisar agora** | UC009 | Velas, médias móveis 9/21, cartões (fechamento, variação, IFR, volatilidade) desenhados a partir das cotações de demonstração, e o selo da recomendação (pré-pronta) |
| 15 | **Ver detalhes** | UC010 | Fundamentos, gráfico de linha, últimas cotações e parecer regra por regra |
| 16 | *Recomendações* → selecionar MGLU3 → **Abrir no painel** | UC012 | Recomendações vigentes com justificativa numérica; o painel abre em MGLU3 |
| 17 | *Minha carteira* → **Comprar** 10 PETR4 | UC013 | Posição PETR4 passa de 200 para 210; preço médio recalculado |
| 18 | **Vender** parte de uma posição | UC014 | Quantidade da posição reduzida |
| 19 | Observar o resumo consolidado | UC015 | Valor de mercado e lucro/prejuízo por posição e total, recalculados após as operações |
| 20 | *Alertas* → **Novo alerta** → condição "Preço acima de" com valor menor que o último fechamento (o diálogo mostra o último fechamento) | UC016 | Alerta cadastrado como ativo |
| 21 | **Avaliar agora** | UC017 | O alerta dispara; notificação na barra de status |
| 22 | **Arquivar** o alerta disparado | UC018 | Mensagem "Alerta arquivado." e estado atualizado na tabela |

### Fechamento (1 min)

- Mostrar *Ajuda → Sobre* (versão protótipo de telas, equipe, padrões de projeto).
- Reforçar: as telas e fluxos correspondem aos quadros UC001–UC020 do documento;
  importação, recomendações e backtesting mostram o resultado esperado e o cálculo
  real, a gravação em banco e a sincronização online chegam na versão final
  (2º bimestre).
- Lembrar o aviso: as recomendações são apoio à decisão baseado em regras
  explícitas e não constituem recomendação de investimento.

## Se algo der errado

| Sintoma | O que fazer |
|---|---|
| Configuração do CMake não encontra o Qt | Instalar Qt 6.4+ com Widgets e Charts (`qt6-base-dev qt6-charts-dev` no Ubuntu) ou informar o caminho com `-DCMAKE_PREFIX_PATH=<pasta do Qt>` |
| Título da janela sem "(protótipo)" ou tela diferente do documento | Build antigo ou de outra pasta: apagar `build/`, recompilar com `-DAPENAS_PROTOTIPO=ON` e rodar `./build/analisador-b3-prototipo` |
| Dados diferentes dos do roteiro (ativo de teste sobrando, posição alterada) | Sobrou estado de um ensaio: fechar o programa e abrir de novo (tudo volta ao inicial) |
| Alerta de PETR4 não apareceu ao entrar como investidor | A mensagem some da barra de status depois de alguns segundos; ir em *Alertas* → **Avaliar agora** |
| Recomendações vazias | Clicar em **Gerar para todos os ativos** (passo 12) |
| Perguntarem por que a importação/sincronização "não fez nada" | É o comportamento previsto do protótipo: resultado pré-pronto; o cálculo real chega no 2º bimestre |
