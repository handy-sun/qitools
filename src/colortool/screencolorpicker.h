#ifndef SCREENCOLORPICKER_H
#define SCREENCOLORPICKER_H

#include <QColor>
#include <QDialog>
#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QRect>

namespace ColorTool {

class ScreenColorPicker : public QDialog
{
    Q_OBJECT
public:
    explicit ScreenColorPicker(QWidget *parent = nullptr);
    bool grabDesktopPixmap();

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPixmap     m_pmScreen;
    QImage      m_imgScreen;
    QPoint      m_mousePos;

    QRect getUnitedScreenRect(); // 多屏时,获得包含各屏幕位置的最小矩形
    QPoint screenImagePos() const;
    QColor pickedColor() const;
    void drawPickedRect(QPainter *painter, const QRect &magnifier, const QColor &pickColor) const;

Q_SIGNALS:
    void pickFinished(bool isUseful, const QColor &color);

};

}

#endif // SCREENCOLORPICKER_H
