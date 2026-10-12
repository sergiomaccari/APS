### Diagrama de Classes

O sistema é organizado em camadas: as telas (Qt Widgets) conversam com serviços,
que concentram as regras de negócio; os serviços usam repositórios, que isolam todo
o acesso SQL; e os repositórios operam sobre a conexão única mantida pela classe
BancoDeDados. As classes de domínio circulam entre as camadas como objetos de
valor. A Figura {{F:cls-dominio}} apresenta o domínio, a
Figura {{F:cls-analise}} o núcleo de análise e a Figura {{F:cls-arquitetura}} a
visão em camadas.

No domínio, **Ativo** é uma classe abstrata especializada por herança em **Acao** e
**FundoImobiliario**, que implementam polimorficamente os métodos `tipo()` e
`indiceValor()` — é o mecanismo que permite às telas e regras tratarem qualquer
ativo de forma uniforme.

![Figura {{F:cls-dominio}}. Diagrama de classes do domínio do sistema.](figuras/cls-dominio.png)

O núcleo de análise aplica o padrão de projeto **Strategy** (GAMMA et al., 2000): a
interface **RegraAnalise** define o contrato `avaliar()`, implementado por quatro
regras concretas (cruzamento de médias móveis, índice de força relativa, dividend
yield e preço sobre lucro), e o **MotorAnalise** compõe as regras ativas e
consolida seus votos em uma recomendação com pontuação entre −1 e +1 [RN015]. Uma
regra sem dados suficientes se abstém [RN019], e a decisão final aplica o limiar de
±0,25 [RN016]. Novas regras entram no sistema implementando a interface, sem
alterar o motor.

![Figura {{F:cls-analise}}. Diagrama de classes do núcleo de análise, com o padrão Strategy.](figuras/cls-analise.png)

Na arquitetura em camadas destacam-se ainda três padrões: **Observer** — o
ServicoAlerta notifica os observadores registrados (a JanelaPrincipal implementa a
interface ObservadorAlerta) quando um alerta dispara [RF018]; **Repository** — um
repositório por agregado isola o SQL do restante do sistema; e **Singleton** — a
classe BancoDeDados garante uma única conexão com o arquivo SQLite.

![Figura {{F:cls-arquitetura}}. Diagrama de classes da arquitetura em camadas.](figuras/cls-arquitetura.png)
