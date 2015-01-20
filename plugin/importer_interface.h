/*
  This file is part of the KDE project.

  Copyright (c) 2011 Lionel Chauvin <megabigbug@yahoo.fr>
  Copyright (c) 2011 Cédric Bellegarde <gnumdk@gmail.com>

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#ifndef REGISTRAR_INTERFACE_H
#define REGISTRAR_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <qwindowdefs.h>
#include "menuinfo.h"

/*
 * Proxy class for interface com.canonical.AppMenu.Registrar
 */
class ComCanonicalAppMenuRegistrarInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.canonical.AppMenu.Registrar"; }

public:
    ComCanonicalAppMenuRegistrarInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ComCanonicalAppMenuRegistrarInterface();

Q_SIGNALS: // SIGNALS
    void WindowRegistered(WId windowId, const QString &service, const QDBusObjectPath &menuObjectPath);
    void WindowUnregistered(WId windowId);
};

namespace com {
  namespace canonical {
    namespace AppMenu {
      typedef ::ComCanonicalAppMenuRegistrarInterface Registrar;
    }
  }
}
#endif
