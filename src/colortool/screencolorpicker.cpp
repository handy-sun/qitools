#include "screencolorpicker.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QDebug>
#include <QtGlobal>

static const int greyHeight = 30;
static const int blockOffset = 5;
static const int blockSize = greyHeight - 2 * blockOffset;

using namespace ColorTool;

ScreenColorPicker::ScreenColorPicker(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags()
                   | Qt::BypassWindowManagerHint
                   | Qt::FramelessWindowHint
                   );

//    setAttribute(Qt::WA_TranslucentBackground, true);
    setCursor(QCursor(QPixmap(":/colortool/colorpicker.png"), 0, 19));
    setMouseTracking(true);
    setGeometry(getUnitedScreenRect());
}

bool ScreenColorPicker::grabDesktopPixmap()
{
    QRect unitedGeometry = getUnitedScreenRect();
    setGeometry(unitedGeometry);
    m_mousePos = QCursor::pos() - unitedGeometry.topLeft();

    m_pmScreen = QApplication::primaryScreen()->grabWindow(
        0,
        unitedGeometry.x(),
        unitedGeometry.y(),
        unitedGeometry.width(),
        unitedGeometry.height()
    );

    if (m_pmScreen.isNull())
        return false;

    QScreen *screen = QApplication::screenAt(QCursor::pos());
    if (screen)
    {
        m_pmScreen.setDevicePixelRatio(screen->devicePixelRatio());
    }
    m_imgScreen = m_pmScreen.toImage();
    update();
    return true;
}

QRect ScreenColorPicker::getUnitedScreenRect()
{
    QRect geometry;
#if 0
    QPainterPath paPath;
    auto scrList = QApplication::screens();
    for (const QScreen *scr : scrList)
    {
        paPath.addRect(scr->geometry());
    }
    geometry = paPath.boundingRect().toRect();
#else
    for (const QScreen *screen : QGuiApplication::screens())
    {
        geometry = geometry.united(screen->geometry());
    }
#endif
    return geometry;
}

QPoint ScreenColorPicker::screenImagePos() const
{
    if (m_imgScreen.isNull())
        return QPoint();

    const qreal dpr = m_pmScreen.devicePixelRatio();
    const QPoint imagePos(qRound(m_mousePos.x() * dpr),
                          qRound(m_mousePos.y() * dpr));
    return QPoint(qBound(0, imagePos.x(), m_imgScreen.width() - 1),
                  qBound(0, imagePos.y(), m_imgScreen.height() - 1));
}

QColor ScreenColorPicker::pickedColor() const
{
    if (m_imgScreen.isNull())
        return QColor();

    return QColor::fromRgba(m_imgScreen.pixel(screenImagePos()));
}

void ScreenColorPicker::drawPickedRect(QPainter *painter, const QRect &magnifier, const QColor &pickColor) const
{
    painter->save();

    const QPoint imagePos = screenImagePos();
    const int radius = qMax(1, qRound(10 * m_pmScreen.devicePixelRatio()));
    const QRect sampleRect = QRect(imagePos.x() - radius,
                                   imagePos.y() - radius,
                                   radius * 2,
                                   radius * 2).intersected(m_imgScreen.rect());
    painter->drawPixmap(magnifier,
                        QPixmap::fromImage(m_imgScreen.copy(sampleRect))
                            .scaled(magnifier.width(), magnifier.height()));
    painter->drawRect(magnifier);
    painter->fillRect(magnifier.left(), magnifier.bottom(), magnifier.width() + 1, greyHeight, QColor(24, 24, 24));
    painter->fillRect(magnifier.left() + blockOffset, magnifier.bottom() + blockOffset, blockSize, blockSize, pickColor);
    painter->setPen(QColor(20, 20, 20, 150));
    painter->drawLine((QPointF)magnifier.center() - QPointF(10.0, 0.0), (QPointF)magnifier.center() + QPointF(10.0, 0.0));
    painter->drawLine((QPointF)magnifier.center() - QPointF(0.0, 10.0), (QPointF)magnifier.center() + QPointF(0.0, 10.0));
    painter->setPen(Qt::white);
    painter->setFont(qApp->font());
    painter->drawText(magnifier.adjusted(greyHeight, magnifier.height(), 0, greyHeight - blockOffset), Qt::AlignCenter,
                      QString("(%1,%2,%3)").arg(pickColor.red(), 3).arg(pickColor.green(), 3).arg(pickColor.blue(), 3));

    painter->restore();
}

void ScreenColorPicker::paintEvent(QPaintEvent *)
{
    if (m_pmScreen.isNull())
        return;

    QPainter painter(this);
    QRect magnifier(m_mousePos.x(), m_mousePos.y(), 100, 100);
    QColor color = pickedColor();

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
    Q_EMIT pickFinished(event->button() == Qt::LeftButton, pickedColor());
    close();
}

void ScreenColorPicker::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->pos(); // event->pos() 是以 geometry 左上为基准的
    update();
}
