# Perfil técnico — sergiomaccari / Driva-tecnologia

Acesso à organização **Driva-tecnologia** funcionou (50 repositórios listados, todos privados). Repositórios examinados em detalhe: `relatorios-driva`, `management-service` e `PRM` (Driva); `EntrevistaDriva`, `Simulador-Relativ-stico`, `CyberMetro` e `Praticas_ASL` (pessoais). O repositório `APS` (diretório de trabalho atual) está **vazio** no GitHub — provavelmente o trabalho de faculdade que está começando.

## 1. O que a Driva constrói e com que stack

A Driva constrói uma **plataforma B2B de dados/enriquecimento comercial** (sales intelligence): serviços de autenticação e gestão de empresas/usuários/créditos, dashboards de analytics internos (vendas, forecast, enriquecimento, churn, inadimplência), PRM para parceiros, além de forte investimento em **automação e IA** (repos `aifirst`, `SergIA`, `agente-db`, `claude-automacoes`, `n8n-files`, `athen-ai`).

**Stack dominante: TypeScript de ponta a ponta.**
- **`relatorios-driva`** (TS ~582 KB): monorepo **Turborepo + npm workspaces**, **Next.js 16 (App Router), React 19, Tailwind CSS, Radix UI, ApexCharts, PostgreSQL** (camadas bronze/silver/gold), **NextAuth (Google OAuth + JWT)**, sistema de permissões com herança, Docker.
- **`management-service`** (TS ~2,7 MB): serviço de auth/gestão em **Node/NestJS** (yarn, `start:dev`, testes), **SSO Microsoft Entra (OIDC)**, deploy em **Azure Container Apps**.
- **`PRM`** (TS ~1,2 MB): app **Next.js + Prisma** (migrate/seed) para gestão de parceiros, comissões e documentos.
- Padrões transversais: PostgreSQL como warehouse (bronze/gold), n8n para ETL/orquestração, Docker em tudo.

## 2. Perfil técnico do Sergio (evidências dos repositórios)

- **`EntrevistaDriva`** (Python + Docker): pipeline de dados **end-to-end** — API transacional em **FastAPI** com Faker (com simulação de rate limit 429 e lógica de updates), orquestração **n8n** com retry/backoff e paginação, **PostgreSQL bronze/gold**, API de analytics isolando o banco, dashboard **Streamlit + Plotly**, tudo em **Docker Compose**. Mostra maturidade de arquitetura (separação de camadas, justificativas de decisão no README).
- **`Simulador-Relativ-stico`** (TypeScript + GLSL + Three.js/Vite): o mais sofisticado — física relativística implementada **em shaders GLSL** (contração de Lorentz, Doppler, beaming, aberração), reconstrução de cor via curvas CIE 1931 pré-tabeladas na CPU, artigo em LaTeX (PT/EN), demo no GitHub Pages, i18n. Evidencia forte base **matemática/física** aliada a engenharia de performance.
- **`CyberMetro`** (C++ ~2 MB, CMake/Visual Studio): jogo com **SFML**, nlohmann/json, sistema de saves, **diagramas UML** — projeto típico de disciplina de POO, com C++ orientado a objetos de porte razoável.
- **`Praticas_ASL`** (TeX + MATLAB + Python): práticas de Análise de Sinais e Sistemas Lineares — computação científica e relatórios em LaTeX.
- Outros: `Analise-Tensoes-Marquise`, `Analise-MHS-Pendulo-Simples`, `Simulador-de-Colis-es`, seminário de computação quântica — perfil de **engenharia com forte veia de física/simulação**.

**Resumo:** aluno de engenharia (provável Eng. da Computação/Elétrica) com domínio real de **Python (FastAPI, dados/ETL), TypeScript (Three.js/web), C++ (POO)**, Docker, PostgreSQL e n8n; nível de sofisticação acima da média para graduação (shaders, arquitetura em camadas, documentação LaTeX). Estagiário em Automação na Driva (o teste técnico `EntrevistaDriva` casa com os repos da empresa).

## 3. Recomendação de stack para o trabalho de faculdade (APS)

Aproveitando o que ele já domina e o que usa no estágio:

- **Backend/dados:** **Python + FastAPI + PostgreSQL** (com Docker Compose) — é a combinação que ele já provou dominar no `EntrevistaDriva` e entrega rápido. Se o trabalho envolver ETL/automação, **n8n** é um diferencial que ele já conhece.
- **Frontend/visualização:** duas rotas seguras:
  - **Streamlit + Plotly** se o foco for dados/dashboard (mínimo esforço, ele já usou);
  - **TypeScript + Vite** (com Three.js se houver simulação/visualização 3D) se o trabalho pedir interface rica — ou **Next.js + Tailwind** se quiser alinhar com a stack da Driva e reaproveitar o aprendizado do estágio.
- **Se a APS for de POO/estruturas de dados:** C++ com SFML e UML já é o caminho batido dele (padrão `CyberMetro`).
- **Documentação:** **LaTeX**, que ele já usa em `Praticas_ASL` e no artigo do simulador.

Recomendação principal: **FastAPI + PostgreSQL + Docker no backend e Streamlit (dados) ou Vite/TypeScript (interface) no frontend** — maximiza reuso de competências comprovadas e conversa com a stack profissional dele.
