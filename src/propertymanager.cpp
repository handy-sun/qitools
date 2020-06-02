#include "propertymanager.h"
#include <QHash>

PropertyManager::PropertyManager()
{
    m_propertyType["READ"] = [](const QString &type, const QString &name, const QString &value, const QString &)->QString
    {
        return QString("%1 %2() const { return %3; }\n").arg(type).arg(value).arg(name);
    };

    m_propertyType["WRITE"] = [](const QString &type, const QString &name, const QString &value, const QString &notifyValue)->QString
    {
        if (notifyValue.isEmpty())
        return QString("void %1(const %2 &newValue)\n{ if ( newValue == %3 ) { return; } %4 = newValue;%5 }\n").
                arg( value ).
                arg( type ).
                arg( name ).
                arg( name ).
                arg( (notifyValue.isEmpty() ) ? ( "" ) : ( QString( " emit %1( %2 );" ).arg( notifyValue ).arg( name ) ) );
    };

    m_propertyType[ "NOTIFY" ] = []( const QString &type, const QString &name, const QString &value, const QString &, const bool &, const bool & )->QString
    {
        return QString( "Q_SIGNAL void %1(const %2 %3);\n" ).
                arg( value ).
                arg( type ).
                arg( name );
    };

    m_propertyType[ "RESET" ] = []( const QString &type, const QString &name, const QString &value, const QString &notifyValue, const bool &withSlot, const bool &withInline )->QString
    {
        return QString( "%1%2void %3() \n{ if ( %4 == %5() ) { return; } %6 = %7();%8 }\n" ).
                arg( ( withSlot ) ? ( "public: Q_SLOT " ) : ( "" ) ).
                arg( ( withInline ) ? ( "inline " ) : ( "" ) ).
                arg( value ).
                arg( name ).
                arg( type ).
                arg( name ).
                arg( type ).
                arg( ( notifyValue.isEmpty() ) ? ( "" ) : ( QString( " emit %1( %2 );" ).arg( notifyValue ).arg( name ) ) );
    };
}
