#include "screencolorpicker.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>

static const int greyHeight = 30;
static const int blockOffset = 5;
static const int blockSize = greyHeight - 2 * blockOffset;

ScreenColorPicker::ScreenColorPicker(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
//    setFixedSize(QApplication::primaryScreen()->size());
    setCursor(QCursor(QPixmap(":/colorpicker.png"), 0, 19));
    setMouseTracking(true);

    setMultiScreen();
}

void ScreenColorPicker::grabColor()
{
    setMultiScreen();
    m_mousePos = QCursor::pos() - m_multiScreenRect.topLeft();
//    qtout << QApplication::desktop()->winId();
    m_pmScreen = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(),
                                                           m_multiScreenRect.x(), m_multiScreenRect.y(),
                                                           m_multiScreenRect.width(), m_multiScreenRect.height());
    update();
}

void ScreenColorPicker::setMultiScreen()
{
    QPainterPath paPath;
    auto scrList = QApplication::screens();
    for (const QScreen *scr : scrList)
    {
         paPath.addRect(scr->geometry());
    }
    m_multiScreenRect = paPath.boundingRect().toRect();
    setGeometry(m_multiScreenRect);
}

void ScreenColorPicker::drawPickedRect(QPainter *painter, const QRect &magnifier, const QColor &pickColor) const
{
    painter->save();

    painter->drawPixmap(magnifier, m_pmScreen.copy(m_mousePos.x() - 10, m_mousePos.y() - 10, 20, 20).scaled(magnifier.width(), magnifier.height()));
    painter->drawRect(magnifier);
    painter->fillRect(magnifier.left(), magnifier.bottom(), magnifier.width() + 1, greyHeight, QColor(24, 24, 24));
    painter->fillRect(magnifier.left() + blockOffset, magnifier.bottom() + blockOffset, blockSize, blockSize, pickColor);
    painter->setPen(QColor(20, 20, 20, 150));
    painter->drawLine((QPointF)magnifier.center() - QPointF(10.0, 0.0), (QPointF)magnifier.center() + QPointF(10.0, 0.0));
    painter->drawLine((QPointF)magnifier.center() - QPointF(0.0, 10.0), (QPointF)magnifier.center() + QPointF(0.0, 10.0));
    painter->setPen(Qt::white);
    painter->setFont(QFont("Microsoft YaHei", 8));
    painter->drawText(magnifier.adjusted(greyHeight, magnifier.height(), 0, greyHeight - blockOffset), Qt::AlignCenter,
                     QString().sprintf("(%03d,%03d,%03d)", pickColor.red(), pickColor.green(), pickColor.blue()));

    painter->restore();
}

void ScreenColorPicker::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect magnifier(m_mousePos.x(), m_mousePos.y(), 100, 100);
    QColor color = m_pmScreen.toImage().pixel(m_mousePos);

    if (m_mousePos.x() > rect().right() - magnifier.width())
    {
        magnifier.translate(-magnifier.width(), 0);
    }
    if (m_mousePos.y() > rect().bottom() - magnifier.height() - greyHeight)
    {
        magnifier.translate(0, - magnifier.height() - greyHeight);
    }

    painter.fillRect(rect(), m_pmScreen);

    drawPickedRect(&painter, magnifier, color);

    painter.setPen(QColor(128, 0, 0));
    painter.drawRect(rect().adjusted(0, 0, -1, -1)); // 外边红框
}

void ScreenColorPicker::mousePressEvent(QMouseEvent *event)
{
    emit pickFinished(event->button() == Qt::LeftButton, m_pmScreen.toImage().pixel(m_mousePos));
    close();
}

void ScreenColorPicker::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->pos(); // event->pos() 是以 geometry 左上为基准的
    update();
}
