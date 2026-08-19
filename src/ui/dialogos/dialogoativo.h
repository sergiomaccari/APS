#pragma once

// Dialogo de cadastro e edicao de ativos. O tipo escolhido (Acao ou Fundo
// Imobiliario) determina quais campos de fundamentos ficam habilitados e qual
// subclasse concreta e construida em ativoInformado().

#include <memory>

#include <QDialog>

#include "dominio/ativo.h"

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;

namespace analisador
{

class DialogoAtivo : public QDialog
{
    Q_OBJECT

public:
    // Passar um ativo existente abre o dialogo em modo de edicao.
    explicit DialogoAtivo(QWidget* pai = nullptr, const Ativo* ativoExistente = nullptr);

    // Constroi a subclasse correspondente ao tipo escolhido, com o id preservado
    // quando em modo de edicao. Devolve nullptr se os campos estiverem vazios.
    std::unique_ptr<Ativo> ativoInformado() const;

private slots:
    void atualizarCamposPorTipo();

private:
    static const int INDICE_ACAO;
    static const int INDICE_FUNDO;

    QComboBox* m_campoTipo = nullptr;
    QLineEdit* m_campoTicker = nullptr;
    QLineEdit* m_campoNome = nullptr;
    QLineEdit* m_campoSetor = nullptr;
    QDoubleSpinBox* m_campoPrecoLucro = nullptr;
    QDoubleSpinBox* m_campoDividendYield = nullptr;
    QDoubleSpinBox* m_campoValorMercado = nullptr;
    QDoubleSpinBox* m_campoValorPatrimonial = nullptr;
    QLineEdit* m_campoSegmento = nullptr;
    QLabel* m_rotuloPrecoLucro = nullptr;
    QLabel* m_rotuloValorMercado = nullptr;
    QLabel* m_rotuloValorPatrimonial = nullptr;
    QLabel* m_rotuloSegmento = nullptr;
    qint64 m_idEmEdicao = 0;
};

}
