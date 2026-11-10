#include "ui/dialogos/dialogoativo.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "dominio/acao.h"
#include "dominio/fundoimobiliario.h"

namespace analisador
{

const int DialogoAtivo::INDICE_ACAO = 0;
const int DialogoAtivo::INDICE_FUNDO = 1;

DialogoAtivo::DialogoAtivo(QWidget* pai, const Ativo* ativoExistente)
    : QDialog(pai)
{
    setWindowTitle(ativoExistente == nullptr ? QStringLiteral("Cadastrar ativo")
                                             : QStringLiteral("Editar ativo"));
    setMinimumWidth(460);

    m_campoTipo = new QComboBox(this);
    m_campoTipo->addItem(QString::fromUtf8("Ação"));
    m_campoTipo->addItem(QString::fromUtf8("Fundo Imobiliário"));

    m_campoTicker = new QLineEdit(this);
    m_campoTicker->setPlaceholderText(QStringLiteral("PETR4"));
    m_campoTicker->setMaxLength(6);

    m_campoNome = new QLineEdit(this);
    m_campoNome->setPlaceholderText(QString::fromUtf8("Petróleo Brasileiro S.A."));

    m_campoSetor = new QLineEdit(this);
    m_campoSetor->setPlaceholderText(QString::fromUtf8("Petróleo, Gás e Biocombustíveis"));

    const auto criarCampoNumerico = [this](double maximo, int decimais, const QString& sufixo) {
        auto* campo = new QDoubleSpinBox(this);
        campo->setRange(0.0, maximo);
        campo->setDecimals(decimais);
        campo->setSuffix(sufixo);
        campo->setGroupSeparatorShown(true);
        return campo;
    };

    m_campoPrecoLucro = criarCampoNumerico(999.0, 2, QString());
    m_campoDividendYield = criarCampoNumerico(100.0, 2, QStringLiteral(" %"));
    m_campoValorMercado = criarCampoNumerico(1.0e13, 2, QStringLiteral(" R$"));
    m_campoValorPatrimonial = criarCampoNumerico(1.0e6, 2, QStringLiteral(" R$"));
    m_campoSegmento = new QLineEdit(this);
    m_campoSegmento->setPlaceholderText(QString::fromUtf8("Logística"));

    m_rotuloPrecoLucro = new QLabel(QString::fromUtf8("Preço/Lucro:"), this);
    m_rotuloValorMercado = new QLabel(QStringLiteral("Valor de mercado:"), this);
    m_rotuloValorPatrimonial = new QLabel(QString::fromUtf8("Valor patrimonial/cota:"), this);
    m_rotuloSegmento = new QLabel(QStringLiteral("Segmento:"), this);

    auto* formulario = new QFormLayout();
    formulario->addRow(QStringLiteral("Tipo:"), m_campoTipo);
    formulario->addRow(QStringLiteral("Ticker:"), m_campoTicker);
    formulario->addRow(QStringLiteral("Nome:"), m_campoNome);
    formulario->addRow(QStringLiteral("Setor:"), m_campoSetor);
    formulario->addRow(m_rotuloPrecoLucro, m_campoPrecoLucro);
    formulario->addRow(QStringLiteral("Dividend Yield:"), m_campoDividendYield);
    formulario->addRow(m_rotuloValorMercado, m_campoValorMercado);
    formulario->addRow(m_rotuloValorPatrimonial, m_campoValorPatrimonial);
    formulario->addRow(m_rotuloSegmento, m_campoSegmento);

    auto* botoes = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    botoes->button(QDialogButtonBox::Save)->setText(QStringLiteral("Salvar"));
    botoes->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));

    auto* disposicao = new QVBoxLayout(this);
    disposicao->addLayout(formulario);
    disposicao->addWidget(botoes);

    connect(botoes, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(botoes, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_campoTipo, &QComboBox::currentIndexChanged, this, &DialogoAtivo::atualizarCamposPorTipo);

    if (ativoExistente != nullptr)
    {
        m_idEmEdicao = ativoExistente->id();
        m_campoTicker->setText(ativoExistente->ticker());
        m_campoNome->setText(ativoExistente->nomeEmpresa());
        m_campoSetor->setText(ativoExistente->setor());

        if (const auto* acao = dynamic_cast<const Acao*>(ativoExistente))
        {
            m_campoTipo->setCurrentIndex(INDICE_ACAO);
            m_campoPrecoLucro->setValue(acao->precoLucro());
            m_campoDividendYield->setValue(acao->dividendYield());
            m_campoValorMercado->setValue(acao->valorMercado());
        }
        else if (const auto* fundo = dynamic_cast<const FundoImobiliario*>(ativoExistente))
        {
            m_campoTipo->setCurrentIndex(INDICE_FUNDO);
            m_campoDividendYield->setValue(fundo->dividendYield());
            m_campoValorPatrimonial->setValue(fundo->valorPatrimonialPorCota());
            m_campoSegmento->setText(fundo->segmento());
        }
    }

    atualizarCamposPorTipo();
}

void DialogoAtivo::atualizarCamposPorTipo()
{
    const bool ehAcao = m_campoTipo->currentIndex() == INDICE_ACAO;

    m_rotuloPrecoLucro->setVisible(ehAcao);
    m_campoPrecoLucro->setVisible(ehAcao);
    m_rotuloValorMercado->setVisible(ehAcao);
    m_campoValorMercado->setVisible(ehAcao);

    m_rotuloValorPatrimonial->setVisible(!ehAcao);
    m_campoValorPatrimonial->setVisible(!ehAcao);
    m_rotuloSegmento->setVisible(!ehAcao);
    m_campoSegmento->setVisible(!ehAcao);
}

std::unique_ptr<Ativo> DialogoAtivo::ativoInformado() const
{
    const QString ticker = m_campoTicker->text().trimmed().toUpper();
    const QString nome = m_campoNome->text().trimmed();
    const QString setor = m_campoSetor->text().trimmed();
    if (ticker.isEmpty() || nome.isEmpty())
    {
        return nullptr;
    }

    if (m_campoTipo->currentIndex() == INDICE_ACAO)
    {
        auto acao = std::make_unique<Acao>(m_idEmEdicao,
                                           ticker,
                                           nome,
                                           setor,
                                           m_campoPrecoLucro->value(),
                                           m_campoDividendYield->value(),
                                           m_campoValorMercado->value());
        return acao;
    }

    auto fundo = std::make_unique<FundoImobiliario>(m_idEmEdicao,
                                                    ticker,
                                                    nome,
                                                    setor,
                                                    m_campoDividendYield->value(),
                                                    m_campoValorPatrimonial->value(),
                                                    m_campoSegmento->text().trimmed());
    return fundo;
}

}
