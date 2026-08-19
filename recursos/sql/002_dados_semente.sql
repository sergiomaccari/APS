-- =============================================================================
-- Analisador B3 - dados de semente (demonstracao)
--
-- Idempotente: todos os comandos usam INSERT OR IGNORE, apoiados nos indices
-- UNIQUE do esquema inicial. Pode ser executado a cada abertura do sistema.
--
-- Senhas: SHA-256 de (sal + senha), com sal fixo "aps-b3-2026", exatamente como
-- em ServicoAutenticacao::gerarHash().
--   admin@analisadorb3.com      -> admin123
--   investidor@analisadorb3.com -> investidor123
-- =============================================================================

INSERT OR IGNORE INTO usuario (nome, email, senha_hash, papel) VALUES
    ('Administrador do Sistema',
     'admin@analisadorb3.com',
     '4530d89349b3188c7dd80d2edf0b8fc5b5ea61a6650f35943c86a19296d11482',
     'Administrador'),
    ('Investidor de Demonstracao',
     'investidor@analisadorb3.com',
     '502e7f2bf3488cf83791f055f4e6b73ad7df2f01cc72de14690a0a031c8af111',
     'Investidor');

-- Tres acoes e um fundo imobiliario. Fundamentos aproximados, apenas para demonstracao.
INSERT OR IGNORE INTO ativo
    (ticker, nome_empresa, setor, tipo, preco_lucro, valor_mercado, dividend_yield,
     valor_patrimonial_por_cota, segmento)
VALUES
    ('PETR4', 'Petroleo Brasileiro S.A. - Petrobras', 'Petroleo, Gas e Biocombustiveis',
     'ACAO', 4.8, 495000000000.0, 12.4, NULL, NULL),
    ('VALE3', 'Vale S.A.', 'Mineracao',
     'ACAO', 6.2, 280000000000.0, 8.1, NULL, NULL),
    ('ITUB4', 'Itau Unibanco Holding S.A.', 'Bancos',
     'ACAO', 9.1, 320000000000.0, 6.3, NULL, NULL),
    ('HGLG11', 'CSHG Logistica Fundo de Investimento Imobiliario', 'Fundos Imobiliarios',
     'FII', NULL, NULL, 9.2, 168.40, 'Logistica');

-- Regras de analise (Strategy) com os parametros usados na apresentacao.
INSERT OR IGNORE INTO regra_configurada
    (nome_regra, ativa, parametro_principal, parametro_secundario)
VALUES
    ('Cruzamento de Medias Moveis', 1, 9,   21),   -- media curta / media longa (dias)
    ('Indice de Forca Relativa',    1, 30,  70),   -- sobrevenda / sobrecompra
    ('Dividend Yield',              1, 6,   10),   -- DY minimo / DY excelente (%)
    ('Preco sobre Lucro',           1, 8,   20);   -- P/L atrativo / P/L elevado

-- Carteira de exemplo do investidor de demonstracao.
INSERT OR IGNORE INTO carteira (usuario_id, nome, criada_em)
SELECT u.id, 'Carteira Principal', '2026-03-02'
FROM usuario u
WHERE u.email = 'investidor@analisadorb3.com';

INSERT OR IGNORE INTO posicao (carteira_id, ativo_id, quantidade, preco_medio, comprada_em)
SELECT c.id, a.id, 200, 36.90, '2026-03-05'
FROM carteira c
JOIN usuario u ON u.id = c.usuario_id AND u.email = 'investidor@analisadorb3.com'
JOIN ativo a ON a.ticker = 'PETR4'
WHERE c.nome = 'Carteira Principal';

INSERT OR IGNORE INTO posicao (carteira_id, ativo_id, quantidade, preco_medio, comprada_em)
SELECT c.id, a.id, 100, 61.25, '2026-04-10'
FROM carteira c
JOIN usuario u ON u.id = c.usuario_id AND u.email = 'investidor@analisadorb3.com'
JOIN ativo a ON a.ticker = 'VALE3'
WHERE c.nome = 'Carteira Principal';

INSERT OR IGNORE INTO posicao (carteira_id, ativo_id, quantidade, preco_medio, comprada_em)
SELECT c.id, a.id, 30, 158.70, '2026-05-20'
FROM carteira c
JOIN usuario u ON u.id = c.usuario_id AND u.email = 'investidor@analisadorb3.com'
JOIN ativo a ON a.ticker = 'HGLG11'
WHERE c.nome = 'Carteira Principal';
