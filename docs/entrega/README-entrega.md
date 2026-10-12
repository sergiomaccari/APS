# Analisador B3 — Protótipo de telas (1º bimestre)

Protótipo de telas do **Analisador B3**, sistema desktop de análise de ativos da
B3 e apoio à decisão de investimentos, com recomendações de compra, venda ou
neutralidade justificadas por regras transparentes.

Entrega do **1º bimestre** da disciplina **Análise e Projeto de Sistemas** (UTFPR):
Plano do Projeto (13/10/2026) e Testes do Protótipo (20/10/2026).

Equipe: Sergio Roncato Maccari, Arthur Dal Bem Nunes, Leonardo Pereira Shibata e
Thales Romagna Fabrowski.

> As recomendações são apoio à decisão baseado em regras explícitas e **não
> constituem recomendação de investimento**.

## O que o protótipo faz

- Apresenta **as telas descritas no documento** (capítulo 4, Figuras 2 a 13),
  em C++ com Qt 6, navegáveis nos 20 casos de uso (UC001–UC020).
- Trabalha sobre **dados de demonstração fixos**: 2 usuários, 5 ativos (PETR4,
  VALE3, ITUB4, MGLU3 e o fundo imobiliário HGLG11), 4 regras de análise, a
  carteira e 2 alertas do investidor, 5 recomendações e as cotações de `dados/`
  (embutidas no executável).
- Mantém o **estado em memória durante a sessão**: cadastrar, editar e remover
  ativos, usuários e regras; comprar e vender; criar, disparar e arquivar alertas.
  As telas refletem cada mudança até o programa ser fechado.
- Desenha o gráfico de velas com médias móveis 9/21, IFR e volatilidade a partir
  das cotações de demonstração.
- Notifica alertas disparados na barra de status (padrão Observer): ao entrar como
  investidor, o alerta de PETR4 já dispara.

## O que o protótipo não faz (fica para a versão final, 2º bimestre)

- **Não grava nada em disco.** Ao fechar o programa, tudo volta ao estado inicial.
- **Importação de cotações** mostra um resultado pré-pronto (650 cotações em
  5 arquivos, `INVALIDO-exemplo.csv` rejeitado) e registra a linha no histórico,
  mas não lê nem valida os arquivos.
- **Gerar recomendações** devolve as 5 recomendações de demonstração (PETR4,
  VALE3 e HGLG11 Compra; ITUB4 Neutro; MGLU3 Venda), sem executar o motor de
  análise.
- **Backtesting** devolve uma tabela fixa por regra.
- **Sincronização online** está prevista para a versão final: o botão apenas
  informa isso, sem acessar a rede.

## Requisitos

- **Qt 6.4 ou superior** com os módulos **Widgets** e **Charts** (não precisa de
  Qt Sql nem Qt Network).
- **CMake 3.19** ou superior.
- Compilador com suporte a **C++20** (GCC 10+, Clang 12+ ou MSVC 2019+).

No Ubuntu / WSL:

```bash
sudo apt install build-essential cmake qt6-base-dev qt6-charts-dev
```

Em outros sistemas, instale o Qt pelo instalador oficial e informe o caminho na
configuração com `-DCMAKE_PREFIX_PATH=<pasta do Qt>`.

## Como compilar e executar

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DAPENAS_PROTOTIPO=ON
cmake --build build -j
./build/analisador-b3-prototipo
```

O título da janela termina em "(protótipo)" e *Ajuda → Sobre* identifica a versão.

O alvo `capturar-telas-prototipo` gera um PNG de cada tela (útil para conferir
que tudo renderiza, inclusive sem interface gráfica):

```bash
QT_QPA_PLATFORM=offscreen ./build/capturar-telas-prototipo capturas
```

## Credenciais de demonstração

| Perfil | E-mail | Senha |
|---|---|---|
| Administrador | `admin@analisadorb3.com` | `admin123` |
| Investidor | `investidor@analisadorb3.com` | `investidor123` |

Qualquer outra combinação mostra "E-mail ou senha inválidos.". O menu
**Sistema → Trocar usuário** alterna entre os perfis sem reiniciar o programa.

## Mapa tela → casos de uso

| Tela (item da navegação) | Fontes em `src/ui/` | Casos de uso | Figura |
|---|---|---|---|
| Login | `telalogin` | UC001 Fazer login | 2 |
| Janela principal (navegação por papel, barra de status) | `janelaprincipal` | UC001 Fazer login · UC017 Avaliar e disparar alertas (notificação) | 3 e 4 |
| Painel de análise | `teladashboard`, `widgets/graficocandlestick`, `widgets/cartaoindicador` | UC008 Buscar ativos · UC009 Visualizar painel de análise do ativo · UC011 Gerar recomendações (**Analisar agora**) | 5 |
| Detalhes do ativo | `teladetalheativo`, `widgets/graficolinha` | UC010 Visualizar detalhes do ativo | 6 |
| Recomendações | `telarecomendacoes` | UC011 Gerar recomendações · UC012 Visualizar recomendações vigentes | 7 |
| Minha carteira | `telacarteira`, `dialogos/dialogooperacao` | UC013 Comprar ativo · UC014 Vender ativo · UC015 Visualizar resultado da carteira | 8 |
| Alertas | `telaalertas`, `dialogos/dialogoalerta` | UC016 Cadastrar alerta · UC017 Avaliar e disparar alertas · UC018 Arquivar alerta | 9 |
| Ativos (administração) | `telaativos`, `dialogos/dialogoativo` | UC002 Cadastrar ativo · UC003 Atualizar ativo · UC004 Remover ativo · UC008 Buscar ativos | 10 |
| Importar cotações (administração) | `telaimportacao` | UC005 Importar cotações · UC019 Sincronizar cotações online | 11 |
| Regras de análise (administração) | `telaregras`, `dialogos/dialogoregra`, `dialogos/dialogobacktest` | UC006 Configurar regra de análise · UC020 Executar backtesting das regras | 12 |
| Usuários (administração) | `telausuarios`, `dialogos/dialogousuario` | UC007 Gerenciar usuários | 13 |

## Documentos

- **Plano do Projeto (documento do 1º bimestre):**
  [`docs/entrega/documento-1-bimestre.pdf`](docs/entrega/documento-1-bimestre.pdf)
  (também em [`.docx`](docs/entrega/documento-1-bimestre.docx)).
- **Roteiro da apresentação de 20/10** (UC001–UC020 na ordem da demonstração,
  checklist e contingências):
  [`docs/processos/roteiro-apresentacao-1.md`](docs/processos/roteiro-apresentacao-1.md).

## Estrutura de pastas

```
CMakeLists.txt              alvos analisador-b3-prototipo e capturar-telas-prototipo
dados/                      CSVs de demonstração (+ arquivo inválido proposital)
recursos/prototipo.qrc      cotações de demonstração embutidas no executável
src/ui/                     telas, diálogos e widgets
src/dominio/                entidades (Ativo → Acao | FundoImobiliario, Carteira, Alerta...)
src/servicos/, src/persistencia/, src/analise/
                            interfaces das camadas (apenas cabeçalhos)
src/prototipo/              implementação em memória dessas interfaces, com os
                            dados de demonstração
ferramentas/                utilitário de captura das telas
docs/entrega/               documento do 1º bimestre, seções, figuras e diagramas
docs/processos/             roteiro da apresentação
```
