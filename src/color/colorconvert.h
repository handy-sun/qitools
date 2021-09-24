#ifndef COLORCONVERT_H
#define COLORCONVERT_H

#include <QWidget>

namespace Ui {
class ColorConvert;
}

class QTableWidget;
class QColorDialog;
class ScreenColorPicker;

class ColorConvert : public QWidget
{
    Q_OBJECT

public:
    explicit ColorConvert(QWidget *parent = nullptr);
    ~ColorConvert();

private:
    Ui::ColorConvert    *ui;
    QTableWidget        *m_table;
    ScreenColorPicker   *m_scrPicker;
    QColor              m_color;
    QPixmap             m_pmDecoration;
    //decltype(connect(0, 0, 0, 0)) m_connection;
    static QColorDialog *s_clrDlg;

    void initTableWidget();
    void convertFromRgb();
    void convertFromHex();
    void convertFromDec();
    void convertFromGL();
    void setColorValue(const QVector<int> &usedRows);

public Q_SLOTS:
    void slot_PickFinished(bool isUseful, const QColor &color);

private Q_SLOTS:
    void on_toolButtonPick_clicked();
    void onTableCellChanged(int row, int);
    void on_toolButtonPal_clicked();

};

#endif // COLORCONVERT_H
