#!/usr/bin/env python3
"""Monta os documentos entregáveis a partir das seções em docs/entrega/secoes/.

Uso:  python3 ferramentas/montar-documento.py [1|2|ambos] [--docx caminho/do/pandoc]

O montador:
  1. concatena as seções na ordem do modelo do professor;
  2. substitui os tokens de numeração — {{Q}} (quadros, sequencial),
     {{T:rotulo}} (tabelas) e {{F:rotulo}} (figuras, numeradas pela 1ª aparição);
  3. gera as listas de figuras e de tabelas/quadros a partir das legendas;
  4. grava docs/entrega/documento-<N>-bimestre.md e, com pandoc disponível,
     o .docx correspondente.
"""

import re
import subprocess
import sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
SECOES = RAIZ / "docs/entrega/secoes"
SAIDA = RAIZ / "docs/entrega"

MIOLO_COMUM = [
    "01-abreviaturas.md",
    "02-introducao.md",
    "03-levantamento-questoes.md",
    "04-requisitos.md",
    "05-estimativa.md",
    "06-viabilidade.md",
]
MODELAGEM_1BIM = [
    "quadros-uc-01-09.md",
    "quadros-uc-10-18.md",
    "08-modelagem-classes.md",
    "dicionario-parte1.md",
    "dicionario-parte2.md",
    "09-modelagem-objetos.md",
    "10-modelagem-sequencia.md",
]
MANIFESTOS = {
    "1": ["00-capa-1bim.md", "@SUMARIO", "@LISTAS"] + MIOLO_COMUM
         + ["07-resultados-abre-1bim.md"] + MODELAGEM_1BIM
         + ["15-conclusoes-1bim.md", "16-referencias.md"],
    "2": ["00-capa-2bim.md", "@SUMARIO", "@LISTAS"] + MIOLO_COMUM
         + ["07-resultados-abre-2bim.md"] + MODELAGEM_1BIM
         + ["11-modelagem-comunicacao.md", "12-modelagem-der.md",
            "13-modelagem-estados.md", "14-modelagem-atividades.md",
            "15-conclusoes-2bim.md", "16-referencias.md"],
}

# Quebra de página nos dois formatos (o docx ignora LaTeX cru e vice-versa).
QUEBRA = ('```{=openxml}\n<w:p><w:r><w:br w:type="page"/></w:r></w:p>\n```\n\n'
          '```{=latex}\n\\newpage\n```')

# Sumário posicionado após a capa, como no modelo do professor. No Word ele é
# um campo TOC: abrir o documento e atualizar campos (F9) preenche as páginas.
SUMARIO = ('```{=openxml}\n'
           '<w:p><w:pPr><w:jc w:val="center"/></w:pPr><w:r><w:rPr><w:b/>'
           '<w:sz w:val="28"/></w:rPr><w:t>SUMÁRIO</w:t></w:r></w:p>\n'
           '<w:sdt><w:sdtPr><w:docPartObj>'
           '<w:docPartGallery w:val="Table of Contents"/><w:docPartUnique/>'
           '</w:docPartObj></w:sdtPr><w:sdtContent><w:p>'
           '<w:r><w:fldChar w:fldCharType="begin" w:dirty="true"/></w:r>'
           '<w:r><w:instrText xml:space="preserve"> TOC \\o "1-3" \\h \\z \\u '
           '</w:instrText></w:r><w:r><w:fldChar w:fldCharType="separate"/></w:r>'
           '<w:r><w:t>Sumário gerado pelo Word: clique com o botão direito e '
           'escolha "Atualizar campo" (ou selecione tudo e pressione F9).</w:t>'
           '</w:r><w:r><w:fldChar w:fldCharType="end"/></w:r></w:p>'
           '</w:sdtContent></w:sdt>\n```\n\n'
           '```{=latex}\n'
           '\\renewcommand*\\contentsname{SUMÁRIO}\n'
           '\\setcounter{tocdepth}{3}\n'
           '\\tableofcontents\n```')


def numerar(texto: str):
    """Resolve {{Q}}, {{T:x}} e {{F:x}}; devolve (texto, figuras, tabelas_quadros)."""
    figuras: dict[str, int] = {}
    tabelas: dict[str, int] = {}
    contador_quadro = 0

    def num_figura(m):
        rotulo = m.group(1)
        figuras.setdefault(rotulo, len(figuras) + 1)
        return str(figuras[rotulo])

    def num_tabela(m):
        rotulo = m.group(1)
        tabelas.setdefault(rotulo, len(tabelas) + 1)
        return str(tabelas[rotulo])

    def num_quadro(_):
        nonlocal contador_quadro
        contador_quadro += 1
        return f"Quadro {contador_quadro}."

    texto = re.sub(r"\{\{F:([\w-]+)\}\}", num_figura, texto)
    texto = re.sub(r"\{\{T:([\w-]+)\}\}", num_tabela, texto)
    texto = re.sub(r"Quadro \{\{Q\}\}\.", num_quadro, texto)

    legendas_fig = re.findall(r"!\[(Figura \d+\..*?)\]\(", texto)
    legendas_tab = re.findall(r"\*\*((?:Tabela|Quadro) \d+\..*?)\*\*", texto)

    # Ordena a lista de tabelas e quadros pela posição no documento.
    return texto, legendas_fig, legendas_tab


def paginar(documento: str) -> str:
    """Remove os \\newpage soltos das seções e insere quebra de página (nos
    dois formatos) antes de cada capítulo (título de nível 1)."""
    linhas = []
    dentro_de_cerca = False
    for linha in documento.split("\n"):
        if linha.startswith("```"):
            dentro_de_cerca = not dentro_de_cerca
            linhas.append(linha)
            continue
        if not dentro_de_cerca and linha.strip() == "\\newpage":
            continue
        if not dentro_de_cerca and linha.startswith("# "):
            linhas += ["", QUEBRA, ""]
        linhas.append(linha)
    return "\n".join(linhas)


def montar(bim: str) -> Path:
    partes = []
    for nome in MANIFESTOS[bim]:
        if nome.startswith("@"):
            partes.append(nome)
            continue
        arquivo = SECOES / nome
        if not arquivo.exists():
            sys.exit(f"ERRO: seção ausente: {arquivo}")
        partes.append(arquivo.read_text(encoding="utf-8").strip())

    corpo = "\n\n".join(p for p in partes if not p.startswith("@"))
    corpo, legendas_fig, legendas_tab = numerar(corpo)

    listas = ["# LISTA DE FIGURAS", ""]
    listas += [f"{l}  " for l in legendas_fig]
    listas += ["", "# LISTA DE TABELAS E QUADROS", ""]
    listas += [f"{l}  " for l in legendas_tab]
    bloco_listas = "\n".join(listas)

    # Reinsere sumário e listas na posição dos marcadores (após a capa).
    saida_md = []
    for p in partes:
        if p == "@LISTAS":
            saida_md.append("@@LISTAS@@")
        elif p == "@SUMARIO":
            saida_md.append("@@SUMARIO@@")
        else:
            saida_md.append(p)
    documento = "\n\n".join(saida_md)
    documento, _, _ = numerar(documento)
    documento = documento.replace("@@LISTAS@@", bloco_listas)
    documento = paginar(documento)
    documento = documento.replace("@@SUMARIO@@", SUMARIO)

    destino = SAIDA / f"documento-{bim}-bimestre.md"
    destino.write_text(documento + "\n", encoding="utf-8")
    faltando = [m.group(0) for m in re.finditer(r"\{\{[^}]+\}\}", documento)]
    if faltando:
        print(f"  AVISO: tokens não resolvidos: {sorted(set(faltando))}")
    print(f"  {destino.name}: {len(legendas_fig)} figuras, "
          f"{len(legendas_tab)} tabelas/quadros")
    return destino


def gerar_docx(md: Path, pandoc: str):
    docx = md.with_suffix(".docx")
    cmd = [pandoc, "--from", "markdown-tex_math_dollars", str(md), "-o", str(docx),
           "--resource-path", str(SAIDA),
           "--reference-doc", str(RAIZ / "ferramentas/referencia.docx"),
           "-V", "lang=pt-BR"]
    subprocess.run(cmd, check=True, cwd=SAIDA)
    print(f"  {docx.name} gerado ({docx.stat().st_size // 1024} KB)")


def gerar_pdf(md: Path, pandoc: str):
    """PDF via pandoc + tectonic. O marcador 🟦 não existe nas fontes LaTeX;
    vira [PENDÊNCIA DA EQUIPE] apenas na renderização do PDF."""
    pdf = md.with_suffix(".pdf")
    texto = md.read_text(encoding="utf-8").replace("🟦", "[PENDÊNCIA DA EQUIPE]")
    temporario = md.with_suffix(".pdf.tmp.md")
    temporario.write_text(texto, encoding="utf-8")
    cmd = [pandoc, "--from", "markdown-tex_math_dollars", str(temporario), "-o", str(pdf),
           "--resource-path", str(SAIDA),
           "--pdf-engine", "tectonic",
           "-V", "lang=pt-BR",
           "-V", "geometry:margin=2.5cm",
           "-V", "fontsize=11pt"]
    # Arial (a fonte do modelo do professor), servida pelo Windows via WSL.
    arial = Path("/mnt/c/Windows/Fonts/arial.ttf")
    if arial.exists():
        cmd += ["-V", "mainfont=arial.ttf",
                "-V", f"mainfontoptions=Path={arial.parent}/",
                "-V", "mainfontoptions=BoldFont=arialbd.ttf",
                "-V", "mainfontoptions=ItalicFont=ariali.ttf",
                "-V", "mainfontoptions=BoldItalicFont=arialbi.ttf"]
    else:
        cmd += ["-V", "mainfont=DejaVu Sans"]
    try:
        subprocess.run(cmd, check=True, cwd=SAIDA)
    finally:
        temporario.unlink(missing_ok=True)
    print(f"  {pdf.name} gerado ({pdf.stat().st_size // 1024} KB)")


if __name__ == "__main__":
    alvo = sys.argv[1] if len(sys.argv) > 1 and sys.argv[1] in ("1", "2") else "ambos"
    pandoc = None
    if "--docx" in sys.argv:
        pandoc = sys.argv[sys.argv.index("--docx") + 1]
    for bim in (["1", "2"] if alvo == "ambos" else [alvo]):
        print(f"Montando documento do {bim}º bimestre…")
        md = montar(bim)
        if pandoc:
            gerar_docx(md, pandoc)
            if "--pdf" in sys.argv:
                gerar_pdf(md, pandoc)
