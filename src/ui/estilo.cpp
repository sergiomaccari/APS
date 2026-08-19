#include "ui/estilo.h"

#include <QApplication>

namespace analisador
{

QString folhaDeEstilo()
{
    // Paleta escura sobria; verde e vermelho ficam reservados para ganho e perda.
    return QString::fromUtf8(R"(
        QWidget { background-color: #1f2530; color: #ecf0f1; font-size: 13px; }
        QMainWindow, QDialog { background-color: #1f2530; }
        QLabel#tituloTela { font-size: 20px; font-weight: bold; color: #ffffff; }
        QLabel#tituloLogin { font-size: 26px; font-weight: bold; color: #ffffff; }
        QLabel#subtituloLogin { color: #aab7c4; }
        QLabel#tituloDialogo { font-size: 16px; font-weight: bold; }
        QLabel#textoAuxiliar { color: #9fb0c0; }
        QLabel#dicaLogin { color: #7f8c9b; font-size: 11px; }
        QLabel#erroLogin { color: #e74c3c; font-weight: bold; }
        QLabel#resumoCarteira { font-size: 14px; }

        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit, QPlainTextEdit, QTextBrowser {
            background-color: #2b3340; border: 1px solid #3d4756; border-radius: 4px;
            padding: 5px; selection-background-color: #2980b9;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {
            border: 1px solid #3498db;
        }

        QPushButton {
            background-color: #2b3340; border: 1px solid #3d4756; border-radius: 4px;
            padding: 6px 14px;
        }
        QPushButton:hover { background-color: #34405180; border-color: #3498db; }
        QPushButton:pressed { background-color: #2980b9; }
        QPushButton:default { background-color: #2980b9; border-color: #2980b9; font-weight: bold; }
        QPushButton:disabled { color: #6b7785; border-color: #2f3743; }

        QListWidget#navegacaoLateral {
            background-color: #171c25; border: none; border-right: 1px solid #2b3340;
            font-size: 14px; outline: none;
        }
        QListWidget#navegacaoLateral::item { padding: 11px 16px; }
        QListWidget#navegacaoLateral::item:selected {
            background-color: #2980b9; color: #ffffff; font-weight: bold;
        }
        QListWidget#navegacaoLateral::item:hover:!selected { background-color: #222a36; }

        QTableWidget {
            background-color: #232a36; alternate-background-color: #262e3b;
            gridline-color: #2f3743; border: 1px solid #2f3743; border-radius: 4px;
        }
        QTableWidget::item:selected { background-color: #2980b9; color: #ffffff; }
        QHeaderView::section {
            background-color: #171c25; color: #cfd8e3; padding: 7px;
            border: none; border-right: 1px solid #2b3340; font-weight: bold;
        }

        QFrame#cartaoIndicador {
            background-color: #232a36; border: 1px solid #2f3743; border-radius: 6px;
        }
        QLabel#tituloCartao { color: #9fb0c0; font-size: 11px; text-transform: uppercase; }
        QLabel#valorCartao { font-size: 21px; font-weight: bold; color: #ffffff; }
        QLabel#detalheCartao { font-size: 11px; }

        QGroupBox {
            border: 1px solid #2f3743; border-radius: 6px; margin-top: 12px; padding-top: 10px;
        }
        QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; color: #9fb0c0; }

        QMenuBar { background-color: #171c25; }
        QMenuBar::item:selected { background-color: #2980b9; }
        QMenu { background-color: #232a36; border: 1px solid #2f3743; }
        QMenu::item:selected { background-color: #2980b9; }
        QStatusBar { background-color: #171c25; color: #cfd8e3; }
        QScrollBar:vertical { background: #1f2530; width: 12px; }
        QScrollBar::handle:vertical { background: #3d4756; border-radius: 6px; min-height: 24px; }
        QScrollBar::add-line, QScrollBar::sub-line { height: 0; width: 0; }
    )");
}

void aplicarEstilo(QApplication& aplicacao)
{
    aplicacao.setStyle(QStringLiteral("Fusion"));
    aplicacao.setStyleSheet(folhaDeEstilo());
}

}
