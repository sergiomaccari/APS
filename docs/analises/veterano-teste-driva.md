# Análise do projeto `teste-driva-master`

## 1. O que o projeto é — confirmação

**Confirmado: é um teste técnico da empresa Driva, não um trabalho da disciplina APS.** As evidências são diretas:

- `instrucoes.md` é o enunciado original do desafio, escrito pela empresa: "Na Driva, trabalhamos com inteligência de mercado B2B através do nosso principal produto: o **HubDriva**". Define missão, entregáveis obrigatórios (docker-compose, init.sql, API, dashboard, workflows n8n, README, vídeo) e critérios de avaliação de contratação.
- `README.md` termina com: "Desenvolvido como Teste Técnico para Driva" e contém link de Postman de um workspace pessoal ("Gustavo Moretto Dias's Workspace") — ou seja, é a **solução de um candidato** (provavelmente um veterano, daí a pasta `trabalho veteranos/`).
- O sufixo `-master` no nome da pasta indica download de um ZIP de repositório GitHub.
- Não há nenhum artefato típico de APS (casos de uso, UML, documento de requisitos acadêmico, relatório de disciplina).

O desafio: construir um pipeline ELT de monitoramento de "jobs de enriquecimento de dados" — API de origem paginada → ingestão via n8n → Data Warehouse em camadas Bronze/Gold no Postgres → API de analytics → dashboard.

## 2. Arquitetura e stack

**Stack:** Node.js 18+ / Express 5, PostgreSQL 15 (alpine), n8n (orquestração low-code), frontend HTML único com TailwindCSS (CDN) + Chart.js + Phosphor Icons, tudo orquestrado por Docker Compose (3 serviços: `api`, `postgres`, `n8n`, rede bridge, volumes persistentes, healthcheck no Postgres).

**Componentes:**
- **API única** (`api/index.js`) com dois grupos de rotas: `routes/source.js` (`GET /people/v1/enrichments` — simula a fonte paginada, com auth Bearer e simulação aleatória de erro 429 em 10% das chamadas) e `routes/analytics.js` (`/overview`, `/enrichments` com filtros/paginação, `/workspaces/top` — leem a camada Gold). Também serve o frontend estático.
- **DW em camadas** (`docker/postgres/init.sql`): `api_enrichments_seed` (fonte simulada), `bronze_enrichments` (dado cru em `JSONB` + `dw_ingested_at`/`dw_updated_at`), `gold_enrichments` (colunas tipadas em português + campos calculados: duração, tempo por contato, categoria de tamanho, flags de sucesso/reprocessamento) com índices, e `dw_pipeline_control` (watermark).
- **3 workflows n8n desacoplados:** (1) Ingestão Bronze — loop de paginação com UPSERT idempotente, retry configurado (5 tentativas, espera fixa de 5s) e delay de 2s entre páginas; (2) Processamento Gold — carga incremental via `MAX(data_atualizacao_dw)` da Gold comparado com `dw_updated_at` da Bronze; (3) Orquestrador — cron de 5 min que encadeia ingestão → processamento → log.
- **Seed** (`api/seed.js`): gera 5.000 registros com faker em lotes de 1.000.

## 3. Qualidade do código

**Pontos fortes:**
- Arquitetura bem separada (rotas por responsabilidade, camadas Bronze/Gold corretas, tabela de watermark prevista).
- `routes/analytics.js` usa **queries parametrizadas** corretamente, filtros dinâmicos montados com placeholders, `Promise.all` para count+dados, `FILTER (WHERE ...)` no SQL de KPIs — SQL competente.
- Idempotência via UPSERT, carga incremental documentada, tratamento do 429 previsto no workflow.
- README claro sobre como rodar; Docker Compose com healthcheck e `depends_on` condicional.

**Pontos fracos:**
- `api/seed.js` monta SQL por **interpolação de string** (escapa só apóstrofos) em vez de query parametrizada — frágil, embora os dados venham do faker.
- Inconsistências: README manda usar senha `postgres` mas o compose define `password`; README cita pastas `dashboard/` e `docs/` que não existem; `version: '3.8'` no compose é obsoleto.
- Em `api/index.js`, o caminho do frontend depende de `NODE_ENV === 'production'`, mas o compose **não define NODE_ENV no serviço api** — funciona só porque o volume monta `./frontend` e/ou o Dockerfile compensa; frágil.
- Auth duplicada (inline em `source.js`, middleware em `analytics.js`, com chave default hardcoded como fallback); import morto (`const { parse } = require('dotenv')` em `source.js`); `PORT` sem default.
- API key exposta no frontend (hardcoded no `index.html` — admitido como "demo", mas a auth vira teatro).
- A tabela `dw_pipeline_control` é criada e seedada, mas o workflow Gold usa `MAX(data_atualizacao_dw)` direto — a watermark comparando timestamps de eventos diferentes (Bronze vs Gold) pode perder registros em execuções concorrentes.
- Retry com espera **fixa**, não backoff exponencial como o enunciado pedia; gráfico de status calculado no cliente sobre no máximo 1.000 linhas (impreciso com 5.000 registros); zero testes automatizados; sem lint.

**Veredito:** solução funcional e acima da média para teste técnico júnior, com boas decisões de dados, mas com descuidos de consistência e sem testes.

## 4. O que pode servir de referência para um trabalho de APS

Como **referência de estudo** (não como entregável — o domínio e os artefatos esperados em APS são outros):

1. **`instrucoes.md` como modelo de especificação de requisitos** — é o artefato mais valioso para APS: contexto de negócio, requisitos funcionais e não funcionais, regras de negócio detalhadas (traduções, campos calculados, categorização), critérios de aceitação e entregáveis. Excelente exemplo real de como escrever um documento de requisitos.
2. **Arquitetura em camadas e separação de responsabilidades** — o padrão medallion (Bronze/Gold), a divisão fonte/analytics na API e os 3 workflows desacoplados são material direto para diagramas de componentes, implantação (o docker-compose praticamente É um diagrama de implantação em YAML) e sequência (fluxo orquestrador → ingestão → processamento).
3. **Modelagem de dados** — `init.sql` demonstra decisões de projeto discutíveis em aula: JSONB para captura fiel vs colunas tipadas para consumo, índices por padrão de acesso, tabela de controle/watermark.
4. **Projeto de API REST** — paginação com `meta`, autenticação por API key, filtros, códigos de status (401/429/500): bom exemplo para especificação de interfaces.
5. **Qualidade como anti-exemplo também** — as inconsistências README×compose, a watermark frágil e a chave no frontend rendem discussão sobre rastreabilidade requisito→implementação e atributos de qualidade.

**Ressalva:** não deve ser reaproveitado como código do trabalho da disciplina (é solução autoral de terceiro para processo seletivo; usar trechos sem crédito configuraria plágio).

Caminho analisado: `/home/sergi/repositorios/APS/APS/trabalho veteranos/teste-driva-master/teste-driva-master/`
