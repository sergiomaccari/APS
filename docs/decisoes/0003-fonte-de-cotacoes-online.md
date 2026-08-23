# ADR 0003 — Fonte da sincronização online de cotações

- **Status:** aceita (Sergio, 19/08/2026)
- **Contexto:** o RF020 exige baixar cotações diárias faltantes com um clique,
  como complemento à importação CSV (RNF001 preservado: offline continua
  funcionando; RNF008: a sincronização é opcional e falha graciosamente).

## Alternativas consideradas

| Fonte | Prós | Contras |
|---|---|---|
| **Arquivos COTAHIST (B3)** | oficial, gratuito, formato estável | vem em ZIP; o Qt 6 não expõe descompactação de ZIP na API pública — exigiria dependência extra (viola RP004) ou um inflator próprio; um arquivo por dia |
| **brapi.dev** | API brasileira, JSON simples | exige token por usuário — atrito para a equipe e para a banca |
| **Yahoo Finance (chart API v8)** | JSON limpo, sem token, um request por ativo com intervalo arbitrário (`period1/period2`), tickers B3 com sufixo `.SA` | **não oficial** — pode mudar sem aviso |

## Decisão

**Yahoo Finance chart API v8**, isolada atrás da interface `FonteCotacoesRemota`
(implementação `FonteYahooFinance`). O risco de instabilidade é aceito porque:
(1) a importação CSV permanece o caminho primário e oficial do sistema;
(2) a interface permite trocar a fonte (ex.: brapi) sem tocar no serviço;
(3) sem rede ou com a API fora do ar, o sistema apenas informa e segue.

## Consequências

- `QT_QPA`? não; requer módulo **Qt Network** (continua dentro do RP004) e
  backend TLS presente na máquina (verificado em tempo de execução com
  mensagem clara quando ausente).
- Cada sincronização é registrada na tabela `importacao` (auditoria única para
  CSV e online).
- Fundamentos (P/L, DY, VP/cota) não vêm nessa API — seguem mantidos manualmente
  pelo administrador.
