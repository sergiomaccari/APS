#include "ui/dialogos/dialogoregra.h"

#include <memory>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "analise/motoranalise.h"
#include "analise/regraanalise.h"

namespace analisador
{

DialogoRegra::DialogoRegra(const RegraConfigurada& regra, QWidget* pai)
    : QDialog(pai)
    , m_regra(regra)
{
    setWindowTitle(QStringLiteral("Configurar regra"));
    setMinimumWidth(480);

    auto* rotuloNome = new QLabel(m_regra.nomeRegra(), this);
    rotuloNome->setObjectName(QStringLiteral("tituloDialogo"));

    // A descricao vem da propria estrategia, ja configurada com os parametros
    // atuais - assim o texto explica exatamente o criterio em vigor.
    QString descricao = QString::fromUtf8("Regra sem descrição disponível.");
    const std::unique_ptr<RegraAnalise> estrategia = MotorAnalise::criarRegra(m_regra.nomeRegra());
    if (estrategia)
    {
        estrategia->configurar(m_regra.parametroPrincipal(), m_regra.parametroSecundario());
        descricao = estrategia->descricao();
    }

    m_rotuloDescricao = new QLabel(descricao, this);
    m_rotuloDescricao->setWordWrap(true);
    m_rotuloDescricao->setObjectName(QStringLiteral("textoAuxiliar"));

    m_campoAtiva = new QCheckBox(QString::fromUtf8("Regra ativa (participa da análise)"), this);
    m_campoAtiva->setChecked(m_regra.ativa());

    m_campoPrincipal = new QDoubleSpinBox(this);
    m_campoPrincipal->setRange(0.0, 100000.0);
    m_campoPrincipal->setDecimals(2);
    m_campoPrincipal->setValue(m_regra.parametroPrincipal());

    m_campoSecundario = new QDoubleSpinBox(this);
    m_campoSecundario->setRange(0.0, 100000.0);
    m_campoSecundario->setDecimals(2);
    m_campoSecundario->setValue(m_regra.parametroSecundario());

    auto* formulario = new QFormLayout();
    formulario->addRow(QStringLiteral("1º parâmetro:"), m_campoPrincipal);
    formulario->addRow(QStringLiteral("2º parâmetro:"), m_campoSecundario);

    auto* botoes = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    botoes->button(QDialogButtonBox::Save)->setText(QStringLiteral("Salvar"));
    botoes->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));

    auto* disposicao = new QVBoxLayout(this);
    disposicao->addWidget(rotuloNome);
    disposicao->addWidget(m_rotuloDescricao);
    disposicao->addSpacing(8);
    disposicao->addWidget(m_campoAtiva);
    disposicao->addLayout(formulario);
    disposicao->addWidget(botoes);

    connect(botoes, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(botoes, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

RegraConfigurada DialogoRegra::regraInformada() const
{
    RegraConfigurada resultado = m_regra;
    resultado.definirAtiva(m_campoAtiva->isChecked());
    resultado.definirParametroPrincipal(m_campoPrincipal->value());
    resultado.definirParametroSecundario(m_campoSecundario->value());
    return resultado;
}

}
