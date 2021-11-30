#ifndef PROPERTYGENERATOR_H
#define PROPERTYGENERATOR_H

#include <QWidget>
#include "../core/pluginterface.h"

namespace Ui {
class PropertyGenerator;
}

namespace CodeGen {

class PropertyManager;

class PropertyGenerator : public QWidget, Core::PlugInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.sooncheer.QiTools.PlugInterface.049c19-1.0")
    Q_INTERFACES(Core::PlugInterface)

public:
    explicit PropertyGenerator(QWidget *parent = nullptr);
    ~PropertyGenerator() override;

    QString pluginName() const override { return tr("CodeGenerator"); }

private:
    Ui::PropertyGenerator   *ui;
    PropertyManager         *m_manager;

private Q_SLOTS:
    void on_pushBtnGenCode_clicked();
    void on_pushBtnGenProperty_clicked();

};

}

#endif // PROPERTYGENERATOR_H
