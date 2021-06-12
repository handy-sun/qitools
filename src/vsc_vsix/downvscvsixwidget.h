#ifndef DOWNVSCVSIXWIDGET_H
#define DOWNVSCVSIXWIDGET_H

#include <QWidget>

namespace Ui {
class DownVscVsixWidget;
}

class DownVscVsixWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DownVscVsixWidget(QWidget *parent = nullptr);
    ~DownVscVsixWidget();

private:
    Ui::DownVscVsixWidget *ui;

private Q_SLOTS:
    void on_pushButtonDownload_clicked();
    void on_pushButtonGenUrl_clicked();

};

#endif // DOWNVSCVSIXWIDGET_H
