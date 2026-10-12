#include "ui/dialogos/dialogoalerta.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QVBoxLayout>

namespace analisador
{

DialogoAlerta::DialogoAlerta(const QVector<OpcaoAlerta>& opcoes, QWidget* pai)
    : QDialog(pai)
    , m_opcoes(opcoes)
{
    setWindowTitle(QStringLiteral("Novo alerta"));
    setMinimumWidth(440);

    m_campoAtivo = new QComboBox(this);
    for (const OpcaoAlerta& opcao : m_opcoes)
    {
        m_campoAtivo->addItem(opcao.rotulo, opcao.ativoId);
    }

    m_campoCondicao = new QComboBox(this);
    m_campoCondicao->addItem(descricaoCondicao(TipoCondicaoAlerta::PrecoAcimaDe),
                             static_cast<int>(TipoCondicaoAlerta::PrecoAcimaDe));
    m_campoCondicao->addItem(descricaoCondicao(TipoCondicaoAlerta::PrecoAbaixoDe),
                             static_cast<int>(TipoCondicaoAlerta::PrecoAbaixoDe));
    m_campoCondicao->addItem(descricaoCondicao(TipoCondicaoAlerta::VariacaoDiariaAcimaDe),
                             static_cast<int>(TipoCondicaoAlerta::VariacaoDiariaAcimaDe));

    m_campoValor = new QDoubleSpinBox(this);
    m_campoValor->setRange(0.01, 1000000.0);
    m_campoValor->setDecimals(2);

    m_rotuloAjuda = new QLabel(QString(), this);
    m_rotuloAjuda->setWordWrap(true);
    m_rotuloAjuda->setObjectName(QStringLiteral("textoAuxiliar"));

    auto* formulario = new QFormLayout();
    formulario->addRow(QStringLiteral("Ativo:"), m_campoAtivo);
    formulario->addRow(QString::fromUtf8("Condição:"), m_campoCondicao);
    formulario->addRow(QString::fromUtf8("Valor de referência:"), m_campoValor);

    auto* botoes = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    botoes->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Criar alerta"));
    botoes->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));
    botoes->button(QDialogButtonBox::Ok)->setEnabled(!m_opcoes.isEmpty());

    auto* disposicao = new QVBoxLayout(this);
    disposicao->addLayout(formulario);
    disposicao->addWidget(m_rotuloAjuda);
    disposicao->addWidget(botoes);

    connect(botoes, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(botoes, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_campoAtivo, &QComboBox::currentIndexChanged, this, &DialogoAlerta::atualizarSugestao);
    connect(m_campoCondicao, &QComboBox::currentIndexChanged, this, &DialogoAlerta::atualizarSugestao);

    atualizarSugestao();
}

void DialogoAlerta::atualizarSugestao()
{
    const QLocale brasil(QLocale::Portuguese, QLocale::Brazil);
    const bool ehVariacao = condicao() == TipoCondicaoAlerta::VariacaoDiariaAcimaDe;

    m_campoValor->setSuffix(ehVariacao ? QStringLiteral(" %") : QString());
    m_campoValor->setPrefix(ehVariacao ? QString() : QStringLiteral("R$ "));

    const int indice = m_campoAtivo->currentIndex();
    if (indice < 0 || indice >= m_opcoes.size())
    {
        m_rotuloAjuda->setText(QString::fromUtf8("Cadastre um ativo antes de criar alertas."));
        return;
    }

    const OpcaoAlerta& opcao = m_opcoes.at(indice);
    if (ehVariacao)
    {
        m_campoValor->setValue(3.0);
        m_rotuloAjuda->setText(QString::fromUtf8(
            "O alerta dispara quando a variação do dia, em módulo, passar do valor informado."));
        return;
    }

    if (opcao.ultimoFechamento > 0.0)
    {
        // Sugere um valor 5% acima ou abaixo do ultimo fechamento, conforme a condicao.
        const double fator = condicao() == TipoCondicaoAlerta::PrecoAcimaDe ? 1.05 : 0.95;
        m_campoValor->setValue(opcao.ultimoFechamento * fator);
        m_rotuloAjuda->setText(QString::fromUtf8("Último fechamento de %1: R$ %2.")
                                   .arg(opcao.rotulo.section(QLatin1Char(' '), 0, 0),
                                        brasil.toString(opcao.ultimoFechamento, 'f', 2)));
        return;
    }

    m_rotuloAjuda->setText(QString::fromUtf8(
        "Este ativo ainda não tem cotações importadas: o alerta só será avaliado depois da importação."));
}

qint64 DialogoAlerta::ativoId() const
{
    return m_campoAtivo->currentData().toLongLong();
}

TipoCondicaoAlerta DialogoAlerta::condicao() const
{
    return static_cast<TipoCondicaoAlerta>(m_campoCondicao->currentData().toInt());
}

double DialogoAlerta::valorReferencia() const
{
    return m_campoValor->value();
}

}
