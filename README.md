# Analisador B3

Sistema desktop de **análise de ativos da B3 e apoio à decisão de investimentos**,
com recomendações de compra, venda ou neutralidade **justificadas por regras
transparentes** — cada parecer mostra os números que levaram à conclusão.

Trabalho da disciplina **Análise e Projeto de Sistemas** (UTFPR).
Equipe: Sergio Roncato Maccari, Arthur Dal Bem Nunes, Leonardo Pereira Shibata e
Thales Romagna Fabrowski.

> As recomendações são apoio à decisão baseado em regras explícitas e **não
> constituem recomendação de investimento**.

## O que o sistema faz

**Administrador**
- Mantém o cadastro de ativos (ações e fundos imobiliários) com seus fundamentos.
- Importa séries históricas de cotações a partir de arquivos CSV no padrão
  brasileiro — de forma **transacional**: um erro estrutural rejeita o arquivo
  inteiro, e datas repetidas são ignoradas (importar duas vezes não duplica nada).
- **Sincroniza cotações online** (Yahoo Finance) com um clique: o sistema baixa
  apenas o período posterior à última cotação de cada ativo — o CSV continua
  sendo o caminho primário e tudo funciona offline sem a sincronização.
- Configura as regras de análise (liga, desliga, ajusta parâmetros e **pesos da
  média ponderada**) sem recompilar.
- Executa o **backtesting** das regras: reexecuta o motor em cada ponto do
  histórico e mede sinais, taxa de acerto e retorno médio por regra.
- Gerencia usuários e papéis.

**Investidor**
- Painel com gráfico de candles, médias móveis sobrepostas e cartões de
  indicadores (último fechamento, variação do período, RSI e volatilidade).
- Recomendação vigente por ativo, com a justificativa completa e o detalhamento
  regra por regra.
- Carteira com preço médio ponderado, valor de mercado e lucro/prejuízo.
- Alertas de preço que disparam quando a condição é satisfeita.

## Arquitetura

Monólito modular em camadas — cada camada só conhece a de baixo:

```
        ┌──────────────────────────────────────────────────────────┐
        │  src/ui/         Interface gráfica (Qt Widgets + Charts)  │
        │  9 telas · 4 diálogos · 3 widgets · JanelaPrincipal       │
        └───────────────────────────┬──────────────────────────────┘
                                    │ Contexto (injeção de dependência)
        ┌───────────────────────────▼──────────────────────────────┐
        │  src/servicos/   Regras de negócio                        │
        │  autenticação · ativos · importação · carteira ·          │
        │  alertas (Observer) · recomendações                       │
        └──────────────┬───────────────────────────┬───────────────┘
                       │                           │
        ┌──────────────▼──────────────┐ ┌──────────▼───────────────┐
        │  src/analise/               │ │  src/persistencia/        │
        │  indicadores + Strategy     │ │  Repository + Singleton   │
        │  (4 regras) + MotorAnalise  │ │  de conexão + migrações   │
        └──────────────┬──────────────┘ └──────────┬───────────────┘
                       │                           │
        ┌──────────────▼───────────────────────────▼───────────────┐
        │  src/dominio/    Entidades e regras próprias do objeto     │
        │  Ativo (abstrata) → Acao | FundoImobiliario · Cotacao ·    │
        │  Carteira/Posicao · Alerta · Recomendacao · Importacao     │
        └───────────────────────────┬──────────────────────────────┘
                                    │
                            SQLite (9 tabelas)
```

### Padrões de projeto aplicados

| Padrão | Onde | Para quê |
|---|---|---|
| **Strategy** | `analise/regraanalise.h` + 4 regras concretas | cada critério de análise é uma estratégia intercambiável; o motor não conhece nenhuma delas |
| **Strategy** (2ª aplicação) | `servicos/fontecotacoesremota.h` → `FonteYahooFinance` | a fonte da sincronização online é plugável; trocar de provedor não toca no serviço |
| **Observer** | `servicos/servicoalerta.h` ↔ `JanelaPrincipal` | o serviço notifica alertas disparados sem conhecer a interface |
| **Repository** | `src/persistencia/` | isola o SQL do resto do sistema |
| **Singleton** | `persistencia/bancodedados.h` | uma única conexão com o banco |
| **Herança + polimorfismo** | `Ativo` → `Acao` / `FundoImobiliario` | `tipo()` e `indiceValor()` variam por subclasse; as regras fundamentalistas usam `dynamic_cast` |

## Tecnologias

| Camada | Tecnologia | Por quê |
|---|---|---|
| Linguagem | **C++20** (orientado a objetos) | requisito do trabalho |
| Interface | **Qt 6 Widgets + Qt Charts** | formulários e gráficos (candlestick) no mesmo framework |
| Banco | **SQLite** via Qt Sql | arquivo único, sem servidor: a demonstração roda offline |
| Build | **CMake** ≥ 3.19 | padrão da comunidade C++ |
| Testes | **Qt Test** + ctest | 61 casos automatizados |

## Como compilar e executar

Guia detalhado (incluindo solução de problemas): [`docs/processos/como-compilar-e-rodar.md`](docs/processos/como-compilar-e-rodar.md).

```bash
# 1. Dependências (Ubuntu / WSL)
sudo apt install build-essential cmake qt6-base-dev qt6-charts-dev libqt6sql6-sqlite

# 2. Compilar
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# 3. Executar
./build/analisador-b3

# 4. Testes
ctest --test-dir build --output-on-failure
```

### Credenciais de demonstração

| Perfil | E-mail | Senha |
|---|---|---|
| Administrador | `admin@analisadorb3.com` | `admin123` |
| Investidor | `investidor@analisadorb3.com` | `investidor123` |

O menu **Sistema → Trocar usuário** permite alternar entre os perfis sem
reiniciar o programa.

### Roteiro de demonstração

1. Entre como **administrador** → *Importar cotações* → **Importar pasta inteira**
   e escolha `dados/`: os cinco CSVs são casados por ticker e importados.
2. Tente importar `dados/INVALIDO-exemplo.csv` para ver a **rejeição transacional**
   (três erros apontados por linha, nenhuma cotação gravada).
3. Com internet, clique em **Sincronizar online** na tela de importação: só o
   período faltante de cada ativo é baixado (rodar duas vezes não duplica nada).
4. Vá em *Regras de análise* para ver os critérios, parâmetros e pesos; em
   **Backtesting…**, execute a avaliação histórica das regras (sinais, taxa de
   acerto e retorno médio por regra).
5. Em *Recomendações* → **Gerar para todos os ativos**: PETR4 sai **Compra**
   (cruzamento de médias para cima) e MGLU3 sai **Venda** (múltiplo elevado,
   proventos baixos e tendência de queda).
6. **Trocar usuário** → entre como **investidor** → *Painel de análise*: gráfico,
   indicadores e a justificativa da recomendação.
7. *Alertas* → **Novo alerta** com preço abaixo do fechamento atual →
   **Avaliar agora**: o alerta dispara e a notificação aparece na barra de status.
8. *Minha carteira*: posições com lucro/prejuízo e o resumo consolidado.

## Estrutura de pastas

```
CMakeLists.txt              alvos: núcleo (biblioteca), aplicação e testes
dados/                      CSVs de demonstração (+ arquivo inválido proposital)
recursos/sql/               migrações versionadas embutidas no executável
src/dominio/                entidades e máquinas de estado
src/analise/                indicadores, regras (Strategy) e motor de análise
src/persistencia/           conexão, migrações e repositórios
src/servicos/               regras de negócio
src/ui/                     telas, diálogos e widgets
testes/                     testes automatizados (Qt Test)
docs/                       specs, ADRs, planos e processos do projeto
```

## Banco de dados

Nove tabelas: `usuario`, `ativo`, `cotacao`, `carteira`, `posicao`, `alerta`,
`recomendacao`, `regra_configurada` e `importacao` — com chaves estrangeiras,
índices e restrições `UNIQUE` (destaque para `cotacao(ativo_id, data)`, que
garante a idempotência da importação).

O arquivo é criado automaticamente na primeira execução, no diretório de dados do
usuário (`~/.local/share/UTFPR - APS/Analisador B3/analisador-b3.sqlite` no Linux),
e as migrações são aplicadas e registradas em `schema_versao`.

## Formato do CSV de cotações

```
Data;Abertura;Maxima;Minima;Fechamento;Volume
14/08/2026;38,42;38,90;38,10;38,75;41250300
```

Data `dd/mm/aaaa`, decimal com vírgula (aceita separador de milhar) e volume
inteiro. Validações por linha: número de colunas, data válida, preços positivos,
`máxima >= mínima` e abertura/fechamento dentro da faixa do dia.
