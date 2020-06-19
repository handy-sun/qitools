#ifndef COLORCONVERT_H
#define COLORCONVERT_H

#include <QWidget>

namespace Ui {
class ColorConvert;
}

class QTableWidget;

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
    QTableWidget     *m_table;
    QColor            m_color;
    QPixmap           m_pmDecoration;

    void initTableWidget();
};

#endif // COLORCONVERT_H
