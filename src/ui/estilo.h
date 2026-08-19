#pragma once

// Folha de estilo da aplicacao, em arquivo proprio para poder ser reaproveitada
// por qualquer alvo que monte telas (a aplicacao e o utilitario de captura).

#include <QString>

class QApplication;

namespace analisador
{

// Aplica o tema escuro da aplicacao (estilo Fusion + QSS).
void aplicarEstilo(QApplication& aplicacao);

// Somente a folha de estilo, para quem precisar aplica-la em outro contexto.
QString folhaDeEstilo();

}
