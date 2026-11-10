#pragma once

// Dialogo de cadastro de alerta: ativo monitorado, condicao e valor de
// referencia. As condicoes vem do enum TipoCondicaoAlerta do dominio.

#include <QDialog>
#include <QString>
#include <QVector>

#include "dominio/alerta.h"

class QComboBox;
class QDoubleSpinBox;
class QLabel;

namespace analisador
{

// Ativo oferecido no dialogo, com o ultimo fechamento como valor sugerido.
struct OpcaoAlerta
{
    qint64 ativoId = 0;
    QString rotulo;
    double ultimoFechamento = 0.0;
};

class DialogoAlerta : public QDialog
{
    Q_OBJECT

public:
    explicit DialogoAlerta(const QVector<OpcaoAlerta>& opcoes, QWidget* pai = nullptr);

    qint64 ativoId() const;
    TipoCondicaoAlerta condicao() const;
    double valorReferencia() const;

private slots:
    void atualizarSugestao();

private:
    QVector<OpcaoAlerta> m_opcoes;
    QComboBox* m_campoAtivo = nullptr;
    QComboBox* m_campoCondicao = nullptr;
    QDoubleSpinBox* m_campoValor = nullptr;
    QLabel* m_rotuloAjuda = nullptr;
};

}
