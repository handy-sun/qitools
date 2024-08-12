#ifndef IMAGETOOLWIDGET_H
#define IMAGETOOLWIDGET_H

#include <QWidget>
#include <QImage>
#include <QFileInfo>
#include "../core/pluginterface.h"

namespace Ui {
class ImageToolWidget;
}

namespace ImageConvert {

class ImageToolWidget : public QWidget, Core::PlugInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.sooncheer.QiTools.PlugInterface.049c19-1.0")
    Q_INTERFACES(Core::PlugInterface)
public:
    explicit ImageToolWidget(QWidget *parent = nullptr);
    ~ImageToolWidget() override;

    QString pluginName() const override { return tr("ImageConvert"); }

private:
    Ui::ImageToolWidget *ui;
    QImage              m_image;
    QFileInfo           m_fileInfo;

private Q_SLOTS:
    void on_pushButtonSaveDestDir_clicked();
    void on_pushButtonOpenImg_clicked();

};

}

#endif // IMAGETOOLWIDGET_H
