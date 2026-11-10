-- =============================================================================
-- Analisador B3 - migracao 003: ativo de demonstracao para o cenario de VENDA
--
-- Motivo: com os quatro ativos iniciais nenhum parecer consolidado fechava em
-- Venda (as regras fundamentalistas puxavam a media para cima). Este ativo tem
-- multiplo elevado (P/L 28) e proventos baixos (DY 0,4%), o que, somado a serie
-- em queda de dados/MGLU3.csv, produz recomendacao de Venda - permitindo
-- demonstrar os tres resultados possiveis: Compra, Neutro e Venda.
--
-- Idempotente: INSERT OR IGNORE apoiado no indice UNIQUE de ativo.ticker.
-- =============================================================================

INSERT OR IGNORE INTO ativo
    (ticker, nome_empresa, setor, tipo, preco_lucro, valor_mercado, dividend_yield,
     valor_patrimonial_por_cota, segmento)
VALUES
    ('MGLU3', 'Magazine Luiza S.A.', 'Comercio Varejista',
     'ACAO', 28.0, 12500000000.0, 0.4, NULL, NULL);
