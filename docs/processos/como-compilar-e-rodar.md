# Processo: compilar e executar o Analisador B3

- **Objetivo:** deixar o sistema compilando e rodando em uma máquina nova, com o
  banco de demonstração populado, pronto para os Testes de Validação da disciplina.
- **Quando usar:** ao preparar o ambiente de qualquer integrante da equipe e antes
  das apresentações de 20/10 e 24/11/2026.
- **Tempo estimado:** 15 minutos (a maior parte é o download do Qt).

## 1. Pré-requisitos

| Item | Versão mínima | Verificar com |
|---|---|---|
| Compilador C++ com suporte a C++20 | GCC 11 / Clang 14 | `g++ --version` |
| CMake | 3.19 | `cmake --version` |
| Qt | 6.2 (Widgets, Charts, Sql) | `qmake6 --version` |
| Driver SQLite do Qt | — | ver passo 4 |

### Ubuntu, Debian ou WSL

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-charts-dev libqt6sql6-sqlite
```

> `qt6-charts-dev` é obrigatório: sem ele o CMake falha em
> `find_package(Qt6 ... Charts)`. `libqt6sql6-sqlite` é o driver do banco; sem ele
> o sistema abre e avisa que o driver `QSQLITE` não está disponível.

### Fedora

```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qtcharts-devel qt6-qtbase-mysql
```

### Windows

Instale o **Qt Online Installer** marcando *Qt 6.x → MSVC 2022 64-bit* e
*Qt Charts*, mais o **Visual Studio 2022** com "Desenvolvimento para desktop com
C++". Depois configure na pasta do projeto:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.7.0\msvc2019_64"
cmake --build build --config Release
```

### WSL com interface gráfica

No Windows 11 o WSLg já exibe janelas Qt sem configuração. No Windows 10, instale
um servidor X (VcXsrv) e exporte o display:

```bash
export DISPLAY=$(grep nameserver /etc/resolv.conf | awk '{print $2}'):0
```

## 2. Compilar

```bash
cd ~/repositorios/APS
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

A configuração imprime uma linha de confirmação, por exemplo:
`-- AnalisadorB3 1.0.0 - Qt 6.4.2 - build Release`.

Alvos gerados em `build/`:

| Alvo | O que é |
|---|---|
| `libanalisador-nucleo.a` | domínio + análise + persistência + serviços |
| `analisador-b3` | aplicação com interface gráfica |
| `teste-indicadores`, `teste-regras`, `teste-importacao` | testes automatizados |

## 3. Executar

```bash
./build/analisador-b3
```

Na primeira execução o sistema cria o banco e aplica as migrações
(esquema, dados de semente e o ativo de demonstração). Entre com:

- **Administrador:** `admin@analisadorb3.com` / `admin123`
- **Investidor:** `investidor@analisadorb3.com` / `investidor123`

Para carregar as cotações: *Importar cotações* → **Importar pasta inteira** →
selecione `dados/` (o build já copia os CSVs para `build/dados/`).

## 4. Rodar os testes

```bash
ctest --test-dir build --output-on-failure
```

São 61 casos: indicadores (27), regras de análise (26) e importação (8, com banco
SQLite temporário e migrações reais). Para rodar um alvo isolado, com mais
detalhes:

```bash
./build/teste-importacao -v2
```

## 5. Recomeçar do zero

O banco fica fora da pasta do projeto. Para voltar ao estado inicial:

```bash
rm -rf ~/.local/share/"UTFPR - APS"/"Analisador B3"
rm -rf build
```

## 6. Problemas mais prováveis

| Sintoma | Causa | Correção |
|---|---|---|
| `Could not find a package configuration file provided by "Qt6"` | Qt não instalado ou fora do caminho | instale os pacotes do passo 1 ou passe `-DCMAKE_PREFIX_PATH=<pasta do Qt>` |
| `Unknown CMake command "qt_add_resources"` ou erro em `Charts` | falta `qt6-charts-dev` | instale o pacote e reconfigure (`rm -rf build`) |
| Janela abre e avisa `Driver QSQLITE não disponível` | falta o driver SQLite do Qt | `sudo apt install libqt6sql6-sqlite` |
| `qt.qpa.plugin: Could not load the Qt platform plugin "xcb"` | ambiente sem servidor gráfico (WSL/servidor) | use WSLg, instale VcXsrv e exporte `DISPLAY`, ou instale `libxcb-cursor0` |
| Gráfico vazio com "Sem cotações importadas" | CSVs ainda não importados | faça o passo 3 (importar a pasta `dados/`) |
| Recomendação sai `Neutro` para todos | regras desativadas ou sem histórico | verifique *Regras de análise* e se as cotações foram importadas |
| Erro de link em `qrc_recursos` ou migração não encontrada | build antigo, anterior ao ajuste do recurso | `rm -rf build` e recompile |

## 7. Checklist antes da apresentação

- [ ] `cmake --build build` sem avisos novos.
- [ ] `ctest --test-dir build` com 3 alvos passando.
- [ ] Banco recriado do zero e CSVs importados (passo 5 e depois o passo 3).
- [ ] Roteiro do README executado inteiro, incluindo a rejeição do arquivo
      inválido e o disparo de um alerta.
- [ ] Testado na máquina do laboratório, **sem internet**.
