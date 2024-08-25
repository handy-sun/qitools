#ifndef COLORCONVERT_H
#define COLORCONVERT_H

#include <QWidget>
#include "../core/pluginterface.h"

namespace Ui {
class ColorConvert;
}

class QTableWidget;
class QColorDialog;

namespace ColorTool {

class ScreenColorPicker;

class ColorConvert : public QWidget, Core::PlugInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.sooncheer.QiTools.PlugInterface.049c19-1.0")
    Q_INTERFACES(Core::PlugInterface)

public:
    explicit ColorConvert(QWidget *parent = nullptr);
    ~ColorConvert() override;

    QString pluginName() const override { return tr("ColorTool"); }

private:
    Ui::ColorConvert    *ui;
    QTableWidget        *m_table;
    ScreenColorPicker   *m_scrPicker;
    QColor              m_color;
    QPixmap             m_pmDecoration;

    static QColorDialog *s_clrDlg;

    void initTableWidget();
    void convertFromRgb();
    void convertFromHex();
    void convertFromDec();
    void convertFromGL();

    void updateTblExceptRow(int except);

public Q_SLOTS:
    void slot_PickFinished(bool isUseful, const QColor &color);

private Q_SLOTS:
    void on_toolButtonPick_clicked();
    void onTableCellChanged(int row, int);
    void on_toolButtonPal_clicked();

};

}

#endif // COLORCONVERT_H
