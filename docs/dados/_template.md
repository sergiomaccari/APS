# Modelo de dados: <TABELA ou DOMÍNIO>

> Documente apenas a **estrutura** — nunca dados reais nem credenciais.
> Use a skill `modelo-de-dados` para manter isto atualizado.

## Visão geral
<O que estes dados representam e qual processo/plataforma os usa.>

## Tabela `<nome_da_tabela>`

| Coluna | Tipo | Nulo? | Default | Descrição |
|--------|------|-------|---------|-----------|
| `id` | bigserial | não | — | Chave primária |
| `<col>` | `<tipo>` | sim/não | `<default>` | <descrição> |

- **Chave primária:** `id`
- **Chaves estrangeiras:** `<col>` → `<outra_tabela>.<col>`
- **Índices:** `<idx>` em (`<col>`) — <motivo>
- **Restrições:** <unique, check, etc.>

## Relações
- `<tabela>` 1:N `<outra_tabela>` — <descrição do relacionamento>

## Migrações
| Data | Mudança | Motivo |
|------|---------|--------|
| <AAAA-MM-DD> | <criação / alteração> | <por quê> |

## Referências
- Processo que usa: `docs/processos/<...>.md`
- Plataforma/banco: `docs/plataformas/<...>.md`
