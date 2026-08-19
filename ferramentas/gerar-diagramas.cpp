// Gerador de diagramas UML do Analisador B3.
//
// Le uma especificacao JSON e desenha o diagrama em PNG com QPainter, no estilo
// classico das ferramentas UML (caixas amarelas, moldura com etiqueta "sd/stm/act").
// Motivacao: os documentos da disciplina precisam de ~120 diagramas consistentes
// entre si e regeneraveis; nenhuma ferramenta externa de diagramacao esta
// disponivel nesta maquina, mas o Qt ja esta.
//
// Uso:
//   gerar-diagramas <entrada.json> <saida.png>
//   gerar-diagramas --lote <pasta-com-jsons> <pasta-de-saida>
//
// Tipos de diagrama:
//   "sequencia" - layout automatico (participantes + mensagens em ordem)
//   "grade"     - elementos posicionados em grade (col/linha), com ligacoes;
//                 cobre classes, objetos, casos de uso, estados, atividades,
//                 comunicacao e entidade-relacionamento.
// O formato completo esta documentado em docs/entrega/diagramas/FORMATO.md.

#include <algorithm>
#include <cmath>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFontMetricsF>
#include <QGuiApplication>
#include <QHash>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QPainter>
#include <QPainterPath>
#include <QTextStream>
#include <QVector>

namespace
{

QTextStream& saida()
{
    static QTextStream fluxo(stdout);
    return fluxo;
}

// ----- paleta (estilo classico de ferramenta UML, fundo claro para impressao) --
const QColor COR_FUNDO(0xFF, 0xFF, 0xFF);
const QColor COR_CAIXA(0xFE, 0xFE, 0xCE);      // amarelo classico
const QColor COR_BORDA(0x8A, 0x60, 0x12);
const QColor COR_TEXTO(0x20, 0x20, 0x20);
const QColor COR_LINHA(0x40, 0x40, 0x40);
const QColor COR_ATIVACAO(0xF8, 0xF0, 0xB0);
const QColor COR_NOTA(0xFF, 0xF6, 0xC8);
const QColor COR_TABELA_TITULO(0xD9, 0xE2, 0xF3);
const QColor COR_MOLDURA(0x60, 0x60, 0x60);

const double ESCALA = 2.0;   // renderiza em 2x para ficar nitido no documento

QFont fonteNormal()
{
    QFont fonte(QStringLiteral("DejaVu Sans"));
    fonte.setPointSizeF(10.5);
    return fonte;
}

QFont fonteNegrito()
{
    QFont fonte = fonteNormal();
    fonte.setBold(true);
    return fonte;
}

QFont fonteItalico()
{
    QFont fonte = fonteNormal();
    fonte.setItalic(true);
    return fonte;
}

QFont fontePequena()
{
    QFont fonte = fonteNormal();
    fonte.setPointSizeF(9.0);
    return fonte;
}

QStringList linhasDe(const QString& texto)
{
    return texto.split(QLatin1Char('\n'));
}

QSizeF medirTexto(const QStringList& linhas, const QFont& fonte)
{
    const QFontMetricsF metrica(fonte);
    double largura = 0.0;
    for (const QString& linha : linhas)
    {
        largura = std::max(largura, metrica.horizontalAdvance(linha));
    }
    return QSizeF(largura, metrica.height() * linhas.size());
}

void desenharTexto(QPainter& p, const QPointF& centro, const QStringList& linhas,
                   const QFont& fonte, Qt::Alignment alinhamento = Qt::AlignHCenter)
{
    const QFontMetricsF metrica(fonte);
    p.setFont(fonte);
    p.setPen(COR_TEXTO);
    const double alturaTotal = metrica.height() * linhas.size();
    double y = centro.y() - alturaTotal / 2.0 + metrica.ascent();
    for (const QString& linha : linhas)
    {
        double x = centro.x();
        const double larguraLinha = metrica.horizontalAdvance(linha);
        if (alinhamento == Qt::AlignHCenter)
        {
            x -= larguraLinha / 2.0;
        }
        p.drawText(QPointF(x, y), linha);
        y += metrica.height();
    }
}

// Ponto na borda de um retangulo na direcao de "para" (a partir do centro).
QPointF bordaRetangulo(const QRectF& r, const QPointF& para)
{
    const QPointF c = r.center();
    const double dx = para.x() - c.x();
    const double dy = para.y() - c.y();
    if (qFuzzyIsNull(dx) && qFuzzyIsNull(dy))
    {
        return c;
    }
    const double escalaX = qFuzzyIsNull(dx) ? 1e18 : (r.width() / 2.0) / std::abs(dx);
    const double escalaY = qFuzzyIsNull(dy) ? 1e18 : (r.height() / 2.0) / std::abs(dy);
    const double t = std::min(escalaX, escalaY);
    return QPointF(c.x() + dx * t, c.y() + dy * t);
}

// Ponto na borda de uma elipse inscrita no retangulo, na direcao de "para".
QPointF bordaElipse(const QRectF& r, const QPointF& para)
{
    const QPointF c = r.center();
    const double dx = para.x() - c.x();
    const double dy = para.y() - c.y();
    const double a = r.width() / 2.0;
    const double b = r.height() / 2.0;
    const double norma = std::sqrt((dx * dx) / (a * a) + (dy * dy) / (b * b));
    if (qFuzzyIsNull(norma))
    {
        return c;
    }
    return QPointF(c.x() + dx / norma, c.y() + dy / norma);
}

void desenharSetaAberta(QPainter& p, const QPointF& ponta, double angulo)
{
    const double comprimento = 11.0;
    const double abertura = 0.46;
    const QPointF a(ponta.x() - comprimento * std::cos(angulo - abertura),
                    ponta.y() - comprimento * std::sin(angulo - abertura));
    const QPointF b(ponta.x() - comprimento * std::cos(angulo + abertura),
                    ponta.y() - comprimento * std::sin(angulo + abertura));
    p.drawLine(ponta, a);
    p.drawLine(ponta, b);
}

void desenharSetaCheia(QPainter& p, const QPointF& ponta, double angulo)
{
    const double comprimento = 12.0;
    const double abertura = 0.42;
    QPainterPath caminho;
    caminho.moveTo(ponta);
    caminho.lineTo(ponta.x() - comprimento * std::cos(angulo - abertura),
                   ponta.y() - comprimento * std::sin(angulo - abertura));
    caminho.lineTo(ponta.x() - comprimento * std::cos(angulo + abertura),
                   ponta.y() - comprimento * std::sin(angulo + abertura));
    caminho.closeSubpath();
    p.fillPath(caminho, COR_LINHA);
}

void desenharTriangulo(QPainter& p, const QPointF& ponta, double angulo)
{
    // Ponta de heranca/realizacao: triangulo vazado.
    const double comprimento = 16.0;
    const double abertura = 0.42;
    QPainterPath caminho;
    caminho.moveTo(ponta);
    caminho.lineTo(ponta.x() - comprimento * std::cos(angulo - abertura),
                   ponta.y() - comprimento * std::sin(angulo - abertura));
    caminho.lineTo(ponta.x() - comprimento * std::cos(angulo + abertura),
                   ponta.y() - comprimento * std::sin(angulo + abertura));
    caminho.closeSubpath();
    p.save();
    p.setBrush(COR_FUNDO);
    p.setPen(QPen(COR_LINHA, 1.4));
    p.drawPath(caminho);
    p.restore();
}

void desenharLosango(QPainter& p, const QPointF& ponta, double angulo, bool cheio)
{
    const double comprimento = 9.0;
    const QPointF meio(ponta.x() - comprimento * std::cos(angulo),
                       ponta.y() - comprimento * std::sin(angulo));
    const QPointF fim(ponta.x() - 2 * comprimento * std::cos(angulo),
                      ponta.y() - 2 * comprimento * std::sin(angulo));
    const double perp = angulo + M_PI_2;
    const double meiaLargura = 5.0;
    QPainterPath caminho;
    caminho.moveTo(ponta);
    caminho.lineTo(meio.x() + meiaLargura * std::cos(perp), meio.y() + meiaLargura * std::sin(perp));
    caminho.lineTo(fim);
    caminho.lineTo(meio.x() - meiaLargura * std::cos(perp), meio.y() - meiaLargura * std::sin(perp));
    caminho.closeSubpath();
    p.save();
    p.setBrush(cheio ? QBrush(COR_LINHA) : QBrush(COR_FUNDO));
    p.setPen(QPen(COR_LINHA, 1.4));
    p.drawPath(caminho);
    p.restore();
}

// Rotulo com fundo claro sobre a linha, para continuar legivel cruzando tracos.
void desenharRotuloDeLigacao(QPainter& p, const QPointF& centro, const QString& rotulo)
{
    if (rotulo.isEmpty())
    {
        return;
    }
    const QStringList linhas = linhasDe(rotulo);
    const QSizeF tamanho = medirTexto(linhas, fontePequena());
    const QRectF fundo(centro.x() - tamanho.width() / 2.0 - 3.0,
                       centro.y() - tamanho.height() / 2.0 - 1.0,
                       tamanho.width() + 6.0,
                       tamanho.height() + 2.0);
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 220));
    p.drawRect(fundo);
    p.restore();
    desenharTexto(p, centro, linhas, fontePequena());
}

void desenharAtor(QPainter& p, const QRectF& area, const QString& nome)
{
    const QFontMetricsF metrica(fonteNormal());
    const double alturaNome = metrica.height();
    const QPointF topo(area.center().x(), area.top() + 4.0);
    const double raioCabeca = 8.0;

    p.setPen(QPen(COR_LINHA, 1.6));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPointF(topo.x(), topo.y() + raioCabeca), raioCabeca, raioCabeca);
    const double pescoco = topo.y() + 2 * raioCabeca;
    const double quadril = area.bottom() - alturaNome - 14.0;
    p.drawLine(QPointF(topo.x(), pescoco), QPointF(topo.x(), quadril));                    // tronco
    p.drawLine(QPointF(topo.x() - 14.0, pescoco + 7.0), QPointF(topo.x() + 14.0, pescoco + 7.0)); // bracos
    p.drawLine(QPointF(topo.x(), quadril), QPointF(topo.x() - 12.0, quadril + 14.0));      // pernas
    p.drawLine(QPointF(topo.x(), quadril), QPointF(topo.x() + 12.0, quadril + 14.0));

    desenharTexto(p, QPointF(area.center().x(), area.bottom() - alturaNome / 2.0),
                  linhasDe(nome), fonteNormal());
}

// Moldura padrao UML: retangulo externo + etiqueta pentagonal no canto ("sd UC001").
QRectF desenharMoldura(QPainter& p, const QSizeF& conteudo, const QString& etiqueta)
{
    const QRectF moldura(6.0, 6.0, conteudo.width() - 12.0, conteudo.height() - 12.0);
    p.setPen(QPen(COR_MOLDURA, 1.4));
    p.setBrush(Qt::NoBrush);
    p.drawRect(moldura);

    if (!etiqueta.isEmpty())
    {
        const QSizeF tamanho = medirTexto(linhasDe(etiqueta), fonteNegrito());
        const double largura = tamanho.width() + 22.0;
        const double altura = tamanho.height() + 8.0;
        QPainterPath caminho;
        caminho.moveTo(moldura.topLeft());
        caminho.lineTo(moldura.left() + largura, moldura.top());
        caminho.lineTo(moldura.left() + largura, moldura.top() + altura - 8.0);
        caminho.lineTo(moldura.left() + largura - 10.0, moldura.top() + altura);
        caminho.lineTo(moldura.left(), moldura.top() + altura);
        caminho.closeSubpath();
        p.setBrush(QColor(0xEF, 0xEF, 0xEF));
        p.drawPath(caminho);
        desenharTexto(p, QPointF(moldura.left() + largura / 2.0 - 4.0, moldura.top() + altura / 2.0),
                      linhasDe(etiqueta), fonteNegrito());
    }
    return moldura;
}

bool salvarImagem(QImage& imagem, const QString& caminho)
{
    QDir().mkpath(QFileInfo(caminho).absolutePath());
    if (!imagem.save(caminho, "PNG"))
    {
        saida() << "FALHA ao salvar " << caminho << Qt::endl;
        return false;
    }
    return true;
}

// ============================ diagrama de sequencia ============================

bool gerarSequencia(const QJsonObject& raiz, const QString& caminhoSaida)
{
    struct Participante
    {
        QString id;
        QString nome;
        bool ator = false;
        double centroX = 0.0;
        double primeiraMensagem = 1e18;
        double ultimaMensagem = -1.0;
    };

    QVector<Participante> participantes;
    QHash<QString, int> indicePorId;
    const QJsonArray listaParticipantes = raiz.value(QStringLiteral("participantes")).toArray();
    for (const QJsonValue& valor : listaParticipantes)
    {
        const QJsonObject objeto = valor.toObject();
        Participante participante;
        participante.id = objeto.value(QStringLiteral("id")).toString();
        participante.nome = objeto.value(QStringLiteral("nome")).toString();
        participante.ator = objeto.value(QStringLiteral("ator")).toBool(false);
        indicePorId.insert(participante.id, participantes.size());
        participantes.append(participante);
    }
    if (participantes.isEmpty())
    {
        saida() << "sequencia sem participantes" << Qt::endl;
        return false;
    }

    const QJsonArray mensagens = raiz.value(QStringLiteral("mensagens")).toArray();
    const QFontMetricsF metrica(fonteNormal());
    const QFontMetricsF metricaPequena(fontePequena());

    // ---- posicoes horizontais: largura da caixa + folga para os rotulos ----
    const double alturaCaixa = metrica.height() + 16.0;
    QVector<double> larguraCaixa(participantes.size());
    for (int i = 0; i < participantes.size(); ++i)
    {
        const QSizeF tamanho = medirTexto(linhasDe(participantes[i].nome), fonteNormal());
        larguraCaixa[i] = std::max(participantes[i].ator ? 56.0 : 90.0, tamanho.width() + 22.0);
    }

    // folga minima entre linhas de vida vizinhas, considerando o rotulo mais largo
    QVector<double> folgaAposColuna(participantes.size(), 60.0);
    for (const QJsonValue& valor : mensagens)
    {
        const QJsonObject mensagem = valor.toObject();
        const int de = indicePorId.value(mensagem.value(QStringLiteral("de")).toString(), -1);
        const int para = indicePorId.value(mensagem.value(QStringLiteral("para")).toString(), -1);
        if (de < 0 || para < 0)
        {
            saida() << "mensagem com participante desconhecido em " << caminhoSaida << Qt::endl;
            return false;
        }
        const QString rotulo = mensagem.value(QStringLiteral("rotulo")).toString();
        const double larguraRotulo = metricaPequena.horizontalAdvance(rotulo) + 26.0;
        if (de == para)
        {
            folgaAposColuna[de] = std::max(folgaAposColuna[de], larguraRotulo + 30.0);
            continue;
        }
        const int esquerda = std::min(de, para);
        const int direita = std::max(de, para);
        // distribui a exigencia do rotulo pelas colunas que a mensagem cruza
        const double porVao = larguraRotulo / (direita - esquerda);
        for (int i = esquerda; i < direita; ++i)
        {
            folgaAposColuna[i] = std::max(folgaAposColuna[i], porVao);
        }
    }

    double x = 30.0;
    for (int i = 0; i < participantes.size(); ++i)
    {
        participantes[i].centroX = x + larguraCaixa[i] / 2.0;
        x += larguraCaixa[i] + (i + 1 < participantes.size() ? folgaAposColuna[i] : 0.0);
    }
    const double larguraConteudo = x + 30.0;

    // ---- posicoes verticais ----
    const double alturaAtor = 64.0 + metrica.height();
    const double topoCaixas = 34.0;
    const double alturaTopo = topoCaixas + std::max(alturaCaixa, alturaAtor);
    const double passo = 40.0;
    const double passoAuto = 54.0;

    double yAtual = alturaTopo + 26.0;
    QVector<double> alturaDaMensagem(mensagens.size());
    for (int i = 0; i < mensagens.size(); ++i)
    {
        const QJsonObject mensagem = mensagens.at(i).toObject();
        const bool ehAuto = mensagem.value(QStringLiteral("de")).toString()
                            == mensagem.value(QStringLiteral("para")).toString();
        alturaDaMensagem[i] = yAtual;
        yAtual += ehAuto ? passoAuto : passo;
    }
    const double alturaConteudo = yAtual + 28.0;

    // faixa de ativacao de cada participante
    for (int i = 0; i < mensagens.size(); ++i)
    {
        const QJsonObject mensagem = mensagens.at(i).toObject();
        for (const QString& chave : {QStringLiteral("de"), QStringLiteral("para")})
        {
            Participante& participante = participantes[indicePorId.value(mensagem.value(chave).toString())];
            participante.primeiraMensagem = std::min(participante.primeiraMensagem, alturaDaMensagem[i] - 10.0);
            participante.ultimaMensagem = std::max(participante.ultimaMensagem, alturaDaMensagem[i] + 14.0);
        }
    }

    // ---- desenho ----
    QImage imagem(static_cast<int>(larguraConteudo * ESCALA),
                  static_cast<int>(alturaConteudo * ESCALA),
                  QImage::Format_RGB32);
    imagem.fill(COR_FUNDO);
    QPainter p(&imagem);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.scale(ESCALA, ESCALA);

    desenharMoldura(p, QSizeF(larguraConteudo, alturaConteudo),
                    raiz.value(QStringLiteral("moldura")).toString());

    // linhas de vida
    for (const Participante& participante : participantes)
    {
        p.setPen(QPen(COR_LINHA, 1.0, Qt::DashLine));
        p.drawLine(QPointF(participante.centroX, alturaTopo),
                   QPointF(participante.centroX, alturaConteudo - 22.0));
    }

    // barras de ativacao
    for (const Participante& participante : participantes)
    {
        if (participante.ultimaMensagem < 0.0)
        {
            continue;
        }
        const QRectF barra(participante.centroX - 5.0, participante.primeiraMensagem,
                           10.0, participante.ultimaMensagem - participante.primeiraMensagem);
        p.setPen(QPen(COR_BORDA, 1.0));
        p.setBrush(COR_ATIVACAO);
        p.drawRect(barra);
    }

    // caixas/atores do topo (depois das linhas, para cobrir o inicio do traco)
    for (int i = 0; i < participantes.size(); ++i)
    {
        const Participante& participante = participantes[i];
        if (participante.ator)
        {
            const QRectF area(participante.centroX - larguraCaixa[i] / 2.0, topoCaixas - 14.0,
                              larguraCaixa[i], alturaAtor);
            p.setBrush(COR_FUNDO);
            desenharAtor(p, area, participante.nome);
        }
        else
        {
            const QRectF caixa(participante.centroX - larguraCaixa[i] / 2.0, topoCaixas,
                               larguraCaixa[i], alturaCaixa);
            p.setPen(QPen(COR_BORDA, 1.4));
            p.setBrush(COR_CAIXA);
            p.drawRect(caixa);
            desenharTexto(p, caixa.center(), linhasDe(participante.nome), fonteNormal());
        }
    }

    // mensagens
    for (int i = 0; i < mensagens.size(); ++i)
    {
        const QJsonObject mensagem = mensagens.at(i).toObject();
        const int de = indicePorId.value(mensagem.value(QStringLiteral("de")).toString());
        const int para = indicePorId.value(mensagem.value(QStringLiteral("para")).toString());
        const bool retorno = mensagem.value(QStringLiteral("retorno")).toBool(false);
        const QString rotulo = mensagem.value(QStringLiteral("rotulo")).toString();
        const double y = alturaDaMensagem[i];

        p.setPen(QPen(COR_LINHA, 1.3, retorno ? Qt::DashLine : Qt::SolidLine));

        if (de == para)
        {
            // auto-chamada: pequeno lacete a direita da linha de vida
            const double x0 = participantes[de].centroX + 5.0;
            const double xLargura = 34.0;
            p.drawLine(QPointF(x0, y), QPointF(x0 + xLargura, y));
            p.drawLine(QPointF(x0 + xLargura, y), QPointF(x0 + xLargura, y + 18.0));
            p.drawLine(QPointF(x0 + xLargura, y + 18.0), QPointF(x0, y + 18.0));
            desenharSetaCheia(p, QPointF(x0, y + 18.0), M_PI);
            p.setFont(fontePequena());
            p.setPen(COR_TEXTO);
            p.drawText(QPointF(x0 + xLargura + 8.0, y + 8.0), rotulo);
            continue;
        }

        const double borda = 5.0;
        const double x1 = participantes[de].centroX + (de < para ? borda : -borda);
        const double x2 = participantes[para].centroX + (de < para ? -borda : borda);
        p.drawLine(QPointF(x1, y), QPointF(x2, y));
        const double angulo = de < para ? 0.0 : M_PI;
        if (retorno)
        {
            p.setPen(QPen(COR_LINHA, 1.3));
            desenharSetaAberta(p, QPointF(x2, y), angulo);
        }
        else
        {
            desenharSetaCheia(p, QPointF(x2, y), angulo);
        }
        desenharRotuloDeLigacao(p, QPointF((x1 + x2) / 2.0, y - metricaPequena.height() / 2.0 - 2.0), rotulo);
    }

    p.end();
    return salvarImagem(imagem, caminhoSaida);
}

// ============================ diagramas em grade ===============================

struct Elemento
{
    QString id;
    QString tipo;
    QString nome;
    QString estereotipo;
    QStringList texto;
    QStringList atributos;
    QStringList metodos;
    QStringList colunasTabela;
    double col = 0.0;
    double linha = 0.0;
    double larguraMinima = 0.0;
    QSizeF tamanho;
    QRectF area;
};

// Tamanho natural de cada tipo de elemento, medido pelo texto que carrega.
QSizeF medirElemento(const Elemento& e)
{
    const QFontMetricsF metrica(fonteNormal());
    const double alturaLinha = metrica.height();

    if (e.tipo == QLatin1String("inicial"))
    {
        return QSizeF(20.0, 20.0);
    }
    if (e.tipo == QLatin1String("final"))
    {
        return QSizeF(24.0, 24.0);
    }
    if (e.tipo == QLatin1String("decisao"))
    {
        const QSizeF t = e.texto.isEmpty() ? QSizeF(0, 0) : medirTexto(e.texto, fontePequena());
        return QSizeF(std::max(44.0, t.width() + 34.0), std::max(34.0, t.height() + 24.0));
    }
    if (e.tipo == QLatin1String("barra"))
    {
        return QSizeF(std::max(120.0, e.larguraMinima), 7.0);
    }
    if (e.tipo == QLatin1String("ator"))
    {
        const QSizeF nome = medirTexto(linhasDe(e.nome), fonteNormal());
        return QSizeF(std::max(64.0, nome.width() + 8.0), 66.0 + nome.height());
    }
    if (e.tipo == QLatin1String("casodeuso"))
    {
        const QSizeF t = medirTexto(e.texto, fonteNormal());
        return QSizeF(t.width() * 1.42 + 28.0, t.height() * 1.7 + 18.0);
    }
    if (e.tipo == QLatin1String("nota") || e.tipo == QLatin1String("texto"))
    {
        const QSizeF t = medirTexto(e.texto, fontePequena());
        return QSizeF(t.width() + 20.0, t.height() + 16.0);
    }
    if (e.tipo == QLatin1String("classe") || e.tipo == QLatin1String("objeto"))
    {
        QStringList cabecalho = linhasDe(e.nome);
        if (!e.estereotipo.isEmpty())
        {
            cabecalho.prepend(e.estereotipo);
        }
        double largura = medirTexto(cabecalho, fonteNegrito()).width();
        double altura = medirTexto(cabecalho, fonteNegrito()).height() + 12.0;
        if (!e.atributos.isEmpty() || e.tipo == QLatin1String("classe"))
        {
            const QSizeF t = e.atributos.isEmpty() ? QSizeF(0, 6) : medirTexto(e.atributos, fontePequena());
            largura = std::max(largura, t.width());
            altura += t.height() + 10.0;
        }
        if (!e.metodos.isEmpty())
        {
            const QSizeF t = medirTexto(e.metodos, fontePequena());
            largura = std::max(largura, t.width());
            altura += t.height() + 10.0;
        }
        return QSizeF(std::max(largura + 20.0, e.larguraMinima), altura);
    }
    if (e.tipo == QLatin1String("tabela"))
    {
        double largura = medirTexto(linhasDe(e.nome), fonteNegrito()).width();
        const QSizeF corpo = e.colunasTabela.isEmpty() ? QSizeF(0, 0)
                                                       : medirTexto(e.colunasTabela, fontePequena());
        largura = std::max(largura, corpo.width());
        const double altura = alturaLinha + 10.0 + corpo.height() + 10.0;
        return QSizeF(std::max(largura + 22.0, e.larguraMinima), altura);
    }
    // estado, acao e o que mais vier: retangulo (arredondado) com texto
    QStringList conteudo = e.texto;
    if (!e.nome.isEmpty())
    {
        conteudo.prepend(e.nome);
    }
    const QSizeF t = medirTexto(conteudo, fonteNormal());
    return QSizeF(std::max(t.width() + 26.0, std::max(70.0, e.larguraMinima)), t.height() + 18.0);
}

void desenharElemento(QPainter& p, const Elemento& e)
{
    const QRectF r = e.area;
    p.setPen(QPen(COR_BORDA, 1.4));
    p.setBrush(COR_CAIXA);

    if (e.tipo == QLatin1String("inicial"))
    {
        p.setBrush(COR_LINHA);
        p.setPen(Qt::NoPen);
        p.drawEllipse(r.center(), 8.0, 8.0);
        return;
    }
    if (e.tipo == QLatin1String("final"))
    {
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(COR_LINHA, 1.6));
        p.drawEllipse(r.center(), 10.0, 10.0);
        p.setBrush(COR_LINHA);
        p.setPen(Qt::NoPen);
        p.drawEllipse(r.center(), 5.5, 5.5);
        return;
    }
    if (e.tipo == QLatin1String("decisao"))
    {
        QPainterPath losango;
        losango.moveTo(r.center().x(), r.top());
        losango.lineTo(r.right(), r.center().y());
        losango.lineTo(r.center().x(), r.bottom());
        losango.lineTo(r.left(), r.center().y());
        losango.closeSubpath();
        p.drawPath(losango);
        if (!e.texto.isEmpty())
        {
            desenharTexto(p, r.center(), e.texto, fontePequena());
        }
        return;
    }
    if (e.tipo == QLatin1String("barra"))
    {
        p.setBrush(COR_LINHA);
        p.setPen(Qt::NoPen);
        p.drawRect(r);
        return;
    }
    if (e.tipo == QLatin1String("ator"))
    {
        p.setBrush(Qt::NoBrush);
        desenharAtor(p, r, e.nome);
        return;
    }
    if (e.tipo == QLatin1String("casodeuso"))
    {
        p.drawEllipse(r);
        desenharTexto(p, r.center(), e.texto, fonteNormal());
        return;
    }
    if (e.tipo == QLatin1String("nota"))
    {
        const double dobra = 11.0;
        QPainterPath caminho;
        caminho.moveTo(r.topLeft());
        caminho.lineTo(r.right() - dobra, r.top());
        caminho.lineTo(r.right(), r.top() + dobra);
        caminho.lineTo(r.bottomRight());
        caminho.lineTo(r.bottomLeft());
        caminho.closeSubpath();
        p.setBrush(COR_NOTA);
        p.drawPath(caminho);
        p.drawLine(QPointF(r.right() - dobra, r.top()), QPointF(r.right() - dobra, r.top() + dobra));
        p.drawLine(QPointF(r.right() - dobra, r.top() + dobra), QPointF(r.right(), r.top() + dobra));
        desenharTexto(p, r.center(), e.texto, fontePequena());
        return;
    }
    if (e.tipo == QLatin1String("texto"))
    {
        desenharTexto(p, r.center(), e.texto, fontePequena());
        return;
    }
    if (e.tipo == QLatin1String("classe") || e.tipo == QLatin1String("objeto"))
    {
        p.drawRect(r);
        const QFontMetricsF negrito(fonteNegrito());
        const QFontMetricsF pequena(fontePequena());

        QStringList cabecalho = linhasDe(e.nome);
        if (!e.estereotipo.isEmpty())
        {
            cabecalho.prepend(e.estereotipo);
        }
        double y = r.top() + 6.0;
        const double alturaCabecalho = negrito.height() * cabecalho.size();
        const QFont fonteNome = e.estereotipo == QLatin1String("<<abstract>>")
                                    ? fonteItalico()
                                    : fonteNegrito();
        desenharTexto(p, QPointF(r.center().x(), y + alturaCabecalho / 2.0), cabecalho,
                      e.tipo == QLatin1String("classe") ? fonteNome : fonteNegrito());
        if (e.tipo == QLatin1String("objeto"))
        {
            // nome de objeto e sublinhado por convencao UML
            const double larguraNome = negrito.horizontalAdvance(e.nome);
            p.setPen(QPen(COR_TEXTO, 1.0));
            p.drawLine(QPointF(r.center().x() - larguraNome / 2.0, y + alturaCabecalho + 1.0),
                       QPointF(r.center().x() + larguraNome / 2.0, y + alturaCabecalho + 1.0));
        }
        y += alturaCabecalho + 6.0;

        const auto compartimento = [&](const QStringList& itens) {
            p.setPen(QPen(COR_BORDA, 1.2));
            p.drawLine(QPointF(r.left(), y), QPointF(r.right(), y));
            y += 5.0;
            p.setFont(fontePequena());
            p.setPen(COR_TEXTO);
            for (const QString& item : itens)
            {
                p.drawText(QPointF(r.left() + 8.0, y + pequena.ascent()), item);
                y += pequena.height();
            }
            y += 5.0;
        };
        if (!e.atributos.isEmpty() || e.tipo == QLatin1String("classe"))
        {
            compartimento(e.atributos);
        }
        if (!e.metodos.isEmpty())
        {
            compartimento(e.metodos);
        }
        return;
    }
    if (e.tipo == QLatin1String("tabela"))
    {
        const QFontMetricsF negrito(fonteNegrito());
        const QFontMetricsF pequena(fontePequena());
        const double alturaTitulo = negrito.height() + 10.0;
        p.setBrush(COR_TABELA_TITULO);
        p.drawRect(QRectF(r.left(), r.top(), r.width(), alturaTitulo));
        desenharTexto(p, QPointF(r.center().x(), r.top() + alturaTitulo / 2.0),
                      linhasDe(e.nome), fonteNegrito());
        p.setBrush(COR_FUNDO);
        p.drawRect(QRectF(r.left(), r.top() + alturaTitulo, r.width(), r.height() - alturaTitulo));
        p.setFont(fontePequena());
        p.setPen(COR_TEXTO);
        double y = r.top() + alturaTitulo + 5.0;
        for (const QString& coluna : e.colunasTabela)
        {
            p.drawText(QPointF(r.left() + 10.0, y + pequena.ascent()), coluna);
            y += pequena.height();
        }
        return;
    }

    // estado / acao (retangulo arredondado)
    const double raio = e.tipo == QLatin1String("acao") ? 10.0 : 13.0;
    p.drawRoundedRect(r, raio, raio);
    QStringList conteudo = e.texto;
    if (!e.nome.isEmpty())
    {
        // estado com nome + acoes internas: nome em negrito e divisor
        const QFontMetricsF negrito(fonteNegrito());
        desenharTexto(p, QPointF(r.center().x(), r.top() + 6.0 + negrito.height() / 2.0),
                      linhasDe(e.nome), fonteNegrito());
        if (!e.texto.isEmpty())
        {
            const double yDivisor = r.top() + negrito.height() + 9.0;
            p.setPen(QPen(COR_BORDA, 1.0));
            p.drawLine(QPointF(r.left() + 4.0, yDivisor), QPointF(r.right() - 4.0, yDivisor));
            const QSizeF t = medirTexto(e.texto, fonteNormal());
            desenharTexto(p, QPointF(r.center().x(), yDivisor + 4.0 + t.height() / 2.0),
                          e.texto, fonteNormal());
        }
        return;
    }
    desenharTexto(p, r.center(), conteudo, fonteNormal());
}

QPointF bordaDoElemento(const Elemento& e, const QPointF& para)
{
    if (e.tipo == QLatin1String("casodeuso") || e.tipo == QLatin1String("inicial")
        || e.tipo == QLatin1String("final"))
    {
        return bordaElipse(e.area, para);
    }
    return bordaRetangulo(e.area, para);
}

bool gerarGrade(const QJsonObject& raiz, const QString& caminhoSaida)
{
    QVector<Elemento> elementos;
    QHash<QString, int> indicePorId;

    const QJsonArray listaElementos = raiz.value(QStringLiteral("elementos")).toArray();
    for (const QJsonValue& valor : listaElementos)
    {
        const QJsonObject objeto = valor.toObject();
        Elemento e;
        e.id = objeto.value(QStringLiteral("id")).toString();
        e.tipo = objeto.value(QStringLiteral("tipo")).toString(QStringLiteral("estado"));
        e.nome = objeto.value(QStringLiteral("nome")).toString();
        e.estereotipo = objeto.value(QStringLiteral("estereotipo")).toString();
        e.texto = linhasDe(objeto.value(QStringLiteral("texto")).toString());
        if (e.texto.size() == 1 && e.texto.first().isEmpty())
        {
            e.texto.clear();
        }
        const auto listaDe = [&objeto](const char* chave) {
            QStringList linhas;
            for (const QJsonValue& item : objeto.value(QLatin1String(chave)).toArray())
            {
                linhas.append(item.toString());
            }
            return linhas;
        };
        e.atributos = listaDe("atributos");
        e.metodos = listaDe("metodos");
        e.colunasTabela = listaDe("colunas");
        e.col = objeto.value(QStringLiteral("col")).toDouble();
        e.linha = objeto.value(QStringLiteral("linha")).toDouble();
        e.larguraMinima = objeto.value(QStringLiteral("largura")).toDouble(0.0);
        e.tamanho = medirElemento(e);
        indicePorId.insert(e.id, elementos.size());
        elementos.append(e);
    }
    if (elementos.isEmpty())
    {
        saida() << "grade sem elementos em " << caminhoSaida << Qt::endl;
        return false;
    }

    // ---- trilhas: cada valor distinto de col/linha vira uma faixa com a folga dada ----
    const double folgaColunas = raiz.value(QStringLiteral("folgaColunas")).toDouble(70.0);
    const double folgaLinhas = raiz.value(QStringLiteral("folgaLinhas")).toDouble(56.0);

    const auto montarTrilhas = [](QVector<double> valores, const QVector<double>& tamanhos,
                                  double folga, double margem) {
        // valores[i] e a coordenada logica do elemento i; tamanhos[i] o tamanho no eixo
        QVector<double> distintos = valores;
        std::sort(distintos.begin(), distintos.end());
        distintos.erase(std::unique(distintos.begin(), distintos.end(),
                                    [](double a, double b) { return std::abs(a - b) < 1e-9; }),
                        distintos.end());
        QVector<double> maiores(distintos.size(), 0.0);
        for (int i = 0; i < valores.size(); ++i)
        {
            for (int t = 0; t < distintos.size(); ++t)
            {
                if (std::abs(valores[i] - distintos[t]) < 1e-9)
                {
                    maiores[t] = std::max(maiores[t], tamanhos[i]);
                }
            }
        }
        QVector<QPair<double, double>> trilhas;   // (valor logico, centro em px)
        double cursor = margem;
        for (int t = 0; t < distintos.size(); ++t)
        {
            trilhas.append(qMakePair(distintos[t], cursor + maiores[t] / 2.0));
            cursor += maiores[t] + folga;
        }
        return qMakePair(trilhas, cursor - folga + margem);
    };

    QVector<double> cols, linhas, largs, alts;
    for (const Elemento& e : elementos)
    {
        cols.append(e.col);
        linhas.append(e.linha);
        largs.append(e.tamanho.width());
        alts.append(e.tamanho.height());
    }
    const auto trilhasX = montarTrilhas(cols, largs, folgaColunas, 34.0);
    const auto trilhasY = montarTrilhas(linhas, alts, folgaLinhas, 44.0);

    const auto mapear = [](const QVector<QPair<double, double>>& trilhas, double valor) {
        // interpolacao linear entre trilhas, para pontos de passagem fracionarios
        if (valor <= trilhas.first().first)
        {
            return trilhas.first().second;
        }
        for (int i = 1; i < trilhas.size(); ++i)
        {
            if (valor <= trilhas[i].first + 1e-9)
            {
                const double f = (valor - trilhas[i - 1].first)
                                 / (trilhas[i].first - trilhas[i - 1].first);
                return trilhas[i - 1].second + f * (trilhas[i].second - trilhas[i - 1].second);
            }
        }
        return trilhas.last().second;
    };

    for (Elemento& e : elementos)
    {
        const QPointF centro(mapear(trilhasX.first, e.col), mapear(trilhasY.first, e.linha));
        e.area = QRectF(centro.x() - e.tamanho.width() / 2.0,
                        centro.y() - e.tamanho.height() / 2.0,
                        e.tamanho.width(), e.tamanho.height());
    }

    const double larguraConteudo = trilhasX.second;
    const double alturaConteudo = trilhasY.second;

    QImage imagem(static_cast<int>(larguraConteudo * ESCALA),
                  static_cast<int>(alturaConteudo * ESCALA),
                  QImage::Format_RGB32);
    imagem.fill(COR_FUNDO);
    QPainter p(&imagem);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.scale(ESCALA, ESCALA);

    desenharMoldura(p, QSizeF(larguraConteudo, alturaConteudo),
                    raiz.value(QStringLiteral("moldura")).toString());

    // Rotulos de ligacao sao desenhados por ultimo (sobre os elementos), senao
    // uma caixa vizinha cobre o texto quando o segmento e curto.
    struct RotuloPendente
    {
        QPointF posicao;
        QString texto;
    };
    QVector<RotuloPendente> rotulosPendentes;

    // ---- ligacoes primeiro (as linhas ficam por baixo dos elementos) ----
    const QJsonArray ligacoes = raiz.value(QStringLiteral("ligacoes")).toArray();
    for (const QJsonValue& valor : ligacoes)
    {
        const QJsonObject ligacao = valor.toObject();
        const int de = indicePorId.value(ligacao.value(QStringLiteral("de")).toString(), -1);
        const int para = indicePorId.value(ligacao.value(QStringLiteral("para")).toString(), -1);
        if (de < 0 || para < 0)
        {
            saida() << "ligacao com id desconhecido em " << caminhoSaida << Qt::endl;
            return false;
        }
        const QString tipo = ligacao.value(QStringLiteral("tipo")).toString(QStringLiteral("seta"));
        const QString rotulo = ligacao.value(QStringLiteral("rotulo")).toString();
        const QString rotuloOrigem = ligacao.value(QStringLiteral("rotuloOrigem")).toString();
        const QString rotuloDestino = ligacao.value(QStringLiteral("rotuloDestino")).toString();

        // pontos de passagem opcionais (coordenadas logicas de grade)
        QVector<QPointF> pontos;
        for (const QJsonValue& item : ligacao.value(QStringLiteral("via")).toArray())
        {
            const QJsonObject ponto = item.toObject();
            pontos.append(QPointF(mapear(trilhasX.first, ponto.value(QStringLiteral("col")).toDouble()),
                                  mapear(trilhasY.first, ponto.value(QStringLiteral("linha")).toDouble())));
        }

        const QPointF alvoInicial = pontos.isEmpty() ? elementos[para].area.center() : pontos.first();
        const QPointF alvoFinal = pontos.isEmpty() ? elementos[de].area.center() : pontos.last();
        const QPointF inicio = bordaDoElemento(elementos[de], alvoInicial);
        const QPointF fim = bordaDoElemento(elementos[para], alvoFinal);

        QVector<QPointF> caminho;
        caminho.append(inicio);
        caminho += pontos;
        caminho.append(fim);

        const bool tracejada = tipo == QLatin1String("tracejada")
                               || tipo == QLatin1String("realizacao");
        p.setPen(QPen(COR_LINHA, 1.3, tracejada ? Qt::DashLine : Qt::SolidLine));
        for (int i = 1; i < caminho.size(); ++i)
        {
            p.drawLine(caminho[i - 1], caminho[i]);
        }
        p.setPen(QPen(COR_LINHA, 1.3));

        const QPointF penultimo = caminho[caminho.size() - 2];
        const double anguloFim = std::atan2(fim.y() - penultimo.y(), fim.x() - penultimo.x());
        const QPointF segundo = caminho[1];
        const double anguloInicio = std::atan2(inicio.y() - segundo.y(), inicio.x() - segundo.x());

        if (tipo == QLatin1String("heranca") || tipo == QLatin1String("realizacao"))
        {
            desenharTriangulo(p, fim, anguloFim);
        }
        else if (tipo == QLatin1String("agregacao"))
        {
            desenharLosango(p, inicio, anguloInicio, false);
            desenharSetaAberta(p, fim, anguloFim);
        }
        else if (tipo == QLatin1String("composicao"))
        {
            desenharLosango(p, inicio, anguloInicio, true);
            desenharSetaAberta(p, fim, anguloFim);
        }
        else if (tipo != QLatin1String("associacao"))
        {
            desenharSetaAberta(p, fim, anguloFim);
        }

        // rotulo no ponto medio do segmento mais longo
        int maior = 0;
        double comprimentoMaior = -1.0;
        for (int i = 1; i < caminho.size(); ++i)
        {
            const double comprimento = QLineF(caminho[i - 1], caminho[i]).length();
            if (comprimento > comprimentoMaior)
            {
                comprimentoMaior = comprimento;
                maior = i;
            }
        }
        const QPointF meio = (caminho[maior - 1] + caminho[maior]) / 2.0;
        if (!rotulo.isEmpty())
        {
            // desloca o rotulo para cima da linha, proporcional a altura do texto
            const double altura = medirTexto(linhasDe(rotulo), fontePequena()).height();
            rotulosPendentes.append({meio + QPointF(0.0, -(altura / 2.0 + 5.0)), rotulo});
        }
        if (!rotuloOrigem.isEmpty())
        {
            rotulosPendentes.append({inicio + QPointF(0.0, -12.0), rotuloOrigem});
        }
        if (!rotuloDestino.isEmpty())
        {
            rotulosPendentes.append({fim + QPointF(0.0, -12.0), rotuloDestino});
        }
    }

    for (const Elemento& e : elementos)
    {
        desenharElemento(p, e);
    }

    for (const RotuloPendente& pendente : rotulosPendentes)
    {
        desenharRotuloDeLigacao(p, pendente.posicao, pendente.texto);
    }

    p.end();
    return salvarImagem(imagem, caminhoSaida);
}

// ================================== principal ==================================

bool gerarDeArquivo(const QString& entrada, const QString& saidaPng)
{
    QFile arquivo(entrada);
    if (!arquivo.open(QIODevice::ReadOnly))
    {
        saida() << "nao foi possivel abrir " << entrada << Qt::endl;
        return false;
    }
    QJsonParseError erro;
    const QJsonDocument documento = QJsonDocument::fromJson(arquivo.readAll(), &erro);
    if (documento.isNull())
    {
        saida() << entrada << ": JSON invalido - " << erro.errorString()
                << " (posicao " << erro.offset << ")" << Qt::endl;
        return false;
    }
    const QJsonObject raiz = documento.object();
    const QString tipo = raiz.value(QStringLiteral("tipo")).toString();
    if (tipo == QLatin1String("sequencia"))
    {
        return gerarSequencia(raiz, saidaPng);
    }
    return gerarGrade(raiz, saidaPng);
}

}

int main(int argc, char* argv[])
{
    QGuiApplication aplicacao(argc, argv);
    const QStringList argumentos = QCoreApplication::arguments();

    if (argumentos.size() == 4 && argumentos.at(1) == QLatin1String("--lote"))
    {
        const QDir origem(argumentos.at(2));
        const QString destino = argumentos.at(3);
        int falhas = 0;
        int total = 0;
        const QStringList arquivos = origem.entryList(QStringList() << QStringLiteral("*.json"),
                                                      QDir::Files, QDir::Name);
        for (const QString& nome : arquivos)
        {
            ++total;
            const QString png = QDir(destino).filePath(QFileInfo(nome).completeBaseName()
                                                       + QStringLiteral(".png"));
            if (!gerarDeArquivo(origem.filePath(nome), png))
            {
                ++falhas;
            }
        }
        saida() << total - falhas << "/" << total << " diagramas gerados" << Qt::endl;
        return falhas == 0 ? 0 : 1;
    }

    if (argumentos.size() != 3)
    {
        saida() << "uso: gerar-diagramas <entrada.json> <saida.png>" << Qt::endl
                << "     gerar-diagramas --lote <pasta-json> <pasta-png>" << Qt::endl;
        return 2;
    }
    return gerarDeArquivo(argumentos.at(1), argumentos.at(2)) ? 0 : 1;
}
