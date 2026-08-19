# Formato dos diagramas (JSON → PNG)

Os diagramas dos documentos são gerados pelo alvo `gerar-diagramas`:

```bash
cmake --build build --target gerar-diagramas
QT_QPA_PLATFORM=offscreen ./build/gerar-diagramas --lote docs/entrega/diagramas docs/entrega/figuras
```

Cada `.json` desta pasta vira um `.png` em `docs/entrega/figuras/` com o mesmo nome.
Exemplos completos: `seq-uc001.json` (sequência) e `est-uc017-alerta.json` (estados).

## Tipo "sequencia" — layout automático

```json
{
  "tipo": "sequencia",
  "moldura": "sd UC001",
  "participantes": [
    {"id": "u", "nome": ": Usuário", "ator": true},
    {"id": "tela", "nome": ": TelaLogin"}
  ],
  "mensagens": [
    {"de": "u", "para": "tela", "rotulo": "1: informar e-mail e senha"},
    {"de": "tela", "para": "tela", "rotulo": "1.1: validar()"},
    {"de": "tela", "para": "u", "rotulo": "resultado", "retorno": true}
  ]
}
```

- Participantes na ordem esquerda→direita; `"ator": true` desenha o boneco.
- Mensagem com `de == para` vira auto-chamada (lacete). `"retorno": true` = seta
  tracejada aberta. Numere os rótulos no padrão do professor (1, 1.1, 1.1.1…).
- Espaçamento entre linhas de vida se ajusta ao rótulo mais largo automaticamente.

## Tipo "grade" — elementos posicionados por (col, linha)

Cobre classes, objetos, casos de uso, estados, atividades, comunicação e DER.
`col`/`linha` são coordenadas lógicas (aceitam fração, ex.: `0.5`); cada valor
distinto vira uma "trilha" dimensionada pelo maior elemento nela.

```json
{
  "tipo": "grade",
  "moldura": "stm Alerta",
  "folgaColunas": 70, "folgaLinhas": 56,
  "elementos": [ ... ],
  "ligacoes": [ ... ]
}
```

### Tipos de elemento

| tipo | campos usados | desenha |
|---|---|---|
| `inicial` / `final` | — | bola cheia / alvo (estados e atividades) |
| `estado` | `nome` (negrito) + `texto` (ações internas, ex.: "do / login()") | retângulo arredondado com divisor |
| `acao` | `texto` | retângulo arredondado (atividade) |
| `decisao` | `texto` opcional | losango |
| `barra` | `largura` | barra de sincronização |
| `classe` | `nome`, `estereotipo`, `atributos[]`, `metodos[]` | caixa UML de 3 compartimentos (`<<abstract>>` deixa o nome em itálico) |
| `objeto` | `nome` ("petr4 : Acao"), `atributos[]` ("ticker = \"PETR4\"") | caixa com nome sublinhado |
| `casodeuso` | `texto` | elipse |
| `ator` | `nome` | boneco + nome |
| `nota` | `texto` | nota com canto dobrado |
| `tabela` | `nome`, `colunas[]` ("id INTEGER  [PK]") | tabela do DER |
| `texto` | `texto` | texto solto |

### Ligações

```json
{"de": "a", "para": "b", "tipo": "seta", "rotulo": "evento [guarda] / ação",
 "rotuloOrigem": "1", "rotuloDestino": "0..*", "via": [{"col": 1.5, "linha": 0.5}]}
```

| tipo | traço | ponta |
|---|---|---|
| `seta` (padrão) | sólido | seta aberta |
| `tracejada` | tracejado | seta aberta (use p/ `<<include>>`/`<<extend>>` com o estereótipo no `rotulo`) |
| `heranca` | sólido | triângulo vazado no destino (aponte PARA a superclasse) |
| `realizacao` | tracejado | triângulo vazado |
| `associacao` | sólido | sem ponta (multiplicidades em `rotuloOrigem`/`rotuloDestino`) |
| `agregacao` / `composicao` | sólido | losango vazado/cheio na ORIGEM (o todo) |

`via` desvia a linha por pontos de passagem (coordenadas de grade, frações valem).

## Convenções dos documentos

- Moldura: `uc` (casos de uso), `sd UCxxx` (sequência), `com UCxxx` (comunicação),
  `stm UCxxx | Classe` (estados), `act UCxxx | Classe | Estado` (atividades),
  `cd Nome` (classes), `od Nome` (objetos), `der Analisador B3` (banco).
- Comunicação: ator + objetos ligados por `seta` com o rótulo numerado
  ("1: autenticar(email, senha)"); mensagens múltiplas no mesmo par = rótulo multilinha.
- Nome de arquivo define a ordem no documento — siga a tabela abaixo.

## Inventário e nomes de arquivo

| Prefixo | Conteúdo | Quantos |
|---|---|---|
| `uc-casos-de-uso` | diagrama geral de casos de uso | 1 |
| `cls-dominio`, `cls-analise`, `cls-arquitetura` | classes: domínio (herança Ativo), Strategy/Motor, camadas UI→serviços→repositórios | 3 |
| `obj-analise`, `obj-carteira` | objetos/instâncias | 2 |
| `seq-uc001` … `seq-uc018` | sequência, um por UC | 18 |
| `com-uc001` … `com-uc018` | comunicação, um por UC | 18 |
| `der-banco` | entidade-relacionamento (9 tabelas) | 1 |
| `est-ucNNN-<classe>` | estados (2 classes por UC + domínio onde indicado) | 40 |
| `atv-ucNNN-<classe>` | atividades (o estado principal de cada diagrama de estados) | 40 |

### Classes escolhidas por UC (estados e atividades)

| UC | Fronteira | Controle | Domínio (extra) |
|---|---|---|---|
| UC001 | TelaLogin | ServicoAutenticacao | — |
| UC002 | DialogoAtivo | ServicoAtivo | — |
| UC003 | TelaAtivos | ServicoAtivo | — |
| UC004 | TelaAtivos | RepositorioAtivo | — |
| UC005 | TelaImportacao | ServicoImportacao | **Importacao** |
| UC006 | TelaRegras | RepositorioRegra | — |
| UC007 | TelaUsuarios | ServicoAutenticacao | — |
| UC008 | TelaAtivos | RepositorioAtivo | — |
| UC009 | TelaDashboard | CalculadoraIndicadores | — |
| UC010 | TelaDetalheAtivo | RepositorioCotacao | — |
| UC011 | TelaRecomendacoes | MotorAnalise | **Recomendacao** |
| UC012 | TelaRecomendacoes | RepositorioRecomendacao | — |
| UC013 | DialogoOperacao | ServicoCarteira | — |
| UC014 | TelaCarteira | ServicoCarteira | **Posicao** |
| UC015 | TelaCarteira | ServicoCarteira | — |
| UC016 | DialogoAlerta | ServicoAlerta | — |
| UC017 | JanelaPrincipal | ServicoAlerta | **Alerta** |
| UC018 | TelaAlertas | RepositorioAlerta | — |

Título dentro do arquivo (`moldura`) e nome do arquivo em minúsculas, ex.:
`est-uc005-importacao.json` → moldura `stm UC005 | Importacao`.
