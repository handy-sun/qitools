#ifndef STABLE_H
#define STABLE_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <functional>
#include <initializer_list>

#ifndef qtout
#   define qtout qDebug()
#endif

#endif // STABLE_H

static constexpr qreal g_kMinimumApplicationFontPointSize =
#ifdef Q_OS_WIN
13.0
#else
12.0
#endif
;