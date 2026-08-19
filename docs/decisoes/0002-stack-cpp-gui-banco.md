# ADR 0002 — Stack C++: GUI e banco de dados

- **Data:** 2026-08-18
- **Status:** aceita (aprovada pelo Sergio em 2026-08-18)

## Contexto

Restrição do projeto (inicio.txt item 7): **C++ orientado a objetos**. A disciplina
exige telas gráficas funcionais (1º bim, 20/10) e sistema completo com banco de
≥5 tabelas rodando (2º bim, 24/11). O tema proposto (ADR 0001) precisa de
formulários CRUD + gráficos financeiros (candlestick, linhas, médias móveis).

## Decisão (proposta)

- **GUI: Qt 6 (Widgets) + Qt Charts** — framework C++ maduro com formulários,
  tabelas, gráficos (inclusive candlestick) e integração SQL no mesmo pacote;
  visual profissional; documentação vasta; LGPL ok para uso acadêmico. A
  arquitetura natural (telas como classes, controllers, DAOs) espelha exatamente o
  que o dicionário de informações do professor pede.
- **Banco: SQLite via Qt SQL** — zero configuração no dia da apresentação (arquivo
  único, nada de servidor para subir no laboratório), transacional, SQL padrão,
  suporta com folga as 8+ tabelas. O modelo do professor aceita "MongoDB ou
  similares"; MUTEQ usou MySQL — SQLite cumpre o requisito com risco mínimo.
- **HTTP/JSON (atualização opcional de cotações): libcurl + nlohmann/json.**
- **Build: CMake** (padrão que o Sergio já usa no CyberMetro).

## Alternativas consideradas

- **Dear ImGui + ImPlot** — gráficos lindos e build simples, mas o paradigma
  *immediate mode* rende "telas" menos convencionais para formulários CRUD (a cara
  que o professor espera) e não traz camada SQL.
- **SFML** — ótimo para jogos (CyberMetro), mas não tem widgets/formulários; seria
  reinventar UI básica.
- **wxWidgets** — viável, porém sem módulo de charts equivalente e com menos
  documentação/momentum que Qt.
- **PostgreSQL** — mais impressionante, porém adiciona um servidor como ponto de
  falha na apresentação avaliada; ganho de nota nulo.

## Consequências

Positivo: um único framework cobre telas, gráficos e banco; demo sem dependências
externas; modelagem UML mapeia 1:1 no código. Negativo: curva de aprendizado do Qt
para a equipe (mitigada pela IA gerar exemplos/esqueleto) e binário maior.
