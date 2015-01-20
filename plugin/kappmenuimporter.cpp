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

#include "kappmenuimporter.h"

#include <QApplication>
#include <QDBusInterface>
#include <QDBusReply>

#include <qdebug.h>

#include "kappmenuimporter.moc"

static const char *KDED_SERVICE = "org.kde.kded5";
static const char *KAPPMENU_SERVICE = "org.kde.kappmenu";
static const char *KDED_INTERFACE = "org.kde.kded5";
static const char *KAPPMENU_INTERFACE = "org.kde.kappmenu";
static const char *KDED_PATH = "/kded";
static const char *KAPPMENU_PATH = "/KAppMenu";

KAppMenuImporter::KAppMenuImporter(QObject *parent): QObject(parent)
{
    QDBusInterface kded(KDED_SERVICE, KDED_PATH, KDED_INTERFACE);

    QDBusReply<bool> reply = kded.call("loadModule", "appmenu");

    if (!reply.isValid()) {
       qFatal("Error talking to KDED");
       return;
    }
    if (!reply.value()) {
       qWarning("KDED could not load appmenud");
       return;
    }

    qDebug() << "Appmenu module loaded in kded";

    mRegistrar = new com::canonical::AppMenu::Registrar("com.canonical.AppMenu.Registrar",
                                                        "/com/canonical/AppMenu/Registrar",
                                                        QDBusConnection::sessionBus());

    if(mRegistrar) {
        if(!mRegistrar->isValid()) {
            qDebug() << "Name: " << mRegistrar->lastError().name();
            qDebug() << "Message" << mRegistrar->lastError().message();
        }
    }


    bool isConn = QDBusConnection::sessionBus().connect(KAPPMENU_SERVICE, KAPPMENU_PATH, KAPPMENU_INTERFACE, "WindowRegistered",
                 this,  SLOT(slotWindowRegistered(qulonglong, const QString&, const QDBusObjectPath&)));

    qDebug() << "WindowRegistered " << isConn;

    isConn = QDBusConnection::sessionBus().connect(KAPPMENU_SERVICE, KAPPMENU_PATH, KAPPMENU_INTERFACE, "WindowUnregistered",
                 this,  SLOT(slotWindowUnregistered(qulonglong)));

    qDebug() << "WindowUnregistered " << isConn;
}

KAppMenuImporter::~KAppMenuImporter()
{
}

QMenu* KAppMenuImporter::menuForWinId(xcb_window_t wid)
{
    qDebug() << "KAppMenuImporter::menuForWinId " << importers.contains(wid);

    MyDBusMenuImporter* importer = importers.value(wid);

    return importer ? importer->menu() : 0;
}

bool KAppMenuImporter::available()
{
    static const char *APPMENU_INTERFACE = "com.canonical.AppMenu.Registrar";
    static const char *APPMENU_PATH = "/com/canonical/AppMenu/Registrar";
    QDBusInterface appmenu(APPMENU_INTERFACE, APPMENU_PATH, APPMENU_INTERFACE);
    QDBusReply<QDBusArgument> reply = appmenu.call("GetMenus");
    return reply.isValid();
}

void KAppMenuImporter::slotWindowRegistered(qulonglong id, const QString& service, const QDBusObjectPath& path)
{
    qDebug() << "KAppMenuImporter::slotWindowRegistered " << id;
    delete importers.take(id);
    MyDBusMenuImporter* importer = new MyDBusMenuImporter(id, service, &mIcons, path.path(), this);
    importers.insert(id, importer);
    connect(importer, SIGNAL(actionActivationRequested(QAction*)),
            SLOT(slotActionActivationRequested(QAction*)));
    emit windowRegistered(id);
}

void KAppMenuImporter::slotWindowUnregistered(qulonglong id)
{
    MyDBusMenuImporter* importer = importers.take(id);
    if (importer) {
        importer->deleteLater();
    }

    emit windowUnregistered(id);
}

void KAppMenuImporter::slotActionActivationRequested(QAction* a)
{
    MyDBusMenuImporter *importer = static_cast<MyDBusMenuImporter*>(sender());
    emit actionActivationRequested(importer->wid(), a);
}
