# ADR 0004 — Protótipo de telas por stubs em tempo de link, numa branch órfã

- **Data:** 2026-10-12
- **Status:** aceita

## Contexto
A branch `13/10-prototipo` (06/10) entregava o sistema completo rotulado como
protótipo. A equipe entendeu que o 1º bimestre deve mostrar "uma ideia de
frontend navegável", não o sistema final, e que o histórico público da branch
não deve revelar o sistema pronto. Prazo: documento em 13/10, telas em 20/10.

## Decisão
1. Mesmo app Qt, novo alvo `analisador-b3-prototipo` que compila a interface e o
   domínio sobre `src/prototipo/` — implementações em memória dos mesmos
   headers de serviços, repositórios e análise. A UI não muda.
2. Fidelidade "dados fixos + estado em memória na sessão": CRUDs, compra/venda e
   alertas funcionam até fechar; importação, recomendações e backtesting
   devolvem resultado pré-pronto; sincronização online fica "prevista para a
   versão final". Única conta real: MMS/IFR para desenhar o gráfico.
3. Branch órfã `entrega-13-10` (git não aceita espaço em "Entrega 13/10")
   gerada por script a partir de `main`, só com protótipo e documentos do 1º
   bimestre. `13/10-prototipo` é apagada local e no GitHub após a revisão do
   Thales ser absorvida em `main` por fast-forward.

## Alternativas consideradas
- Árvore separada só com UI mantida à mão — duplica a UI até 17/11.
- Maquete HTML clicável — foge do Qt e vale menos como "telas funcionais em
  modo gráfico".
- Interfaces abstratas injetadas — mexe em ~60 chamadas da UI e no `Contexto`.
- Branch normal sobre `main` — histórico público continuaria mostrando o
  sistema completo.

## Consequências
- O protótipo passa a ser um alvo permanente do build; mudanças de header em
  `main` que quebrem os stubs aparecem na hora.
- O repositório de entrega não depende de SQLite nem de rede; compila só com
  Qt Widgets/Charts.
- O Thales precisa trocar de branch; o doc da equipe explica.
- Os documentos do 1º bimestre não mudam nesta decisão; as pendências da
  revisão de 12/10 seguem com o Sergio.
