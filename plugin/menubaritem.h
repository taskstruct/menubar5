#ifndef MENUBARITEM_H
#define MENUBARITEM_H

#include <QtCore/QObject>
#include <QtCore/QHash>

#include <QtGui/QIcon>

#include <QtWidgets/QAction>

#include <QtDBus/QDBusObjectPath>

#include <xcb/xcb.h>

class KAppMenuImporter;
class MenuCloner;
class MyDBusMenuImporter;
class QAction;
class QMenu;
class QQmlEngine;
class WindowMenuManager;

namespace Plasma {
    class Containment;
}

class MenuBarItem: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap menuModel READ menuModel NOTIFY menuModelChanged)

public:
    explicit MenuBarItem( QQmlEngine *engine, QObject* parent = 0 );
    ~MenuBarItem();

    void classBegin() {}
    void componentComplete() { updateActiveWinId(); }

    Q_INVOKABLE void init( QObject* plasmoidRef );

    Q_INVOKABLE QUrl getIconName( const QIcon& icon ) { return icon.name(); }

    void setPlasmoidRef( QObject* applet );

    QVariantMap menuModel() const { return m_rootItem; }

public Q_SLOTS:
    // DBus interface
    QString GetCurrentMenu(QDBusObjectPath&) const;
    void ActivateMenuItem(const QList<int>&);
    QString DumpCurrentMenu() const;
    QString DumpMenu(xcb_window_t id) const;
    // /DBus interface

private Q_SLOTS:
    void onMenuItemTriggered();
    void onMenuItemToggled(bool toggled);

Q_SIGNALS:
    void menuModelChanged();

private:
    typedef QHash<xcb_window_t, MyDBusMenuImporter*> ImporterForWId;

    void setupDBusInterface();

    void updateActiveWinId();
    void createMenuBar();
    void createPlasmaContextMenu();
    void createQmlMenuBar(QMenu *menu);
    void populateMenu(QMenu *menu, QObject *parent);
    void createMenuItem(QAction *a, QObject *parent);

    void findPlasmaWindow();

    QMenu* menuForWinId(xcb_window_t wid) const;

    QQmlEngine *m_engine;
    KAppMenuImporter* m_appMenuImporter;
    MenuCloner* m_menuCloner;
    WindowMenuManager* m_windowMenuManager;
    QMenu* m_windowMenu;
    QMenu* m_plasmaMenu;
    xcb_window_t m_activeWinId = 0;
    WId m_plasmaDesktopWinId = 0;
    ImporterForWId m_importers;

    // applet contaiment
    Plasma::Containment *m_contaimentRef = nullptr;

    QVariantMap m_rootItem;
};

#endif // MENUBARITEM_H
