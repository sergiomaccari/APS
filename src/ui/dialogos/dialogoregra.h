#pragma once

// Dialogo de configuracao de uma regra de analise (Strategy).
// O nome da regra nao e editavel: e a chave que a fabrica do MotorAnalise usa
// para instanciar a estrategia concreta. Sao ajustaveis apenas os parametros e
// a ativacao.

#include <QDialog>

#include "dominio/regraconfigurada.h"

class QCheckBox;
class QDoubleSpinBox;
class QLabel;

namespace analisador
{

class DialogoRegra : public QDialog
{
    Q_OBJECT

public:
    explicit DialogoRegra(const RegraConfigurada& regra, QWidget* pai = nullptr);

    // Copia da regra recebida com os valores informados no formulario.
    RegraConfigurada regraInformada() const;

private:
    RegraConfigurada m_regra;
    QCheckBox* m_campoAtiva = nullptr;
    QDoubleSpinBox* m_campoPrincipal = nullptr;
    QDoubleSpinBox* m_campoSecundario = nullptr;
    QLabel* m_rotuloDescricao = nullptr;
};

}
