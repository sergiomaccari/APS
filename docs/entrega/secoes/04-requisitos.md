## 2.5 Requisitos Funcionais, Não Funcionais, Restrições de Projeto e Requisitos de Experiência do Usuário

Após a entrevista com o cliente e a análise das respostas, respeitando as suas
demandas, foram definidos os seguintes requisitos funcionais:

- [RF001] O usuário deve ser capaz de realizar login no sistema.
- [RF002] O administrador deve ser capaz de cadastrar ativos (ações e fundos imobiliários).
- [RF003] O administrador deve ser capaz de atualizar ativos.
- [RF004] O administrador deve ser capaz de remover ativos.
- [RF005] O administrador deve ser capaz de importar cotações de arquivos CSV.
- [RF006] O administrador deve ser capaz de configurar as regras de análise.
- [RF007] O administrador deve ser capaz de gerenciar os usuários do sistema.
- [RF008] O investidor deve ser capaz de buscar ativos por ticker ou nome.
- [RF009] O investidor deve ser capaz de visualizar o gráfico de cotações com médias móveis.
- [RF010] O investidor deve ser capaz de visualizar os indicadores técnicos do período.
- [RF011] O investidor deve ser capaz de visualizar os fundamentos e o histórico de um ativo.
- [RF012] O sistema deve gerar recomendação de compra, venda ou neutralidade para um ativo.
- [RF013] O investidor deve ser capaz de visualizar as recomendações vigentes com sua justificativa.
- [RF014] O investidor deve ser capaz de registrar a compra de um ativo na carteira.
- [RF015] O investidor deve ser capaz de registrar a venda de um ativo da carteira.
- [RF016] O investidor deve ser capaz de visualizar o resultado consolidado da carteira.
- [RF017] O investidor deve ser capaz de cadastrar alertas de preço.
- [RF018] O sistema deve disparar e notificar os alertas cuja condição foi satisfeita.
- [RF019] O investidor deve ser capaz de arquivar alertas já disparados.

Considerando a implementação do sistema, também foram levantados requisitos não
funcionais:

- [RNF001] O sistema deve funcionar sem conexão com a internet, usando as cotações já importadas.
- [RNF002] A importação de cotações deve ser atômica: um arquivo com erro estrutural não grava nenhuma linha.
- [RNF003] Reimportar o mesmo arquivo não deve duplicar cotações.
- [RNF004] As senhas devem ser armazenadas apenas como resumo criptográfico (SHA-256), nunca em texto puro.
- [RNF005] Somente o administrador deve acessar as telas de administração.
- [RNF006] Toda recomendação deve exibir a regra e os números que a justificaram.
- [RNF007] O sistema deve responder às interações da interface em menos de 2 segundos com 5 anos de histórico carregado.

Para definir limitações técnicas ao desenvolvimento do sistema, foram elaboradas as
restrições de projeto:

- [RP001] O sistema será implementado em C++ com o paradigma orientado a objetos.
- [RP002] A interface gráfica utilizará o framework Qt 6 (módulos Widgets e Charts).
- [RP003] O banco de dados será relacional (SQLite), com no mínimo 5 tabelas.
- [RP004] A compilação será feita com CMake, sem dependências externas ao Qt.

Para a interação do usuário com o sistema, foram considerados requisitos de
experiência do usuário:

- [REU001] A navegação deve exibir somente as opções permitidas ao papel do usuário.
- [REU002] Ganho e perda devem ser distinguíveis por cor e por sinal numérico.
- [REU003] Mensagens de erro devem indicar o motivo — e a linha, na importação — em português.
- [REU004] O sistema deve indicar claramente que as recomendações são apoio à decisão, não aconselhamento financeiro.

\newpage
