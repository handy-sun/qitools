#ifndef IMAGETOOLWIDGET_H
#define IMAGETOOLWIDGET_H

#include <QWidget>
#include <QFileInfo>

namespace Ui {
class ImageToolWidget;
}

class ImageToolWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageToolWidget(QWidget *parent = Q_NULLPTR);
    ~ImageToolWidget();

private:
    Ui::ImageToolWidget *ui;
    QImage              m_image;
    QFileInfo           m_fileInfo;

private Q_SLOTS:
    void on_pushButtonSaveDestDir_clicked();
    void on_pushButtonOpenImg_clicked();

};

#endif // IMAGETOOLWIDGET_H
