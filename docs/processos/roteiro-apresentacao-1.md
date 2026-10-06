# Roteiro — Apresentação 1: Testes do Protótipo (20/10/2026)

- **Branch:** `13/10-prototipo` (versão congelada que acompanha o documento do
  1º bimestre entregue em 13/10/2026).
- **Valor:** 2,0 pts — "telas gráficas funcionais representando os casos de uso".
- **Referência no documento:** capítulo 4 (Resultados → Protótipo das telas,
  Figuras 2 a 13) e quadros UC001–UC020.

## O que é esta versão

O documento do 1º bimestre declara que o protótipo foi "construído na tecnologia
definitiva (C++ com Qt 6 e SQLite), navegável em todos os casos de uso e
calculando indicadores reais sobre cotações importadas". Esta branch é exatamente
isso: o sistema no estado descrito pelo documento, marcado como protótipo no
título da janela e em *Ajuda → Sobre*.

Pela classificação da Aula 03, convém dizer na abertura que é um protótipo de
**alta fidelidade** (construído na própria ferramenta de desenvolvimento),
**horizontal** (cobre os 20 casos de uso) e com fatias **verticais** reais
(importação, motor de análise e banco funcionam de ponta a ponta). A "regra do
jogo" combinada com o cliente: o protótipo serve para validar os requisitos e
recebe o refinamento do 2º bimestre (fluxos de exceção, testes de validação).

## Antes da apresentação (checklist)

- [ ] Fazer checkout da branch: `git switch 13/10-prototipo`.
- [ ] Compilar em Release: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j`.
- [ ] Rodar os testes: `ctest --test-dir build --output-on-failure` (5 alvos passando).
- [ ] **Recriar o banco do zero** para a demo começar limpa:
      `rm -f ~/.local/share/"UTFPR - APS"/"Analisador B3"/analisador-b3.sqlite`.
- [ ] Testar na máquina do laboratório **sem internet** (só o passo 10 usa rede).
- [ ] Deixar a pasta `dados/` à mão no seletor de arquivos.
- [ ] Combinar quem apresenta cada bloco 🟦 (sugestão: um integrante por bloco).

## Roteiro (≈ 12 minutos)

Credenciais: administrador `admin@analisadorb3.com` / `admin123`; investidor
`investidor@analisadorb3.com` / `investidor123`.

### Bloco A — Acesso (1 min)

| # | Ação | Caso de uso | Resultado esperado |
|---|---|---|---|
| 1 | Abrir `./build/analisador-b3`, tentar senha errada e depois entrar como administrador | UC001 | Mensagem "E-mail ou senha inválidos."; em seguida a janela principal com a navegação de administração |

### Bloco B — Administração (5 min, perfil administrador)

| # | Ação | Caso de uso | Resultado esperado |
|---|---|---|---|
| 2 | *Ativos* → digitar "PE" na busca | UC008 | Lista filtrada por ticker/nome |
| 3 | *Novo ativo* → cadastrar um ativo de teste | UC002 | Ativo aparece na lista |
| 4 | *Editar* o ativo de teste (ex.: mudar o setor) | UC003 | Alteração refletida na tabela |
| 5 | *Remover* o ativo de teste (confirmar) | UC004 | Confirmação pedida e ativo removido |
| 6 | *Importar cotações* → **Importar pasta inteira** → `dados/` | UC005 | 5 CSVs casados por ticker, 650 cotações importadas |
| 7 | Importar `dados/INVALIDO-exemplo.csv` | UC005 (fluxo alternativo) | Arquivo rejeitado por inteiro, erros apontados linha a linha, nenhuma cotação gravada |
| 8 | *Regras de análise* → *Editar parâmetros* de uma regra; *Ativar / desativar* | UC006 | Parâmetros e peso alterados; aviso para regerar recomendações |
| 9 | **Backtesting…** → *Executar backtesting* | UC020 | Sinais, taxa de acerto e retorno médio por regra |
| 10 | *Importar cotações* → **Sincronizar online** (só com internet) | UC019 | Baixa só o período faltante; sem rede, informa indisponibilidade e o resto segue funcionando (RNF008) |
| 11 | *Usuários* → *Novo usuário* / *Editar* / *Remover* | UC007 | CRUD de usuários com papel |
| 12 | *Recomendações* → **Gerar para todos os ativos** | UC011 | PETR4 **Compra**, ITUB4 **Neutro**, MGLU3 **Venda** |

> Se a sincronização online (passo 10) rodar antes do passo 12, as cotações novas
> podem mudar os pareceres. Para garantir os três resultados acima, gere as
> recomendações **antes** de sincronizar, ou pule o passo 10 sem internet.

### Bloco C — Investidor (5 min)

| # | Ação | Caso de uso | Resultado esperado |
|---|---|---|---|
| 13 | *Sistema → Trocar usuário* → entrar como investidor | UC001 | Navegação sem o bloco de administração |
| 14 | *Painel de análise* → escolher PETR4 e o período; **Analisar agora** | UC009 | Velas, médias móveis 9/21, cartões (fechamento, variação, RSI, volatilidade) e selo da recomendação |
| 15 | **Ver detalhes** | UC010 | Fundamentos, gráfico de linha, últimas cotações e parecer regra por regra |
| 16 | *Recomendações* → selecionar MGLU3 → **Abrir no painel** | UC012 | Recomendações vigentes com justificativa numérica |
| 17 | *Minha carteira* → **Comprar** | UC013 | Nova posição / preço médio recalculado |
| 18 | **Vender** parte de uma posição | UC014 | Quantidade da posição reduzida |
| 19 | Observar o resumo consolidado | UC015 | Valor de mercado e lucro/prejuízo por posição e total |
| 20 | *Alertas* → **Novo alerta** com preço abaixo do fechamento atual | UC016 | Alerta cadastrado como ativo |
| 21 | **Avaliar agora** | UC017 | Alerta dispara; notificação na barra de status (Observer) |
| 22 | **Arquivar** o alerta disparado | UC018 | Mensagem "Alerta arquivado." e estado atualizado na tabela |

### Fechamento (1 min)

- Mostrar *Ajuda → Sobre* (versão protótipo, equipe, padrões de projeto).
- Reforçar: as 20 telas/fluxos correspondem aos quadros UC001–UC020 do documento,
  e o aviso de que as recomendações não constituem recomendação de investimento.

## Se algo der errado

| Sintoma | O que fazer |
|---|---|
| Gráfico vazio no painel | Faltou importar as cotações (passo 6) |
| Recomendações diferentes das esperadas | Banco reaproveitado ou sincronização rodou antes: recriar o banco e repetir a partir do passo 6 |
| Sincronização falha | Sem internet: é o comportamento previsto (RNF008), seguir o roteiro |
| Erro de driver SQLite ao abrir | Instalar `libqt6sql6-sqlite` (ver `como-compilar-e-rodar.md`) |
