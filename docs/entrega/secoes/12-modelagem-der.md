### Mapeamento para o Diagrama Entidade-Relacionamento

O modelo de classes foi mapeado para um banco relacional com **nove tabelas**
(o mínimo exigido em [RP003] é cinco): `usuario`, `ativo`, `cotacao`, `carteira`,
`posicao`, `alerta`, `recomendacao`, `regra_configurada` e `importacao`. As regras
de mapeamento adotadas foram:

- **Uma tabela por classe concreta persistente**, com chave primária artificial
  `id INTEGER`.
- **Herança por tabela única:** a hierarquia Ativo → Acao/FundoImobiliario foi
  mapeada para a tabela `ativo` com coluna discriminadora `tipo` e colunas dos
  fundamentos específicos anuláveis — adequado porque as subclasses diferem em
  poucos atributos.
- **Associações 1:N viram chaves estrangeiras** no lado N (`cotacao.ativo_id`,
  `posicao.carteira_id`, `alerta.usuario_id` etc.), com `ON DELETE CASCADE` nas
  composições (remover um ativo remove suas cotações) e restrição de unicidade
  onde a regra de negócio exige — `UNIQUE(ativo_id, data)` em `cotacao` é o que
  garante [RNF003]/[RN010] no próprio banco.
- **Enumerações** (papel do usuário, estado do alerta, tipo da recomendação)
  viram colunas `TEXT` com domínio controlado pela aplicação, legíveis no banco.

![Figura {{F:der-banco}}. Diagrama Entidade-Relacionamento do banco de dados.](figuras/der-banco.png)
