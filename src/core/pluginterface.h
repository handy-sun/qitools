#ifndef PLUGINTERFACE_H
#define PLUGINTERFACE_H

#include <QObject>

namespace Core {

class PlugInterface
{
public:
    virtual ~PlugInterface() {}

    virtual QString pluginName() const = 0;
//    virtual QWidget *pluginUI() const = 0;

};

//PlugInterface::~PlugInterface(){}

}

Q_DECLARE_INTERFACE(Core::PlugInterface, "org.sooncheer.QiTools.PlugInterface.049c19-1.0")


#endif // PLUGINTERFACE_H
