-- Peso de cada regra na media ponderada do MotorAnalise (RF021 / extensao da spec).
-- Valor padrao 1.0 preserva o comportamento anterior (media simples).
ALTER TABLE regra_configurada ADD COLUMN peso REAL NOT NULL DEFAULT 1.0;
