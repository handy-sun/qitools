#include "qitoolswindow.h"
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QX11Info>
#include <X11/Xutil.h>
#if 1
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setvbuf(stdout, nullptr, _IONBF, 1024);

//    Display *d = XOpenDisplay((char *)NULL);
//    Window root = XDefaultRootWindow(d);
//    XWindowAttributes attr;
//    XGetWindowAttributes(d, root, &attr);
//    XMapRaised(d, root);
//    qDebug() << "Display:" << d << XOpenDisplay((char *)NULL) << attr.width << attr.height;
//    XImage *ximg = XGetImage(d, root, 0, 0, attr.width, attr.height,
//                             AllPlanes, ZPixmap);
//    if (!ximg)
//    {
//        qDebug() << "Ximg is:" << ximg;
//        return 1;
//    }

    QSettings ini(qApp->applicationDirPath() + "/QiTools.ini", QSettings::IniFormat);
//    qSetMessagePattern("%{message} [%{file}:%{line} - %{qthreadptr} | %{time MMdd-h:mm:ss.zzz}]");
    qSetMessagePattern("%{message} [%{function}()=>%{line} - %{threadid} | %{time MMdd-h:mm:ss.zzz}]");
    auto _ba = ini.value("Preference/geometry").toByteArray();
    int styleMode = ini.value("Preference/styleMode").toInt();
    qApp->setStyleSheet(0 == styleMode ? "file:///:/QiTools.css" : "file:///./QiTools.css");
    QiToolsWindow w;
//    QFont font;
//    font.setFamily("MS Shell Dlg 2"); // Tahoma 宋体
//    qApp->setFont(font);
    if (!_ba.isEmpty())
        w.restoreGeometry(_ba);
    else
        w.resize(1280, 720);
    w.show();

    int ret = a.exec();
//    XCloseDisplay(d);
    return ret;
}
#else
int main(int argc, char *argv[])
{
        Display *d = XOpenDisplay(NULL);
//    Display *d = XOpenDisplay(":0.0");
    //    auto *d = QX11Info::display();
    Window root = XDefaultRootWindow(d);
//    XWindowAttributes attr;
//    XGetWindowAttributes(d, root, &attr);
    XMapRaised(d, root);
//    qDebug() << "Display:" << d << root << attr.width << attr.height;
    XImage *ximg = XGetImage(d, root, 0, 0, 1440, 900,
                             AllPlanes, ZPixmap);
    if (!ximg)
    {
        qDebug() << "Ximg is:" << ximg;
        return -1;
    }

    XCloseDisplay(d);
    return 0;
}
#endif
