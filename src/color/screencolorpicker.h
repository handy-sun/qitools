﻿#ifndef SCREENCOLORPICKER_H
#define SCREENCOLORPICKER_H

#include <QDialog>

class ScreenColorPicker : public QDialog
{
    Q_OBJECT
public:
    explicit ScreenColorPicker(QWidget *parent = nullptr);
    void grabColor();

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPixmap     m_pmScreen;
    QPoint      m_mousePos;
    QRect       m_multiScreenRect;

    void setMultiScreen(); // 多屏时 获得包含各屏幕位置的最小矩形并刷新 geometry
    void drawPickedRect(QPainter *painter, const QRect &magnifier, const QColor &pickColor) const;

signals:
    void pickFinished(bool isUseful, const QColor &color);
    //void windowShowState(int state);
};

#endif // SCREENCOLORPICKER_H
