#pragma once

// Papel (perfil de acesso) de um usuario do Analisador B3.
// Os textos retornados por paraTexto() sao exatamente os valores gravados na
// coluna usuario.papel do banco de dados (ver recursos/sql/001_esquema_inicial.sql).

#include <QString>

namespace analisador
{

enum class PapelUsuario
{
    Investidor,
    Administrador
};

// Converte o papel para o texto canonico usado na interface e na persistencia.
inline QString paraTexto(PapelUsuario papel)
{
    switch (papel)
    {
    case PapelUsuario::Administrador:
        return QStringLiteral("Administrador");
    case PapelUsuario::Investidor:
        break;
    }
    return QStringLiteral("Investidor");
}

// Converte o texto lido do banco (ou da interface) para o papel correspondente.
// Qualquer valor desconhecido cai no papel menos privilegiado (Investidor).
inline PapelUsuario papelUsuarioDeTexto(const QString& texto)
{
    const QString normalizado = texto.trimmed();
    if (normalizado.compare(QStringLiteral("Administrador"), Qt::CaseInsensitive) == 0)
    {
        return PapelUsuario::Administrador;
    }
    return PapelUsuario::Investidor;
}

}
