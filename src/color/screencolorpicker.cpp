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
    setFixedSize(QApplication::primaryScreen()->size());
    setCursor(QCursor(QPixmap(":/colorpicker.png"), 0, 19));
    setMouseTracking(true);
}

void ScreenColorPicker::grabColor()
{
    m_mousePos = QCursor::pos();
//    qtout << QApplication::desktop()->winId();
    m_pmScreen = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());

    update();    
}

void ScreenColorPicker::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect magnifier(m_mousePos.x(), m_mousePos.y(), 100, 100);
    QColor color = m_pmScreen.toImage().pixel(m_mousePos);

    painter.fillRect(rect(), m_pmScreen);
    painter.drawPixmap(magnifier, m_pmScreen.copy(m_mousePos.x() - 10, m_mousePos.y() - 10, 20, 20).scaled(100, 100));
    painter.drawRect(magnifier);
    painter.fillRect(magnifier.left(), magnifier.bottom(), magnifier.width() + 1, greyHeight, QColor(24, 24, 24));
    painter.fillRect(magnifier.left() + blockOffset, magnifier.bottom() + blockOffset, blockSize, blockSize, color);
    painter.setPen(QColor(20, 20, 20, 150));
    painter.drawLine((QPointF)magnifier.center() - QPointF(10.0, 0.0), (QPointF)magnifier.center() + QPointF(10.0, 0.0));
    painter.drawLine((QPointF)magnifier.center() - QPointF(0.0, 10.0), (QPointF)magnifier.center() + QPointF(0.0, 10.0));
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 8));
    painter.drawText(magnifier.adjusted(greyHeight, magnifier.height(), 0, greyHeight - blockOffset), Qt::AlignCenter,
                     QString().sprintf("(%03d,%03d,%03d)", color.red(), color.green(), color.blue()));
    painter.setPen(QColor(128, 0, 0));
    painter.drawRect(rect().adjusted(1, 0, -1, -1));
}

void ScreenColorPicker::mousePressEvent(QMouseEvent *event)
{
    emit pickFinished(event->button() == Qt::LeftButton, m_pmScreen.toImage().pixel(event->pos()));
    close();
}

void ScreenColorPicker::mouseMoveEvent(QMouseEvent *)
{
    m_mousePos = QCursor::pos();
    update();
}
