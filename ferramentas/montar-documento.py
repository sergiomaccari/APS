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
    "1": ["00-capa-1bim.md", "@LISTAS"] + MIOLO_COMUM
         + ["07-resultados-abre-1bim.md"] + MODELAGEM_1BIM
         + ["15-conclusoes-1bim.md", "16-referencias.md"],
    "2": ["00-capa-2bim.md", "@LISTAS"] + MIOLO_COMUM
         + ["07-resultados-abre-2bim.md"] + MODELAGEM_1BIM
         + ["11-modelagem-comunicacao.md", "12-modelagem-der.md",
            "13-modelagem-estados.md", "14-modelagem-atividades.md",
            "15-conclusoes-2bim.md", "16-referencias.md"],
}


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


def montar(bim: str) -> Path:
    partes = []
    for nome in MANIFESTOS[bim]:
        if nome == "@LISTAS":
            partes.append("@LISTAS")
            continue
        arquivo = SECOES / nome
        if not arquivo.exists():
            sys.exit(f"ERRO: seção ausente: {arquivo}")
        partes.append(arquivo.read_text(encoding="utf-8").strip())

    corpo = "\n\n".join(p for p in partes if p != "@LISTAS")
    corpo, legendas_fig, legendas_tab = numerar(corpo)

    listas = ["# Lista de Figuras", ""]
    listas += [f"{l}  " for l in legendas_fig]
    listas += ["", "\\newpage", "", "# Lista de Tabelas e Quadros", ""]
    listas += [f"{l}  " for l in legendas_tab]
    listas += ["", "\\newpage"]
    bloco_listas = "\n".join(listas)

    # Reinsere as listas na posição do marcador (após a capa).
    saida_md = []
    for p in partes:
        if p == "@LISTAS":
            saida_md.append("@@LISTAS@@")
        else:
            saida_md.append(p)
    documento = "\n\n".join(saida_md)
    documento, _, _ = numerar(documento)
    documento = documento.replace("@@LISTAS@@", bloco_listas)

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
    cmd = [pandoc, str(md), "-o", str(docx),
           "--resource-path", str(SAIDA),
           "--toc", "--toc-depth=3",
           "-V", "lang=pt-BR",
           "--metadata", "title=AB3 — Analisador de Ativos da B3"]
    subprocess.run(cmd, check=True, cwd=SAIDA)
    print(f"  {docx.name} gerado ({docx.stat().st_size // 1024} KB)")


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
