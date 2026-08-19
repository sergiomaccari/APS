# ADR 0001 — Tema do projeto da disciplina

- **Data:** 2026-08-18
- **Status:** aceita (aprovada pelo Sergio em 2026-08-18)

## Contexto

A disciplina exige um sistema com: cliente entrevistado (4 blocos de questões),
RF/RNF/RP/REU, casos de uso especificados em quadros, banco com **mínimo 5 tabelas**,
dicionário de informações volumoso (veteranos citam meta de **30+ atributos**),
diagramas UML completos (classes, objetos, sequência; depois comunicação, DER,
estados, atividades), **telas gráficas funcionais** no 1º bim e **sistema rodando**
no 2º. Restrição do Sergio (inicio.txt item 7): **C++ orientado a objetos**.

A ideia inicial da equipe: sistema que avalia empresas da B3 e dá recomendações de
compra/venda, com gráficos e métricas por intervalos de tempo.

Referências de régua: MUTEQ (exemplo do professor — catálogo de museu com
Administrador/Visitante, cliente real entrevistado) e SOL (veteranos — reserva de
salas, cliente fictício UTFPR, forte em volume documental e fraco em profundidade
analítica; ver `docs/analises/veterano-sol.md`).

## Decisão (proposta)

**Tema A — "Analisador B3": sistema de análise e apoio à decisão de investimentos
na B3**, refinado da ideia original:

- Atores: **Administrador** (gerencia ativos, importa cotações, cadastra regras de
  análise, gerencia usuários) e **Investidor** (busca ativos, monta carteira, cria
  alertas, visualiza gráficos/métricas e recomendações explicadas).
- O motor de recomendação é **baseado em regras transparentes e configuráveis**
  (ex.: cruzamento de médias móveis, P/L relativo ao setor, dividend yield mínimo) —
  cada recomendação exibe o *porquê*. Nada de promessa preditiva/ML: enquadrado como
  "apoio à decisão", com disclaimer. Em POO isso vira **padrão Strategy** — ouro
  para o diagrama de classes.
- Dados: **importação de séries históricas em CSV da B3** (demo 100% offline, sem
  risco na apresentação) + atualização opcional via API pública (libcurl +
  nlohmann/json).
- Ciclos de vida ricos para os diagramas de estados **de domínio** (onde os
  veteranos falharam): `Alerta` (ATIVO→DISPARADO→ARQUIVADO), `Recomendacao`
  (GERADA→VIGENTE→EXPIRADA/INVALIDADA), `ImportacaoDeDados`
  (PENDENTE→VALIDANDO→CONCLUIDA/REJEITADA).
- Banco com 8+ tabelas naturais: usuario, ativo, cotacao, indicador, regra,
  recomendacao, carteira, posicao, alerta.

## Alternativas consideradas

- **B — Sistema de gestão para cliente real acessível** (agenda/estoque/OS de um
  pequeno negócio conhecido): entrevista real fácil e rito da disciplina
  sob medida — porém genérico, pouco diferenciado dos veteranos, gráficos/métricas
  pobres e menos motivador. Fica como plano B seguro.
- **C — Simulador de carteira/paper trading com ranking**: ciclo de vida de `Ordem`
  riquíssimo e demo divertida — porém escopo maior que A (book de ordens,
  concorrência entre usuários) sem ganho proporcional de nota. Elementos dele
  (carteira simulada) podem ser absorvidos pelo tema A como "trabalho futuro".

## Riscos do tema A e mitigações

1. **Cliente da entrevista não é óbvio** (MUTEQ tinha o diretor do museu). Mitigação:
   cliente fictício é aceito (SOL usou a UTFPR); melhor ainda, usar um investidor
   pessoa física real do círculo da equipe ou enquadrar um "clube de investimentos"
   — decidir com a equipe e preparar as ~28 respostas da entrevista.
2. **Expectativa de qualidade preditiva**: mitigada pelo enquadramento em regras
   transparentes + disclaimer.
3. **Gráficos em C++ dão mais trabalho que na web**: mitigado pela escolha de
   biblioteca com charts prontos (ver ADR 0002).
4. **Escopo**: teto de ~14 casos de uso (MUTEQ teve 17, SOL 12).

## Consequências

Positivo: tema diferenciado, aderente ao perfil da equipe, rico em POO (Strategy,
Observer p/ alertas, Repository/DAO, herança de Ativo), demo impressionante com
dados reais, artefatos UML de domínio com substância. Negativo: exige preparar
cliente/entrevista com cuidado e disciplina de escopo.
