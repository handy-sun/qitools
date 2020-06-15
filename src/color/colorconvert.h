#ifndef COLORCONVERT_H
#define COLORCONVERT_H

#include <QWidget>

namespace Ui {
class ColorConvert;
}

class ColorConvert : public QWidget
{
    Q_OBJECT

public:
    explicit ColorConvert(QWidget *parent = nullptr);
    ~ColorConvert();

private slots:
    void on_toolButtonRgbValue_clicked();

private:
    Ui::ColorConvert *ui;
    QColor            m_color;

};

#endif // COLORCONVERT_H
