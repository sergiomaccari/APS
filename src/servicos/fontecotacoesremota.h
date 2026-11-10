#pragma once

// Interface de uma fonte remota de cotacoes diarias (RF020, ADR 0003).
//
// Existe para isolar o ServicoSincronizacao do provedor concreto: hoje a
// implementacao e a FonteYahooFinance, mas trocar por outra API (brapi, B3)
// nao deve exigir mudanca no servico nem na interface grafica. Nos testes a
// mesma interface permite injetar uma fonte falsa, sem rede.
//
// Contrato: buscar() NAO grava nada no banco - apenas converte a resposta da
// API em objetos de dominio. Falha de rede ou de formato devolve false com a
// causa em *erro, nunca lanca excecao.

#include <QDate>
#include <QString>
#include <QVector>

#include "dominio/cotacao.h"

namespace analisador
{

class FonteCotacoesRemota
{
public:
    virtual ~FonteCotacoesRemota() = default;

    // Nome legivel do provedor, usado no registro de auditoria e nas mensagens.
    virtual QString nome() const = 0;

    // Baixa as cotacoes diarias de "ticker" no intervalo [de, ate], ja com o
    // ativoId preenchido em cada candle. Devolve false com *erro preenchido em
    // falha de rede ou de formato.
    virtual bool buscar(const QString& ticker,
                        qint64 ativoId,
                        const QDate& de,
                        const QDate& ate,
                        QVector<Cotacao>* destino,
                        QString* erro) = 0;
};

}
