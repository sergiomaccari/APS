#include "dominio/regraconfigurada.h"

#include <QLocale>

namespace analisador
{

RegraConfigurada::RegraConfigurada()
{
}

RegraConfigurada::RegraConfigurada(qint64 id,
                                   const QString& nomeRegra,
                                   bool ativa,
                                   double parametroPrincipal,
                                   double parametroSecundario)
    : m_id(id)
    , m_nomeRegra(nomeRegra)
    , m_ativa(ativa)
    , m_parametroPrincipal(parametroPrincipal)
    , m_parametroSecundario(parametroSecundario)
{
}

qint64 RegraConfigurada::id() const
{
    return m_id;
}

QString RegraConfigurada::nomeRegra() const
{
    return m_nomeRegra;
}

bool RegraConfigurada::ativa() const
{
    return m_ativa;
}

double RegraConfigurada::parametroPrincipal() const
{
    return m_parametroPrincipal;
}

double RegraConfigurada::parametroSecundario() const
{
    return m_parametroSecundario;
}

void RegraConfigurada::definirId(qint64 id)
{
    m_id = id;
}

void RegraConfigurada::definirNomeRegra(const QString& nomeRegra)
{
    m_nomeRegra = nomeRegra.trimmed();
}

void RegraConfigurada::definirAtiva(bool ativa)
{
    m_ativa = ativa;
}

void RegraConfigurada::definirParametroPrincipal(double parametroPrincipal)
{
    m_parametroPrincipal = parametroPrincipal;
}

void RegraConfigurada::definirParametroSecundario(double parametroSecundario)
{
    m_parametroSecundario = parametroSecundario;
}

bool RegraConfigurada::valida() const
{
    return !m_nomeRegra.isEmpty() && m_parametroPrincipal >= 0.0 && m_parametroSecundario >= 0.0;
}

QString RegraConfigurada::descricao() const
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    return QStringLiteral("%1 (%2 / %3) - %4")
        .arg(m_nomeRegra,
             brasil.toString(m_parametroPrincipal, 'f', 2),
             brasil.toString(m_parametroSecundario, 'f', 2),
             m_ativa ? QStringLiteral("ativa") : QStringLiteral("inativa"));
}

}
