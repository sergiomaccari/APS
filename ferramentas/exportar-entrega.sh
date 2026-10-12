#!/usr/bin/env bash
# =============================================================================
# exportar-entrega.sh — gera a branch órfã da entrega de 13/10 (protótipo).
#
# Uso:
#   ferramentas/exportar-entrega.sh [--dry-run] [--branch entrega-13-10] [--forcar]
#
#   --dry-run   só lista (ordenado) os arquivos que entrariam, com a contagem,
#               e roda as verificações de proibidos; não cria nada.
#   --branch    nome da branch a gerar (padrão: entrega-13-10).
#   --forcar    recria a branch se ela já existir (sem isso, aborta).
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
#   3. extrai a whitelist com git archive, liga APENAS_PROTOTIPO no
#      CMakeLists.txt, gera um .gitignore reduzido e move
#      docs/entrega/README-entrega.md para README.md;
#   4. verifica que nada proibido entrou e faz um commit único com autor
#      Sergio Maccari; remove o worktree.
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

# Caminhos que NUNCA podem entrar na branch pública (RP-09).
readonly PROIBIDOS_REGEX='^(inicio\.txt|APS/|\.claude/|\.agents/|padroes/|design-system/|docs/analises/|testes/|recursos/sql/)'
readonly PROIBIDOS_CPP_REGEX='^src/(analise|persistencia|servicos)/.*\.cpp$'
# Conteúdo que merece revisão humana antes de publicar (só aviso). Sensível a
# maiúsculas de propósito: ".claude/" no .gitignore gerado não é menção.
readonly SUSPEITOS_REGEX='Co-Authored-By|Claude|Anthropic|[Dd]riva|claude-automacoes'

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
# reduzida mantém só o necessário para quem compila o protótipo.
read -r -d '' GITIGNORE_ENTREGA <<'FIM' || true
# Build do CMake e artefatos locais
build/
build-*/
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
FIM
readonly GITIGNORE_ENTREGA

# ----------------------------------------------------------------------------
# Utilitários de saída
# ----------------------------------------------------------------------------
erro() { printf '\nERRO: %s\n' "$*" >&2; exit 1; }
info() { printf '%s\n' "$*"; }
titulo() { printf '\n== %s ==\n' "$*"; }

uso() {
    sed -n '4,11p' "$0" | sed 's/^# \{0,1\}//'
}

# ----------------------------------------------------------------------------
# Argumentos
# ----------------------------------------------------------------------------
DRY_RUN=0
FORCAR=0
BRANCH="entrega-13-10"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --dry-run) DRY_RUN=1 ;;
        --forcar) FORCAR=1 ;;
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
git ls-tree -r --name-only "$ORIGEM" > "$AUX/arvore.txt"

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

# Aviso de conteúdo: $1 = lista de caminhos; $2 = "HEAD" (lê do commit) ou
# diretório do worktree (lê do disco).
verificar_conteudo() {
    local lista="$1" fonte="$2" achados=""
    titulo "Conteúdo a revisar ($SUSPEITOS_REGEX)"
    grep -vE '\.(png|docx|pdf)$' "$lista" > "$AUX/texto.txt" || true
    if [[ "$fonte" == "HEAD" ]]; then
        # README.md e .gitignore não existem com esse nome em HEAD.
        sed -e "s#^README\.md\$#$README_ORIGEM#" -e '/^\.gitignore$/d' "$AUX/texto.txt" > "$AUX/texto-head.txt"
        if [[ -s "$AUX/texto-head.txt" ]]; then
            achados="$(xargs -d '\n' git grep -n -I -E "$SUSPEITOS_REGEX" "$ORIGEM" -- < "$AUX/texto-head.txt" \
                | sed "s#^$ORIGEM:##" || true)"
        fi
        if grep -qE "$SUSPEITOS_REGEX" <<< "$GITIGNORE_ENTREGA"; then
            achados+=$'\n.gitignore (gerado): contém termo suspeito'
        fi
    else
        achados="$(cd "$fonte" && xargs -d '\n' grep -n -I -E -H -- "$SUSPEITOS_REGEX" < "$AUX/texto.txt" || true)"
    fi
    achados="$(sed '/^$/d' <<< "$achados")"
    if [[ -n "$achados" ]]; then
        printf '\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n'
        printf '!! ATENÇÃO: %d ocorrência(s) para revisar antes do push:\n' "$(wc -l <<< "$achados")"
        sed 's/^/!!   /' <<< "$achados"
        printf '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n'
    else
        info "ok — nenhuma ocorrência."
    fi
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
    verificar_conteudo "$AUX/final.txt" "HEAD"
    relatar_faltando || STATUS=2
    if [[ $STATUS -eq 0 ]]; then
        titulo "Dry-run OK — pronto para gerar a branch (rode sem --dry-run)"
    elif [[ $STATUS -eq 1 ]]; then
        printf '\nDry-run com caminhos proibidos: o modo real abortaria.\n' >&2
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

# APENAS_PROTOTIPO ligado por padrão na branch da entrega.
sed -E -i "s/$CMAKE_OPCAO_REGEX/\\1ON\\2/" "$WT/CMakeLists.txt"
grep -qE '^[[:space:]]*option[[:space:]]*\([[:space:]]*APENAS_PROTOTIPO[[:space:]].*[[:space:]]ON[[:space:]]*\)' "$WT/CMakeLists.txt" \
    || erro "não consegui ligar APENAS_PROTOTIPO no CMakeLists.txt."

printf '%s\n' "$GITIGNORE_ENTREGA" > "$WT/.gitignore"
mv "$WT/$README_ORIGEM" "$WT/README.md"

git -C "$WT" add -A
git -C "$WT" ls-files | LC_ALL=C sort > "$AUX/indexados.txt"

# Confere que o índice bate exatamente com a lista planejada.
if ! diff -u "$AUX/final.txt" "$AUX/indexados.txt" > "$AUX/diff.txt"; then
    cat "$AUX/diff.txt" >&2
    erro "o conteúdo do worktree difere da lista planejada."
fi
verificar_caminhos "$AUX/indexados.txt" || erro "caminho proibido no índice; nada foi publicado."
verificar_conteudo "$AUX/indexados.txt" "$WT"

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
  2. Revisar os avisos de conteúdo acima (se houver).
  3. Publicar: git push -u origin $BRANCH   (com --forcar: git push -f origin $BRANCH)
  4. Conferir: git log --oneline $BRANCH | wc -l  → 1 ; git merge-base main $BRANCH → vazio
EOF
