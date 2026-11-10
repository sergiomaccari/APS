#include "ui/widgets/cartaoindicador.h"

#include <QLabel>
#include <QVBoxLayout>

namespace analisador
{

CartaoIndicador::CartaoIndicador(const QString& titulo, QWidget* pai)
    : QFrame(pai)
{
    setObjectName(QStringLiteral("cartaoIndicador"));
    setFrameShape(QFrame::StyledPanel);
    setMinimumWidth(160);

    m_rotuloTitulo = new QLabel(titulo, this);
    m_rotuloTitulo->setObjectName(QStringLiteral("tituloCartao"));

    m_rotuloValor = new QLabel(QStringLiteral("--"), this);
    m_rotuloValor->setObjectName(QStringLiteral("valorCartao"));

    m_rotuloDetalhe = new QLabel(QString(), this);
    m_rotuloDetalhe->setObjectName(QStringLiteral("detalheCartao"));

    auto* disposicao = new QVBoxLayout(this);
    disposicao->setContentsMargins(14, 10, 14, 10);
    disposicao->setSpacing(2);
    disposicao->addWidget(m_rotuloTitulo);
    disposicao->addWidget(m_rotuloValor);
    disposicao->addWidget(m_rotuloDetalhe);
}

void CartaoIndicador::definirValor(const QString& valor, const QString& detalhe)
{
    m_rotuloValor->setText(valor);
    m_rotuloDetalhe->setText(detalhe);
    m_rotuloDetalhe->setStyleSheet(QString());
}

void CartaoIndicador::definirDetalheComSinal(double valor, const QString& texto)
{
    m_rotuloDetalhe->setText(texto);
    if (valor > 0.0)
    {
        m_rotuloDetalhe->setStyleSheet(QStringLiteral("color: #2ecc71;"));
    }
    else if (valor < 0.0)
    {
        m_rotuloDetalhe->setStyleSheet(QStringLiteral("color: #e74c3c;"));
    }
    else
    {
        m_rotuloDetalhe->setStyleSheet(QStringLiteral("color: #95a5a6;"));
    }
}

void CartaoIndicador::limpar()
{
    m_rotuloValor->setText(QStringLiteral("--"));
    m_rotuloDetalhe->clear();
    m_rotuloDetalhe->setStyleSheet(QString());
}

}
