-- =============================================================================
-- Analisador B3 - esquema inicial do banco de dados (SQLite)
-- Trabalho de Analise e Projeto de Sistemas (UTFPR)
--
-- Nove tabelas (o trabalho exige no minimo cinco):
--   usuario           - quem acessa o sistema (Investidor ou Administrador)
--   ativo             - ativos negociados na B3; coluna "tipo" e o discriminador
--                       da heranca Ativo -> (Acao | FundoImobiliario)
--   cotacao           - serie historica diaria (OHLCV) de cada ativo
--   carteira          - carteira de investimentos de um usuario
--   posicao           - posicao de um ativo dentro de uma carteira
--   alerta            - condicao monitorada por um usuario sobre um ativo
--   recomendacao      - saida do motor de analise para um ativo
--   regra_configurada - parametros das estrategias (Strategy) de analise
--   importacao        - historico das importacoes de CSV (auditoria)
--
-- Convencoes: chaves estrangeiras declaradas com ON DELETE CASCADE onde a
-- existencia do filho depende do pai; colunas de estado guardam exatamente os
-- textos devolvidos pelas funcoes paraTexto() do dominio.
-- =============================================================================

CREATE TABLE IF NOT EXISTS usuario (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    nome       TEXT    NOT NULL,
    email      TEXT    NOT NULL,
    senha_hash TEXT    NOT NULL,
    papel      TEXT    NOT NULL DEFAULT 'Investidor'
                       CHECK (papel IN ('Investidor', 'Administrador'))
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_usuario_email ON usuario (email);

CREATE TABLE IF NOT EXISTS ativo (
    id                          INTEGER PRIMARY KEY AUTOINCREMENT,
    ticker                      TEXT    NOT NULL,
    nome_empresa                TEXT    NOT NULL,
    setor                       TEXT    NOT NULL DEFAULT '',
    tipo                        TEXT    NOT NULL CHECK (tipo IN ('ACAO', 'FII')),
    -- Campos de Acao (nulos quando tipo = 'FII')
    preco_lucro                 REAL,
    valor_mercado               REAL,
    -- Campo comum as duas subclasses
    dividend_yield              REAL    NOT NULL DEFAULT 0,
    -- Campos de FundoImobiliario (nulos quando tipo = 'ACAO')
    valor_patrimonial_por_cota  REAL,
    segmento                    TEXT
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_ativo_ticker ON ativo (ticker);

CREATE TABLE IF NOT EXISTS cotacao (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    ativo_id   INTEGER NOT NULL,
    data       TEXT    NOT NULL,          -- ISO 8601 (yyyy-MM-dd)
    abertura   REAL    NOT NULL,
    maxima     REAL    NOT NULL,
    minima     REAL    NOT NULL,
    fechamento REAL    NOT NULL,
    volume     INTEGER NOT NULL DEFAULT 0,
    FOREIGN KEY (ativo_id) REFERENCES ativo (id) ON DELETE CASCADE
);

-- Impede cotacao duplicada do mesmo ativo no mesmo dia (base da importacao idempotente).
CREATE UNIQUE INDEX IF NOT EXISTS idx_cotacao_ativo_data ON cotacao (ativo_id, data);

CREATE TABLE IF NOT EXISTS carteira (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    usuario_id INTEGER NOT NULL,
    nome       TEXT    NOT NULL,
    criada_em  TEXT    NOT NULL,
    FOREIGN KEY (usuario_id) REFERENCES usuario (id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_carteira_usuario ON carteira (usuario_id);

CREATE TABLE IF NOT EXISTS posicao (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    carteira_id  INTEGER NOT NULL,
    ativo_id     INTEGER NOT NULL,
    quantidade   INTEGER NOT NULL CHECK (quantidade > 0),
    preco_medio  REAL    NOT NULL CHECK (preco_medio > 0),
    comprada_em  TEXT    NOT NULL,
    FOREIGN KEY (carteira_id) REFERENCES carteira (id) ON DELETE CASCADE,
    FOREIGN KEY (ativo_id)    REFERENCES ativo (id)    ON DELETE CASCADE
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_posicao_carteira_ativo ON posicao (carteira_id, ativo_id);

CREATE TABLE IF NOT EXISTS alerta (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    usuario_id       INTEGER NOT NULL,
    ativo_id         INTEGER NOT NULL,
    condicao         TEXT    NOT NULL
                             CHECK (condicao IN ('PrecoAcimaDe', 'PrecoAbaixoDe', 'VariacaoDiariaAcimaDe')),
    valor_referencia REAL    NOT NULL,
    estado           TEXT    NOT NULL DEFAULT 'Ativo'
                             CHECK (estado IN ('Ativo', 'Disparado', 'Arquivado')),
    criado_em        TEXT    NOT NULL,
    disparado_em     TEXT,
    FOREIGN KEY (usuario_id) REFERENCES usuario (id) ON DELETE CASCADE,
    FOREIGN KEY (ativo_id)   REFERENCES ativo (id)   ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_alerta_usuario_estado ON alerta (usuario_id, estado);

CREATE TABLE IF NOT EXISTS recomendacao (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    ativo_id       INTEGER NOT NULL,
    tipo           TEXT    NOT NULL CHECK (tipo IN ('Compra', 'Venda', 'Neutro')),
    justificativa  TEXT    NOT NULL DEFAULT '',
    regra_aplicada TEXT    NOT NULL DEFAULT '',
    gerada_em      TEXT    NOT NULL,
    estado         TEXT    NOT NULL DEFAULT 'Gerada'
                           CHECK (estado IN ('Gerada', 'Vigente', 'Expirada', 'Invalidada')),
    pontuacao      REAL    NOT NULL DEFAULT 0,
    FOREIGN KEY (ativo_id) REFERENCES ativo (id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_recomendacao_ativo_estado ON recomendacao (ativo_id, estado);

CREATE TABLE IF NOT EXISTS regra_configurada (
    id                   INTEGER PRIMARY KEY AUTOINCREMENT,
    nome_regra           TEXT    NOT NULL,
    ativa               INTEGER NOT NULL DEFAULT 1 CHECK (ativa IN (0, 1)),
    parametro_principal  REAL    NOT NULL DEFAULT 0,
    parametro_secundario REAL    NOT NULL DEFAULT 0
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_regra_nome ON regra_configurada (nome_regra);

CREATE TABLE IF NOT EXISTS importacao (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    ativo_id         INTEGER NOT NULL,
    arquivo          TEXT    NOT NULL,
    estado           TEXT    NOT NULL DEFAULT 'Pendente'
                             CHECK (estado IN ('Pendente', 'Validando', 'Concluida', 'Rejeitada')),
    linhas_lidas     INTEGER NOT NULL DEFAULT 0,
    linhas_inseridas INTEGER NOT NULL DEFAULT 0,
    mensagem_erro    TEXT    NOT NULL DEFAULT '',
    executada_em     TEXT,
    FOREIGN KEY (ativo_id) REFERENCES ativo (id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_importacao_ativo ON importacao (ativo_id);
