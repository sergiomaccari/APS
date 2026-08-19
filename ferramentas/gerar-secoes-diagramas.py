#!/usr/bin/env python3
"""Gera as seções de Diagramas de Estados e de Atividades dos documentos a
partir das molduras dos JSONs em docs/entrega/diagramas/.

Moldura esperada:  est → "stm UCxxx | Classe"
                   atv → "act UCxxx | Classe | Estado"
"""

import json
import re
import sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
DIAGRAMAS = RAIZ / "docs/entrega/diagramas"
SECOES = RAIZ / "docs/entrega/secoes"


def ordem(classe: str) -> int:
    c = classe.lower()
    if c in ("importacao", "recomendacao", "posicao", "alerta"):
        return 2  # domínio por último
    if c.startswith(("tela", "dialogo", "janela")):
        return 0  # fronteira primeiro
    return 1      # controle no meio


def coletar(prefixo: str):
    itens = []
    for arq in sorted(DIAGRAMAS.glob(f"{prefixo}-uc*.json")):
        moldura = json.loads(arq.read_text(encoding="utf-8")).get("moldura", "")
        partes = [p.strip() for p in moldura.split("|")]
        m = re.match(r"(?:stm|act)\s+(UC\d+)", partes[0])
        if not m or len(partes) < (2 if prefixo == "est" else 3):
            sys.exit(f"ERRO: moldura fora do padrão em {arq.name}: '{moldura}'")
        uc = m.group(1)
        classe = partes[1]
        estado = partes[2] if len(partes) > 2 else None
        itens.append((uc, ordem(classe), classe, estado, arq.stem))
    itens.sort(key=lambda t: (t[0], t[1], t[2]))
    return itens


estados = coletar("est")
atividades = coletar("atv")

linhas = ["### Diagramas de Estados", ""]
linhas.append(
    "Seguindo a convenção da disciplina, cada caso de uso possui Diagramas de "
    "Estados das suas classes participantes relevantes — a classe de fronteira "
    "(tela ou diálogo) e a classe de controle (serviço ou repositório) — "
    "derivados do respectivo Diagrama de Sequência. Além deles, são apresentadas "
    "as máquinas de estado das classes de domínio que possuem ciclo de vida "
    "próprio implementado no sistema: Importacao (UC005), Recomendacao (UC011), "
    "Posicao (UC014) e Alerta (UC017). As transições seguem a notação "
    "`evento [guarda] / ação`, com os eventos correspondendo aos métodos reais "
    "das classes.")
linhas.append("")
for uc, _, classe, _, nome in estados:
    linhas.append(f"![Figura {{{{F:{nome}}}}}. Diagrama de Estados da classe "
                  f"{classe} para o [{uc}].](figuras/{nome}.png)")
    linhas.append("")
(SECOES / "13-modelagem-estados.md").write_text("\n".join(linhas), encoding="utf-8")

linhas = ["### Diagramas de Atividades", ""]
linhas.append(
    "Cada Diagrama de Estados tem seu estado principal detalhado por um "
    "Diagrama de Atividades, conforme a convenção da disciplina. As ações "
    "correspondem aos passos reais dos métodos implementados, e as decisões "
    "reproduzem as validações e regras de negócio citadas na especificação dos "
    "casos de uso.")
linhas.append("")
for uc, _, classe, estado, nome in atividades:
    linhas.append(f"![Figura {{{{F:{nome}}}}}. Diagrama de Atividades para o "
                  f"estado \"{estado}\", dentro do Diagrama de Estados para a "
                  f"classe {classe} em [{uc}].](figuras/{nome}.png)")
    linhas.append("")
(SECOES / "14-modelagem-atividades.md").write_text("\n".join(linhas), encoding="utf-8")

print(f"estados: {len(estados)} figuras · atividades: {len(atividades)} figuras")
pares_est = {(uc, c) for uc, _, c, _, _ in estados}
pares_atv = {(uc, c) for uc, _, c, _, _ in atividades}
if pares_est != pares_atv:
    print("AVISO: estados sem atividade correspondente:", pares_est - pares_atv)
    print("AVISO: atividades sem estado correspondente:", pares_atv - pares_est)
