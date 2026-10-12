#include "ui/dialogos/dialogooperacao.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace analisador
{

DialogoOperacao::DialogoOperacao(Modo modo, const QVector<OpcaoOperacao>& opcoes, QWidget* pai)
    : QDialog(pai)
    , m_modo(modo)
    , m_opcoes(opcoes)
{
    const bool ehCompra = m_modo == Modo::Compra;
    setWindowTitle(ehCompra ? QStringLiteral("Comprar ativo") : QStringLiteral("Vender ativo"));
    setMinimumWidth(420);

    m_campoAtivo = new QComboBox(this);
    for (const OpcaoOperacao& opcao : m_opcoes)
    {
        m_campoAtivo->addItem(opcao.rotulo, opcao.ativoId);
    }

    m_campoQuantidade = new QSpinBox(this);
    m_campoQuantidade->setRange(1, 1000000);
    m_campoQuantidade->setValue(100);
    m_campoQuantidade->setGroupSeparatorShown(true);

    m_campoPreco = new QDoubleSpinBox(this);
    m_campoPreco->setRange(0.01, 1000000.0);
    m_campoPreco->setDecimals(2);
    m_campoPreco->setPrefix(QStringLiteral("R$ "));

    m_campoData = new QDateEdit(QDate::currentDate(), this);
    m_campoData->setCalendarPopup(true);
    m_campoData->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_campoData->setMaximumDate(QDate::currentDate());

    m_rotuloPreco = new QLabel(QString::fromUtf8("Preço unitário:"), this);
    m_rotuloData = new QLabel(QStringLiteral("Data da compra:"), this);
    m_rotuloLimite = new QLabel(QString(), this);
    m_rotuloLimite->setObjectName(QStringLiteral("textoAuxiliar"));

    auto* formulario = new QFormLayout();
    formulario->addRow(QStringLiteral("Ativo:"), m_campoAtivo);
    formulario->addRow(QStringLiteral("Quantidade:"), m_campoQuantidade);
    formulario->addRow(m_rotuloPreco, m_campoPreco);
    formulario->addRow(m_rotuloData, m_campoData);

    m_rotuloPreco->setVisible(ehCompra);
    m_campoPreco->setVisible(ehCompra);
    m_rotuloData->setVisible(ehCompra);
    m_campoData->setVisible(ehCompra);

    auto* botoes = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    botoes->button(QDialogButtonBox::Ok)->setText(ehCompra ? QStringLiteral("Comprar")
                                                           : QStringLiteral("Vender"));
    botoes->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));
    botoes->button(QDialogButtonBox::Ok)->setEnabled(!m_opcoes.isEmpty());

    auto* disposicao = new QVBoxLayout(this);
    disposicao->addLayout(formulario);
    disposicao->addWidget(m_rotuloLimite);
    disposicao->addWidget(botoes);

    connect(botoes, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(botoes, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_campoAtivo, &QComboBox::currentIndexChanged, this, &DialogoOperacao::ajustarLimitesPorAtivo);

    ajustarLimitesPorAtivo();
}

void DialogoOperacao::ajustarLimitesPorAtivo()
{
    const int indice = m_campoAtivo->currentIndex();
    if (indice < 0 || indice >= m_opcoes.size())
    {
        m_rotuloLimite->clear();
        return;
    }

    const OpcaoOperacao& opcao = m_opcoes.at(indice);
    if (opcao.precoSugerido > 0.0)
    {
        m_campoPreco->setValue(opcao.precoSugerido);
    }

    if (m_modo == Modo::Venda)
    {
        // Na venda a quantidade nunca pode passar da posicao existente.
        const int maximo = qMax(1, opcao.quantidadeMaxima);
        m_campoQuantidade->setRange(1, maximo);
        m_campoQuantidade->setValue(maximo);
        m_rotuloLimite->setText(QString::fromUtf8("Posição atual: %1 unidade(s).").arg(opcao.quantidadeMaxima));
        return;
    }

    m_rotuloLimite->setText(opcao.precoSugerido > 0.0
                                ? QString::fromUtf8("Preço sugerido: último fechamento importado.")
                                : QString::fromUtf8("Sem cotação importada: informe o preço pago."));
}

qint64 DialogoOperacao::ativoId() const
{
    return m_campoAtivo->currentData().toLongLong();
}

int DialogoOperacao::quantidade() const
{
    return m_campoQuantidade->value();
}

double DialogoOperacao::preco() const
{
    return m_campoPreco->value();
}

QDate DialogoOperacao::data() const
{
    return m_campoData->date();
}

}
