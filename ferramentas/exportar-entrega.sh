#!/usr/bin/env bash
# =============================================================================
# exportar-entrega.sh — gera a branch órfã da entrega de 13/10 (protótipo).
#
# Uso:
#   ferramentas/exportar-entrega.sh [--dry-run] [--branch entrega-13-10] [--forcar]
#                                   [--aceitar-avisos]
#
#   --dry-run          só lista (ordenado) os arquivos que entrariam, com a
#                      contagem, e roda as verificações; não cria nada.
#   --branch           nome da branch a gerar (padrão: entrega-13-10).
#   --forcar           recria a branch se ela já existir (sem isso, aborta).
#   --aceitar-avisos   no modo real, segue mesmo com termos suspeitos no
#                      conteúdo (sem isso, aborta antes do commit).
#
# Origem: o commit HEAD atual (arquivos vêm do commit, nunca do disco, para
# não levar nada que não esteja versionado). Fora do --dry-run exige árvore
# sem alterações em arquivos versionados.
#
# O que faz (ver docs/design/0001-prototipo-de-telas-stubs.md, "Branch da
# entrega"):
#   1. monta a whitelist a partir de HEAD — inclui as seções do 1º bimestre
#      (lidas do manifesto em ferramentas/montar-documento.py) e as figuras
#      referenciadas por docs/entrega/documento-1-bimestre.md;
#   2. cria um worktree temporário, faz checkout --orphan e limpa índice e
#      arquivos;
#   3. extrai a whitelist com git archive, reduz o CMakeLists.txt aos alvos
#      do protótipo (APENAS_PROTOTIPO ligado), gera um .gitignore reduzido e
#      move docs/entrega/README-entrega.md para README.md;
#   4. verifica que nada proibido entrou (caminhos e conteúdo) e faz um
#      commit único com autor Sergio Maccari; remove o worktree.
#
# Não faz push: publicar a branch é passo manual do plano.
# =============================================================================
set -euo pipefail

# ----------------------------------------------------------------------------
# Constantes
# ----------------------------------------------------------------------------
readonly AUTOR_NOME="Sergio Maccari"
readonly AUTOR_EMAIL="sergio.r.maccari@gmail.com"
readonly MENSAGEM_COMMIT="Entrega 13/10 — Plano do Projeto e protótipo de telas"
readonly README_ORIGEM="docs/entrega/README-entrega.md"
readonly DOC_1BIM="docs/entrega/documento-1-bimestre.md"
readonly MONTADOR="ferramentas/montar-documento.py"

# Caminhos que NUNCA podem entrar na branch pública (RP-09). A whitelist já
# barra todos; esta lista é defesa em profundidade (ex.: uma nota .md posta
# dentro de src/prototipo/).
readonly PROIBIDOS_REGEX='^(inicio\.txt|APS/|padroes/|design-system/|docs/analises/|testes/|recursos/sql/|LOG\.md$|MARCOS\.md$|docs/HANDOFF\.md$|Leiam esse doc|docs/(specs|design|planos|decisoes)/|docs/processos/prototipo-vs-versao-final\.md$)|(^|/)(\.claude|\.agents|\.superpowers)/|(^|/)CLAUDE\.md$|(^|/)cofre'
readonly PROIBIDOS_CPP_REGEX='^src/(analise|persistencia|servicos)/.*\.cpp$'
# Conteúdo que bloqueia o modo real (salvo --aceitar-avisos). Sem diferenciar
# maiúsculas; "IA" isolado é checado à parte, com maiúsculas, para não casar
# palavras comuns em português. O .gitignore gerado fica de fora (lista
# .claude/ e .superpowers/ de propósito) e é checado só contra os termos de
# autoria/empresa.
readonly SUSPEITOS_REGEX_I='co-authored-by|claude|anthropic|driva|automacoes|superpowers'
readonly SUSPEITOS_REGEX_CS='\bIA\b'
readonly SUSPEITOS_GITIGNORE_REGEX='co-authored-by|anthropic|driva|automacoes'

# Whitelist por padrão (regex sobre os caminhos versionados em HEAD).
# Cada grupo precisa casar com pelo menos um arquivo; os itens marcados como
# "exato" precisam existir individualmente.
readonly -a WHITELIST_GRUPOS=(
    '^CMakeLists\.txt$'
    '^src/ui/'
    '^src/dominio/'
    '^src/servicos/[^/]+\.h$'
    '^src/persistencia/repositorio[^/]*\.h$'
    '^src/prototipo/'
    '^dados/[^/]+\.csv$'
)
# dados/*.csv inclui o INVALIDO-exemplo.csv: o roteiro da apresentação (passo 7)
# e a mensagem fixa de importação (RP-05) o citam.
readonly -a WHITELIST_EXATOS=(
    'CMakeLists.txt'
    'src/analise/regraanalise.h'
    'src/analise/motoranalise.h'
    'src/analise/calculadoraindicadores.h'
    'src/analise/backtester.h'
    'dados/INVALIDO-exemplo.csv'
    'recursos/prototipo.qrc'
    'ferramentas/capturar-telas.cpp'
    'docs/processos/roteiro-apresentacao-1.md'
    'docs/entrega/documento-1-bimestre.md'
    'docs/entrega/documento-1-bimestre.docx'
    'docs/entrega/documento-1-bimestre.pdf'
    "$README_ORIGEM"
)

# .gitignore da branch: gerado aqui em vez de copiado. O original de main cita
# nomes de análises internas e material de terceiros (comentários e caminhos
# em docs/analises/) que não devem aparecer num repositório público; a versão
# reduzida não tem comentários descritivos nem nomes de arquivos.
# Ela ignora, porém, os diretórios locais que continuam no disco do Sergio
# depois de "git checkout entrega-13-10" (eram ignorados ou não versionados em
# main): sem isso apareceriam como não versionados na branch e um
# "git add -A" os publicaria. Menção literal a .claude/ e .agents/ é escolha
# consciente (já constam do .gitignore público de main). .superpowers/ fica só
# em .git/info/exclude, que vale para todas as branches do clone.
read -r -d '' GITIGNORE_ENTREGA <<'FIM' || true
# Build do CMake e artefatos locais
build*/
capturas*/
*.user

# Editores / sistema
.vscode/
.idea/
.DS_Store
Thumbs.db
*.log

# Arquivos locais que nunca devem ser versionados
inicio.txt
.env
.env.*
.claude/
.agents/
APS/
docs/analises/
padroes/
design-system/
cofre*
FIM
readonly GITIGNORE_ENTREGA

# ----------------------------------------------------------------------------
# Utilitários de saída
# ----------------------------------------------------------------------------
erro() { printf '\nERRO: %s\n' "$*" >&2; exit 1; }
info() { printf '%s\n' "$*"; }
titulo() { printf '\n== %s ==\n' "$*"; }

uso() {
    sed -n '4,14p' "$0" | sed 's/^# \{0,1\}//'
}

# ----------------------------------------------------------------------------
# Argumentos
# ----------------------------------------------------------------------------
DRY_RUN=0
FORCAR=0
ACEITAR_AVISOS=0
BRANCH="entrega-13-10"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --dry-run) DRY_RUN=1 ;;
        --forcar) FORCAR=1 ;;
        --aceitar-avisos) ACEITAR_AVISOS=1 ;;
        --branch)
            [[ $# -ge 2 && -n "$2" ]] || erro "--branch exige um nome."
            BRANCH="$2"; shift ;;
        --branch=*) BRANCH="${1#--branch=}" ;;
        -h|--help) uso; exit 0 ;;
        *) uso >&2; erro "argumento desconhecido: $1" ;;
    esac
    shift
done
git check-ref-format --branch "$BRANCH" >/dev/null 2>&1 \
    || erro "nome de branch inválido: $BRANCH"

command -v python3 >/dev/null || erro "python3 é necessário para ler o manifesto e as figuras."
command -v tar >/dev/null || erro "tar é necessário para extrair a whitelist."

RAIZ="$(git rev-parse --show-toplevel 2>/dev/null)" || erro "execute dentro do repositório."
cd "$RAIZ"
ORIGEM="$(git rev-parse --verify HEAD)"
ORIGEM_CURTO="$(git rev-parse --short HEAD)"
ORIGEM_NOME="$(git symbolic-ref --short -q HEAD || echo 'HEAD destacado')"

# Diretório temporário de trabalho (arquivos auxiliares) e worktree.
AUX="$(mktemp -d)"
WT=""
BRANCH_TEMP=""
limpar() {
    local codigo=$?
    if [[ -n "$WT" && -d "$WT" ]]; then
        git -C "$RAIZ" worktree remove --force "$WT" >/dev/null 2>&1 || rm -rf "$WT"
        git -C "$RAIZ" worktree prune >/dev/null 2>&1 || true
    fi
    if [[ -n "$BRANCH_TEMP" ]] && git -C "$RAIZ" show-ref --verify --quiet "refs/heads/$BRANCH_TEMP"; then
        git -C "$RAIZ" branch -D -q "$BRANCH_TEMP" >/dev/null 2>&1 || true
    fi
    rm -rf "$AUX"
    if [[ $codigo -ne 0 && $DRY_RUN -eq 0 ]]; then
        printf '\nExportação interrompida (código %d); nada foi publicado.\n' "$codigo" >&2
    fi
}
trap limpar EXIT

# ----------------------------------------------------------------------------
# Pré-condições
# ----------------------------------------------------------------------------
if [[ $DRY_RUN -eq 0 ]]; then
    if [[ -n "$(git status --porcelain --untracked-files=no)" ]]; then
        git status --short --untracked-files=no >&2
        erro "há alterações não commitadas em arquivos versionados; commite ou descarte antes (a origem é HEAD)."
    fi
    # Arquivos não versionados não afetam o resultado (tudo vem de HEAD), mas
    # podem indicar algo esquecido fora do commit.
    NAO_VERSIONADOS="$(git status --porcelain --untracked-files=normal | grep -c '^??' || true)"
    if [[ "$NAO_VERSIONADOS" -gt 0 ]]; then
        info "Aviso: $NAO_VERSIONADOS caminho(s) não versionado(s) na árvore — ficam de fora (origem é HEAD)."
    fi
    if git show-ref --verify --quiet "refs/heads/$BRANCH"; then
        [[ $FORCAR -eq 1 ]] || erro "a branch '$BRANCH' já existe; use --forcar para recriá-la."
        if git worktree list --porcelain | grep -qx "branch refs/heads/$BRANCH"; then
            erro "a branch '$BRANCH' está em checkout em algum worktree; saia dela antes de recriar."
        fi
    fi
fi

# ----------------------------------------------------------------------------
# 1. Monta a lista de arquivos a partir de HEAD
# ----------------------------------------------------------------------------
# core.quotePath=false: caminhos não ASCII saem literais (sem aspas/escapes) e
# casam com os padrões da whitelist.
git -c core.quotePath=false ls-tree -r --name-only "$ORIGEM" > "$AUX/arvore.txt"

existe_em_head() { grep -qxF -- "$1" "$AUX/arvore.txt"; }

FALTANDO=()

# Seções do 1º bimestre: lidas do manifesto do montador (versão de HEAD).
secoes_1bim() {
    git show "$ORIGEM:$MONTADOR" > "$AUX/montador.py"
    python3 -I - "$AUX/montador.py" <<'PY'
import importlib.util, sys
spec = importlib.util.spec_from_file_location("montador", sys.argv[1])
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)  # só define constantes/funções; main é protegido
for item in mod.MANIFESTOS["1"]:
    if not item.startswith("@"):  # @SUMARIO/@LISTAS são gerados, não arquivos
        print("docs/entrega/secoes/" + item)
PY
}

# Figuras referenciadas pelo documento do 1º bimestre (![legenda](caminho)).
# A legenda pode conter colchetes, ex.: "![Figura 20. ... [UC001].](figuras/x.png)".
figuras_1bim() {
    existe_em_head "$DOC_1BIM" || return 0
    git show "$ORIGEM:$DOC_1BIM" > "$AUX/doc1.md"
    python3 -I - "$AUX/doc1.md" <<'PY'
import posixpath, re, sys
texto = open(sys.argv[1], encoding="utf-8").read()
padrao = re.compile(r"!\[(?:[^\[\]]|\[[^\[\]]*\])*\]\(\s*<?([^)\s>]+)>?(?:\s+\"[^\"]*\")?\s*\)")
vistos = set()
for caminho in padrao.findall(texto):
    if re.match(r"^[a-z]+://", caminho):
        continue  # imagem externa: nada a copiar
    final = posixpath.normpath(posixpath.join("docs/entrega", caminho))
    if final not in vistos:
        vistos.add(final)
        print(final)
PY
}

SECOES=""
if existe_em_head "$MONTADOR"; then
    SECOES="$(secoes_1bim)" || erro "falha ao ler o manifesto em $MONTADOR."
    [[ -n "$SECOES" ]] || erro "o manifesto do 1º bimestre em $MONTADOR veio vazio."
else
    FALTANDO+=("$MONTADOR (manifesto das seções do 1º bimestre)")
fi
FIGURAS="$(figuras_1bim)" || erro "falha ao extrair as figuras de $DOC_1BIM."
if existe_em_head "$DOC_1BIM" && [[ -z "$FIGURAS" ]]; then
    FALTANDO+=("figuras: nenhuma referência ![](...) encontrada em $DOC_1BIM")
fi
N_SECOES="$(sed '/^$/d' <<< "$SECOES" | wc -l)"
N_FIGURAS="$(sed '/^$/d' <<< "$FIGURAS" | wc -l)"

# Grupos por regex.
: > "$AUX/lista.txt"
for padrao in "${WHITELIST_GRUPOS[@]}"; do
    if ! grep -E -- "$padrao" "$AUX/arvore.txt" >> "$AUX/lista.txt"; then
        FALTANDO+=("nenhum arquivo casa com o padrão $padrao")
    fi
done
# Itens exatos, seções e figuras.
while IFS= read -r caminho; do
    [[ -n "$caminho" ]] || continue
    if existe_em_head "$caminho"; then
        printf '%s\n' "$caminho" >> "$AUX/lista.txt"
    else
        FALTANDO+=("$caminho")
    fi
done < <(printf '%s\n' "${WHITELIST_EXATOS[@]}"; printf '%s\n' "$SECOES" "$FIGURAS")

# O .gitignore é gerado (não copiado) e o README muda de lugar.
grep -vxF -e '.gitignore' "$AUX/lista.txt" | sort -u > "$AUX/copiar.txt" || true
{
    grep -vxF -e "$README_ORIGEM" "$AUX/copiar.txt" || true
    echo ".gitignore"
    if grep -qxF -- "$README_ORIGEM" "$AUX/copiar.txt"; then echo "README.md"; fi
} | LC_ALL=C sort -u > "$AUX/final.txt"

# A opção do CMake precisa existir em HEAD com padrão OFF (será trocada para ON).
# Casa "option(APENAS_PROTOTIPO <qualquer texto> OFF)" numa única linha.
readonly CMAKE_OPCAO_REGEX='^([[:space:]]*option[[:space:]]*\([[:space:]]*APENAS_PROTOTIPO[[:space:]].*[[:space:]])OFF([[:space:]]*\).*)$'
if existe_em_head "CMakeLists.txt"; then
    OCORRENCIAS="$(git show "$ORIGEM:CMakeLists.txt" | grep -cE "$CMAKE_OPCAO_REGEX" || true)"
    if [[ "$OCORRENCIAS" -ne 1 ]]; then
        FALTANDO+=("CMakeLists.txt: linha 'option(APENAS_PROTOTIPO \"...\" OFF)' (encontradas: $OCORRENCIAS, esperado: 1)")
    fi
fi

# CMakeLists.txt da branch: só os alvos do protótipo. Cabeçalho reescrito e
# tudo depois do "return()" do modo APENAS_PROTOTIPO removido, para não
# expor alvos, testes e recursos do sistema completo (que não vão para a
# branch). $1 = CMakeLists.txt de HEAD; $2 = saída. Falha se a estrutura
# esperada não for encontrada.
reduzir_cmake() {
    python3 -I - "$1" "$2" <<'PY'
import re, sys
texto = open(sys.argv[1], encoding="utf-8").read()
CABECALHO = """\
# =============================================================================
# Analisador B3 - prototipo de telas (entrega do 1o bimestre)
# Trabalho de Analise e Projeto de Sistemas (UTFPR)
#
# Alvos:
#   analisador-b3-prototipo  (executavel) - prototipo de telas: interface grafica
#                                           (src/ui) e dominio (src/dominio), com
#                                           dados fixos em memoria (src/prototipo);
#                                           sem SQL nem rede
#   capturar-telas-prototipo (executavel) - salva um PNG de cada tela (modo
#                                           offscreen)
#
# Requisitos: CMake 3.19+, compilador C++20 e Qt 6 (Core, Gui, Widgets, Charts).
#
# Compilar:
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j
#   ./build/analisador-b3-prototipo
# =============================================================================
"""
def trocar(padrao, novo, alvo):
    resultado, n = re.subn(padrao, novo, alvo, flags=re.M | re.S)
    if n != 1:
        sys.exit(f"reduzir_cmake: padrão encontrado {n} vez(es), esperado 1: {padrao[:60]}")
    return resultado
# 1. Cabeçalho: bloco inicial de comentários.
texto = trocar(r"\A(?:#[^\n]*\n)+", lambda m: CABECALHO, texto)
# 2. Opção ligada por padrão.
texto = trocar(r"^([ \t]*option[ \t]*\([ \t]*APENAS_PROTOTIPO[ \t][^\n]*?[ \t])OFF([ \t]*\))", r"\1ON\2", texto)
# 3. Só os módulos do Qt que o protótipo usa; desligar a opção não tem sentido aqui.
texto = trocar(
    r"^if\(APENAS_PROTOTIPO\)\n(\s*find_package\(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts\))\n"
    r"else\(\)\n[^\n]*\nendif\(\)\n",
    lambda m: ("if(NOT APENAS_PROTOTIPO)\n"
               "    message(FATAL_ERROR \"Esta versao contem somente o prototipo de telas: "
               "mantenha APENAS_PROTOTIPO=ON.\")\n"
               "endif()\n" + m.group(1).strip() + "\n"),
    texto)
# 4. Corta tudo após o return() do modo protótipo.
texto = trocar(
    r"^if\(APENAS_PROTOTIPO\)\n\s*(message\(STATUS[^\n]*\))\n\s*return\(\)\n\s*endif\(\)\n.*\Z",
    lambda m: m.group(1) + "\n", texto)
for proibido in ("analisador-nucleo", "src/main.cpp", "testes/", "recursos.qrc",
                 "enable_testing", "gerar-diagramas", "Qt6::Test",
                 "Qt6::Sql Qt6::Network", "Sql Network Test"):
    if proibido in texto:
        sys.exit(f"reduzir_cmake: resto do sistema completo no resultado: {proibido!r}")
open(sys.argv[2], "w", encoding="utf-8").write(texto)
PY
}
if existe_em_head "CMakeLists.txt"; then
    git show "$ORIGEM:CMakeLists.txt" > "$AUX/CMakeLists.head.txt"
    if ! reduzir_cmake "$AUX/CMakeLists.head.txt" "$AUX/CMakeLists.reduzido.txt" 2> "$AUX/cmake.err"; then
        FALTANDO+=("CMakeLists.txt: estrutura inesperada para a redução ($(tr '\n' ' ' < "$AUX/cmake.err"))")
    fi
fi

# ----------------------------------------------------------------------------
# Verificações (sobre a lista final de caminhos)
# ----------------------------------------------------------------------------
verificar_caminhos() {
    local lista="$1" falhas=0 achados i
    # Pares (regex, descrição) das regras de bloqueio.
    local -a regras=(
        "$PROIBIDOS_REGEX"               "material privado/da empresa/testes/SQL"
        "$PROIBIDOS_CPP_REGEX"           "implementação real (.cpp) de analise/persistencia/servicos"
        'documento-2-bimestre'           "documento do 2º bimestre"
        '^src/main\.cpp$'                "main do sistema completo"
        '^src/persistencia/bancodedados\.h$' "header do banco de dados"
    )
    titulo "Verificação de caminhos proibidos"
    for ((i = 0; i < ${#regras[@]}; i += 2)); do
        achados="$(grep -E -- "${regras[i]}" "$lista" || true)"
        if [[ -n "$achados" ]]; then
            printf 'PROIBIDO (%s):\n%s\n' "${regras[i+1]}" "$(sed 's/^/  /' <<< "$achados")" >&2
            falhas=1
        fi
    done
    [[ $falhas -eq 0 ]] || return 1
    info "ok — nenhum caminho proibido."
}

# Conteúdo suspeito: $1 = lista de caminhos; $2 = "HEAD" (lê do commit) ou
# diretório do worktree (lê do disco). Devolve 1 se houver ocorrência.
# Texto: grep sem diferenciar maiúsculas (+ "IA" isolado, com maiúsculas).
# .docx: o XML interno (texto e metadados) passa pelos mesmos padrões.
# PNG e PDF (fluxos comprimidos) não são inspecionados; o PDF sai do mesmo
# .md que o .docx.
verificar_conteudo() {
    local lista="$1" fonte="$2" achados="" base="$2"
    titulo "Conteúdo suspeito ($SUSPEITOS_REGEX_I | $SUSPEITOS_REGEX_CS)"
    grep -vE '\.(png|docx|pdf)$' "$lista" | grep -vxF '.gitignore' > "$AUX/texto.txt" || true
    grep -E '\.docx$' "$lista" > "$AUX/docx.txt" || true
    if [[ "$fonte" == "HEAD" ]]; then
        # README.md não existe com esse nome em HEAD.
        sed -e "s#^README\.md\$#$README_ORIGEM#" "$AUX/texto.txt" > "$AUX/texto-head.txt"
        if [[ -s "$AUX/texto-head.txt" ]]; then
            achados="$(xargs -d '\n' git grep -n -I -i -E "$SUSPEITOS_REGEX_I" "$ORIGEM" -- < "$AUX/texto-head.txt" \
                | sed "s#^$ORIGEM:##" || true)"
            achados+=$'\n'"$(xargs -d '\n' git grep -n -I -E "$SUSPEITOS_REGEX_CS" "$ORIGEM" -- < "$AUX/texto-head.txt" \
                | sed "s#^$ORIGEM:##" || true)"
        fi
        # Os .docx são extraídos do commit para um diretório auxiliar.
        base="$AUX/docx-head"
        while IFS= read -r caminho; do
            [[ -n "$caminho" ]] || continue
            mkdir -p "$base/$(dirname "$caminho")"
            git show "$ORIGEM:$caminho" > "$base/$caminho"
        done < "$AUX/docx.txt"
    else
        if [[ -s "$AUX/texto.txt" ]]; then
            achados="$(cd "$fonte" && xargs -d '\n' grep -n -I -i -E -H -- "$SUSPEITOS_REGEX_I" < "$AUX/texto.txt" || true)"
            achados+=$'\n'"$(cd "$fonte" && xargs -d '\n' grep -n -I -E -H -- "$SUSPEITOS_REGEX_CS" < "$AUX/texto.txt" || true)"
        fi
        if [[ -f "$fonte/.gitignore" ]] && grep -qiE "$SUSPEITOS_GITIGNORE_REGEX" "$fonte/.gitignore"; then
            achados+=$'\n.gitignore: contém termo de autoria/empresa'
        fi
    fi
    if grep -qiE "$SUSPEITOS_GITIGNORE_REGEX" <<< "$GITIGNORE_ENTREGA"; then
        achados+=$'\n.gitignore (gerado): contém termo de autoria/empresa'
    fi
    if [[ -s "$AUX/docx.txt" ]]; then
        achados+=$'\n'"$(python3 -I - "$base" "$AUX/docx.txt" "$SUSPEITOS_REGEX_I" "$SUSPEITOS_REGEX_CS" <<'PY'
import re, sys, zipfile
# O programa vem pela entrada padrão (heredoc); a lista de .docx, por arquivo.
base, lista = sys.argv[1], sys.argv[2]
regex_i, regex_cs = re.compile(sys.argv[3], re.I), re.compile(sys.argv[4])
for caminho in filter(None, (l.strip() for l in open(lista, encoding="utf-8"))):
    with zipfile.ZipFile(f"{base}/{caminho}") as z:
        for membro in z.namelist():
            if not membro.endswith((".xml", ".rels")):
                continue
            texto = re.sub(r"<[^>]+>", " ", z.read(membro).decode("utf-8", "replace"))
            for m in list(regex_i.finditer(texto)) + list(regex_cs.finditer(texto)):
                print(f"{caminho}[{membro}]: ...{texto[max(0, m.start()-40):m.end()+40]}...")
PY
)"
    fi
    achados="$(sed '/^[[:space:]]*$/d' <<< "$achados")"
    if [[ -n "$achados" ]]; then
        printf '\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n'
        printf '!! ATENÇÃO: %d ocorrência(s) para revisar antes do push:\n' "$(wc -l <<< "$achados")"
        sed 's/^/!!   /' <<< "$achados"
        printf '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n'
        return 1
    fi
    info "ok — nenhuma ocorrência."
}

relatar_faltando() {
    [[ ${#FALTANDO[@]} -gt 0 ]] || return 0
    titulo "FALTANDO em HEAD ($ORIGEM_CURTO) — ${#FALTANDO[@]} item(ns)"
    printf '  - %s\n' "${FALTANDO[@]}" >&2
    return 1
}

# ----------------------------------------------------------------------------
# Modo --dry-run: só lista e verifica
# ----------------------------------------------------------------------------
if [[ $DRY_RUN -eq 1 ]]; then
    titulo "Dry-run: branch '$BRANCH' a partir de $ORIGEM_NOME ($ORIGEM_CURTO)"
    if [[ -n "$(git status --porcelain --untracked-files=no)" ]]; then
        info "Aviso: há alterações não commitadas; elas NÃO entram (origem é HEAD)."
    fi
    if git show-ref --verify --quiet "refs/heads/$BRANCH"; then
        info "Aviso: a branch '$BRANCH' já existe; o modo real exigirá --forcar."
    fi
    titulo "Arquivos que entrariam"
    cat "$AUX/final.txt"
    info ""
    info "Total: $(wc -l < "$AUX/final.txt") arquivo(s) ($N_SECOES seções do 1º bim, $N_FIGURAS figuras; .gitignore gerado; README.md vem de $README_ORIGEM)."
    STATUS=0
    verificar_caminhos "$AUX/final.txt" || STATUS=1
    CONTEUDO_OK=1
    verificar_conteudo "$AUX/final.txt" "HEAD" || CONTEUDO_OK=0
    relatar_faltando || STATUS=2
    if [[ $STATUS -eq 0 && $CONTEUDO_OK -eq 0 && $ACEITAR_AVISOS -eq 0 ]]; then
        STATUS=3
    fi
    if [[ $STATUS -eq 0 ]]; then
        titulo "Dry-run OK — pronto para gerar a branch (rode sem --dry-run)"
    elif [[ $STATUS -eq 1 ]]; then
        printf '\nDry-run com caminhos proibidos: o modo real abortaria.\n' >&2
    elif [[ $STATUS -eq 3 ]]; then
        printf '\nDry-run com conteúdo suspeito: o modo real abortaria (revise ou use --aceitar-avisos).\n' >&2
    else
        printf '\nDry-run incompleto: commite os itens faltando antes do modo real.\n' >&2
    fi
    exit "$STATUS"
fi

# ----------------------------------------------------------------------------
# Modo real
# ----------------------------------------------------------------------------
relatar_faltando || erro "whitelist incompleta em HEAD; nada foi criado."
verificar_caminhos "$AUX/final.txt" || erro "a whitelist contém caminhos proibidos; nada foi criado."

# 2. Worktree temporário com branch órfã. Usa um nome temporário e só no fim
#    renomeia para $BRANCH: se algo falhar, a branch antiga (com --forcar)
#    continua intacta.
WT="$(mktemp -d)"
BRANCH_TEMP="exportacao-tmp-$$-$(date +%s)"
titulo "Criando worktree temporário em $WT"
git worktree add -q --detach "$WT" "$ORIGEM"
git -C "$WT" checkout -q --orphan "$BRANCH_TEMP"
git -C "$WT" rm -rf --cached -q .
find "$WT" -mindepth 1 -maxdepth 1 ! -name .git -exec rm -rf {} +

# 3. Copia a whitelist do commit (não do disco).
# Uma única chamada do git archive: com xargs, tars concatenados fariam o tar
# parar no primeiro marcador de fim.
mapfile -t COPIAR < "$AUX/copiar.txt"
git -C "$RAIZ" archive --format=tar "$ORIGEM" -- "${COPIAR[@]}" | tar -x -C "$WT"

# CMakeLists.txt reduzido aos alvos do protótipo, com APENAS_PROTOTIPO ligado
# por padrão (gerado e validado na etapa 1).
cp "$AUX/CMakeLists.reduzido.txt" "$WT/CMakeLists.txt"
grep -qE '^[[:space:]]*option[[:space:]]*\([[:space:]]*APENAS_PROTOTIPO[[:space:]].*[[:space:]]ON[[:space:]]*\)' "$WT/CMakeLists.txt" \
    || erro "não consegui ligar APENAS_PROTOTIPO no CMakeLists.txt."

printf '%s\n' "$GITIGNORE_ENTREGA" > "$WT/.gitignore"
mv "$WT/$README_ORIGEM" "$WT/README.md"

git -C "$WT" add -A
git -C "$WT" -c core.quotePath=false ls-files | LC_ALL=C sort > "$AUX/indexados.txt"

# Confere que o índice bate exatamente com a lista planejada.
if ! diff -u "$AUX/final.txt" "$AUX/indexados.txt" > "$AUX/diff.txt"; then
    cat "$AUX/diff.txt" >&2
    erro "o conteúdo do worktree difere da lista planejada."
fi
verificar_caminhos "$AUX/indexados.txt" || erro "caminho proibido no índice; nada foi publicado."
if ! verificar_conteudo "$AUX/indexados.txt" "$WT"; then
    [[ $ACEITAR_AVISOS -eq 1 ]] \
        || erro "conteúdo suspeito na branch; revise as ocorrências acima ou rode com --aceitar-avisos."
    info "Seguindo com --aceitar-avisos."
fi

# 4. Commit único com autor e committer Sergio, sem trailers de coautoria.
env -u GIT_AUTHOR_NAME -u GIT_AUTHOR_EMAIL -u GIT_COMMITTER_NAME -u GIT_COMMITTER_EMAIL \
    git -C "$WT" -c user.name="$AUTOR_NOME" -c user.email="$AUTOR_EMAIL" \
    commit -q -m "$MENSAGEM_COMMIT"

HASH="$(git -C "$WT" rev-parse HEAD)"
[[ "$(git -C "$WT" rev-list --count HEAD)" -eq 1 ]] || erro "a branch gerada não tem exatamente 1 commit."
if git -C "$WT" log -1 --format=%B | grep -qiE 'co-authored-by|claude|anthropic'; then
    erro "a mensagem do commit ganhou trailer/menção indevida (hook?)."
fi
[[ "$(git -C "$WT" log -1 --format='%an <%ae>|%cn <%ce>')" == "$AUTOR_NOME <$AUTOR_EMAIL>|$AUTOR_NOME <$AUTOR_EMAIL>" ]] \
    || erro "autor/committer do commit não é $AUTOR_NOME <$AUTOR_EMAIL>."

# Remove o worktree e dá o nome definitivo à branch.
git worktree remove --force "$WT"
WT=""
git branch -M "$BRANCH_TEMP" "$BRANCH"
BRANCH_TEMP=""
if [[ -n "$(git merge-base "$ORIGEM" "$BRANCH" 2>/dev/null || true)" ]]; then
    erro "a branch '$BRANCH' tem ancestral comum com a origem — não é órfã."
fi

titulo "Branch gerada"
info "Branch:   $BRANCH"
info "Commit:   $HASH"
info "Arquivos: $(wc -l < "$AUX/indexados.txt")"
info "Origem:   $ORIGEM_NOME ($ORIGEM_CURTO)"
cat <<EOF

Próximos passos (manuais):
  1. Compilar do zero num diretório limpo:
       d=\$(mktemp -d) && git worktree add "\$d" $BRANCH
       cmake -S "\$d" -B "\$d/build" -DCMAKE_BUILD_TYPE=Release && cmake --build "\$d/build" -j8
       git worktree remove --force "\$d"
  2. Revisar os avisos de conteúdo acima (se houver; só com --aceitar-avisos).
  3. Publicar: git push -u origin $BRANCH   (com --forcar: git push -f origin $BRANCH)
  4. Conferir: git log --oneline $BRANCH | wc -l  → 1 ; git merge-base main $BRANCH → vazio
EOF
