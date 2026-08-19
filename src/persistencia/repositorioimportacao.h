#pragma once

// Persistencia do historico de importacoes de cotacoes (auditoria).

#include <optional>

#include <QString>
#include <QVector>

#include <QSqlQuery>

#include "dominio/importacao.h"

namespace analisador
{

class RepositorioImportacao
{
public:
    RepositorioImportacao();

    bool salvar(Importacao& importacao);
    bool atualizar(const Importacao& importacao);
    std::optional<Importacao> buscarPorId(qint64 id) const;
    QVector<Importacao> listarRecentes(int limite = 20) const;
    QVector<Importacao> listarPorAtivo(qint64 ativoId) const;

    QString ultimoErro() const;

private:
    static Importacao montarImportacao(const QSqlQuery& consulta);

    mutable QString m_ultimoErro;
};

}
