#ifndef GLSLCODECONVERT_H
#define GLSLCODECONVERT_H

#include <QWidget>
#include "../core/pluginterface.h"

namespace Ui {
class GlslCodeConvert;
}

namespace GlslTool {

class GlslCodeConvert : public QWidget, Core::PlugInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.sooncheer.QiTools.PlugInterface.049c19-1.0")
    Q_INTERFACES(Core::PlugInterface)
public:
    explicit GlslCodeConvert(QWidget *parent = nullptr);
    ~GlslCodeConvert() override;

    QString pluginName() const override { return tr("glslTool"); }

private Q_SLOTS:
    void onTextCodeChanged();

private:
    Ui::GlslCodeConvert *ui;
};

}

#endif // GLSLCODECONVERT_H
