# Protótipo (1º bimestre) × Versão final (2º bimestre)

> Para: Arthur, Leo e Thales. Escrito em 12/10/2026, véspera da entrega do 1º
> bimestre. Leitura de 10 minutos. Referências aprovadas: spec
> [`0002`](../specs/0002-prototipo-de-telas.md), design
> [`0001`](../design/0001-prototipo-de-telas-stubs.md) e ADR
> [`0004`](../decisoes/0004-prototipo-por-stubs-em-branch-orfa.md).

## 1. Resumo

- A branch antiga `13/10-prototipo` entregava o **sistema completo** (banco,
  motor de análise, sincronização, 88 testes) com o título "protótipo". Isso
  não é o que o 1º bimestre pede.
- O 1º bimestre pede **telas funcionais em modo gráfico, representando os casos
  de uso**. Agora entregamos só isso: um frontend navegável, com dados fixos.
- A entrega do 1º bimestre está na branch **`entrega-13-10`**: só o protótipo e
  os documentos do 1º bimestre, sem o histórico do sistema completo.
- O sistema completo continua em **`main`** e é o entregável do 2º bimestre
  (17/11 e validação em 24/11).
- A branch `13/10-prototipo` foi apagada. Quem já tinha puxado:
  `git fetch -p && git checkout entrega-13-10`.

## 2. Comparação por aspecto

| Aspecto | Protótipo (20/10) | Versão final (24/11) |
|---|---|---|
| Telas | As mesmas 9 telas + login + 6 diálogos da versão final. Título e *Ajuda → Sobre* marcados "(protótipo)". | As mesmas telas. |
| Login | Aceita só as 2 contas de demonstração. Senha errada mostra o mesmo erro do sistema real. | Usuários no banco, senha com hash, qualquer usuário cadastrado entra. |
| Dados | Fixos, em memória: 2 usuários, 5 ativos, 4 regras, carteira do investidor, 2 alertas, 5 recomendações e as cotações dos CSVs de `dados/` embutidas no programa. | SQLite persistente: 9 tabelas criadas por 4 migrações na primeira execução. |
| Cadastro de ativos e usuários | Cadastrar, editar e remover funcionam **em memória**. Some ao fechar. | Mesmo fluxo, gravado no banco, com validações. |
| Regras de análise | Editar parâmetros, peso e ativar/desativar funcionam em memória. | Gravado no banco e usado de verdade pelo motor de análise. |
| Carteira | Comprar e vender alteram a posição e o resumo recalcula na sessão (ex.: PETR4 vai de 200 para 210). Ao reabrir, volta ao original. | Posições gravadas; resultado calculado sobre as cotações importadas/sincronizadas. |
| Alertas | Cadastrar e arquivar em memória. O disparo é real: usa a mesma condição do domínio (`Alerta`) sobre os dados fixos e avisa a janela pelo padrão Observer. Ao entrar como investidor, o alerta de PETR4 dispara. | Mesmo Observer, com alertas e mudanças de estado gravados no banco. |
| Importação de CSV | Devolve um **resumo pronto** (650 cotações, 5 arquivos aceitos, `INVALIDO-exemplo.csv` rejeitado) e acrescenta uma linha no histórico. Nenhuma cotação muda. | Lê o arquivo, valida linha a linha e grava numa transação: arquivo com erro é rejeitado inteiro. |
| Recomendações | Devolve as **5 recomendações fixas** (PETR4 Compra, VALE3 Compra, HGLG11 Compra, ITUB4 Neutro, MGLU3 Venda) com justificativa. | `MotorAnalise` aplica as 4 regras (padrão Strategy) e faz a média ponderada pelos pesos. |
| Indicadores e gráficos | Velas, MMS 9/21, IFR e volatilidade desenhados a partir das cotações fixas. É a **única conta** feita no protótipo, só para traçar as linhas. | `CalculadoraIndicadores` completa sobre o histórico real do banco. |
| Backtesting | Tabela fixa por regra, sem cálculo. | `Backtester` roda as regras sobre o histórico e mede sinais, taxa de acerto e retorno médio. |
| Sincronização online | Mostra a mensagem de que está **prevista para a versão final**. Não acessa a rede. | Baixa do Yahoo Finance só o período que falta de cada ativo, sem duplicar. |
| Ao fechar o programa | Tudo volta ao estado inicial. Nenhum arquivo é gravado. | Tudo fica salvo no SQLite. |
| Testes automatizados | Não tem. | 5 alvos, 88 casos (`ctest`). |
| Dependências Qt | Widgets e Charts. | Widgets, Charts, **Sql e Network**. |
| O que o professor avalia | 20/10, 2,0 pts: "telas funcionais em modo gráfico, representando os casos de uso". | 24/11, 2,0 pts: "requisitos atendidos pelos casos de uso" e "diagramas de sequência". Sem sistema funcionando, a parte escrita (8,0) **não é corrigida**. |

## 3. Comparação por caso de uso

Legenda do protótipo: **navega** = tela abre e mostra dados fixos ·
**memória** = funciona, mas some ao fechar · **pronto** = devolve resultado
pré-montado · **versão final** = mostra aviso de que vem no 2º bimestre.

| UC | Nome | Protótipo | O que a versão final faz a mais |
|---|---|---|---|
| UC001 | Fazer login | memória (2 contas fixas) | Usuários do banco, senha com hash |
| UC002 | Cadastrar ativo | memória | Grava no banco; ticker único validado contra a base |
| UC003 | Atualizar ativo | memória | Grava no banco |
| UC004 | Remover ativo | memória | Remove do banco junto com as dependências |
| UC005 | Importar cotações | pronto (resumo fixo + linha no histórico) | Parse e validação reais, transação, rejeição do arquivo inválido de verdade |
| UC006 | Configurar regra de análise | memória | Parâmetros e peso passam a mudar as recomendações |
| UC007 | Gerenciar usuários | memória | Grava no banco, senha com hash |
| UC008 | Buscar ativos | memória (filtra os ativos da sessão) | Busca na base |
| UC009 | Visualizar painel de análise | navega, com gráfico e indicadores calculados sobre as cotações fixas | Indicadores sobre o histórico real, atualizado por importação/sincronização |
| UC010 | Visualizar detalhes do ativo | navega | Fundamentos e histórico vindos do banco |
| UC011 | Gerar recomendações | pronto (5 fixas) | Motor de análise com 4 regras e média ponderada |
| UC012 | Visualizar recomendações vigentes | navega (5 fixas, com justificativa por regra) | Recomendações geradas e gravadas, com histórico |
| UC013 | Comprar ativo | memória | Grava a posição no banco e recalcula o preço médio |
| UC014 | Vender ativo | memória | Atualiza a posição no banco |
| UC015 | Visualizar resultado da carteira | memória (recalcula na sessão) | Resultado sobre cotações atualizadas e posições gravadas |
| UC016 | Cadastrar alerta | memória | Grava no banco |
| UC017 | Avaliar e disparar alertas | funciona (condição real do domínio + Observer, sobre dados fixos) | Mesmo mecanismo, com cotações reais e estado gravado |
| UC018 | Arquivar alerta | memória | Grava no banco |
| UC019 | Sincronizar cotações online | versão final | Yahoo Finance, só o período faltante |
| UC020 | Executar backtesting | pronto (tabela fixa) | `Backtester` real sobre o histórico |

## 4. Como o protótipo foi construído

As telas do protótipo **são as mesmas** da versão final. Os arquivos de
`src/ui/` não foram copiados nem reescritos. A única diferença visível é a
marcação "(protótipo)" no título e em *Ajuda → Sobre*.

O sistema completo tem quatro camadas: interface (`src/ui`), domínio
(`src/dominio`: `Ativo → Acao/FundoImobiliario`, `Carteira`, `Posicao`,
`Alerta`, `Recomendacao`...), análise (`src/analise`: `MotorAnalise`,
`CalculadoraIndicadores`, `Backtester` e as 4 regras) e a parte de dados
(`src/persistencia`: SQLite e 8 repositórios; `src/servicos`: 7 serviços e
`FonteYahooFinance`).

No protótipo, as camadas de análise, persistência e serviços foram trocadas por
**"dublês" em memória**, na pasta `src/prototipo/`. Eles têm os mesmos nomes de
classe e os mesmos métodos que a interface chama, mas por baixo só leem e
escrevem num conjunto de dados fixos. A escolha é feita na hora de compilar: o
alvo `analisador-b3-prototipo` junta interface + domínio + dublês; o alvo
`analisador-b3` junta interface + domínio + camadas reais.

Consequência prática: **nada que se fizer nas telas até 17/11 se perde**. A
mesma tela serve aos dois executáveis. E se alguém mudar um método das camadas
reais sem ajustar o dublê, o build do protótipo quebra na hora e avisa.

## 5. Como rodar as duas versões

Credenciais (nas duas): administrador `admin@analisadorb3.com` / `admin123`;
investidor `investidor@analisadorb3.com` / `investidor123`.

**Protótipo, pela branch da entrega** (o que o professor recebe):

```bash
git fetch -p
git checkout entrega-13-10
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DAPENAS_PROTOTIPO=ON
cmake --build build -j
./build/analisador-b3-prototipo
```

**Protótipo e versão final, pela `main`** (o build padrão gera os dois):

```bash
git checkout main
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/analisador-b3-prototipo     # protótipo
./build/analisador-b3               # versão final
ctest --test-dir build --output-on-failure   # 5 alvos, 88 casos
```

A versão final grava o banco em
`~/.local/share/"UTFPR - APS"/"Analisador B3"/analisador-b3.sqlite`. Para
começar uma demonstração do zero, apague esse arquivo antes. O protótipo não
grava nada. Dependências e solução de problemas:
[`como-compilar-e-rodar.md`](como-compilar-e-rodar.md). Roteiro da apresentação
de 20/10: [`roteiro-apresentacao-1.md`](roteiro-apresentacao-1.md).

## 6. Se o professor perguntar "o que falta para a versão final?"

- "As telas já estão na tecnologia definitiva, C++ com Qt 6. O que entra agora
  são as camadas de baixo: banco SQLite, motor de análise e serviços."
- "Hoje os dados são fixos e somem ao fechar. Na versão final tudo fica gravado
  no banco, com 9 tabelas."
- "A importação de CSV vai validar cada linha e gravar numa transação: se um
  arquivo tem erro, ele é rejeitado por inteiro."
- "As recomendações vão sair do motor de análise: 4 regras independentes
  (padrão Strategy) com média ponderada por peso, e cada uma explica o porquê."
- "O backtesting vai rodar as regras sobre o histórico real, e a sincronização
  online vai baixar do Yahoo Finance só o período que falta."
- "Vamos validar tudo com testes automatizados e com o roteiro dos 20 casos de
  uso, que é o que será avaliado em 24/11."

> ⚠️ Atenção: o capítulo 4 e as conclusões do documento do 1º bimestre ainda
> dizem que o protótipo foi construído "na tecnologia definitiva (C++ com Qt 6 e
> SQLite)" e "calculando indicadores reais". O protótipo entregue é C++/Qt 6,
> mas **sem SQLite** e com resultados pré-prontos. Se o professor notar, a
> resposta é a do primeiro bullet acima. Ajustar o texto é decisão do Sergio.
