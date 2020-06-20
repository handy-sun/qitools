#ifndef SCREENCOLORPICKER_H
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
    void mouseMoveEvent(QMouseEvent *) override;

private:
    QPixmap m_pmScreen;
    QPoint  m_mousePos;

signals:
    void pickFinished(bool isUseful, const QColor &color);
    //void windowShowState(int state);
};

#endif // SCREENCOLORPICKER_H
