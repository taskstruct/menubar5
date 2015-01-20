#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>

#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>

#include <KLocalizedString>
#include <KXmlGui/KActionCollection>
#include <KWindowSystem>

#include <Plasma/Containment>
#include <Plasma/ContainmentActions>
#include <Plasma/Corona>

#include "kappmenuimporter.h"
#include "rendereradaptor.h"
#include "menucloner.h"
#include "menuutils.h"
#include "windowmenumanager.h"
#include "menubaritem.h"

MenuBarItem::MenuBarItem(QQmlEngine *engine, QObject* parent): QObject(parent),
    m_engine(engine),
    m_appMenuImporter(nullptr),
    m_menuCloner(new MenuCloner(this)),
    m_windowMenuManager(new WindowMenuManager(this)),
    m_windowMenu(new QMenu),
    m_plasmaMenu(new QMenu)
{

}

MenuBarItem::~MenuBarItem()
{
    m_windowMenu->deleteLater();
    m_plasmaMenu->deleteLater();
}

void MenuBarItem::init( QObject* plasmoidRef )
{
    setPlasmoidRef(plasmoidRef);

    m_appMenuImporter = new KAppMenuImporter(this);

    //setup window menu
    QAction* action = m_windowMenu->addAction(i18n("File")); //TODO: Use application name
    action->setMenu(m_windowMenuManager->menu());

    //setup plasma menu
    m_plasmaMenu->addMenu(i18n("Plasma"));

    setupDBusInterface();


    connect(m_appMenuImporter, &KAppMenuImporter::windowRegistered, [this](WId wid){
        if (wid == m_activeWinId) {
            createMenuBar();
        }
    });

    connect(m_appMenuImporter, &KAppMenuImporter::windowUnregistered, [this](WId wid)
    {
        if (wid == m_activeWinId) {
            m_activeWinId = 0;
            createMenuBar();
        }
    });

    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &MenuBarItem::updateActiveWinId);

    updateActiveWinId();
}

void MenuBarItem::setupDBusInterface()
{
    static int id = 0;
    QString path = QString("/com/canonical/AppMenu/Renderer_%1").arg(id++);
    new RendererAdaptor(this);
    QDBusConnection::sessionBus().registerObject(path, this);
}

QString MenuBarItem::GetCurrentMenu(QDBusObjectPath& path) const
{
    QString service;
    MyDBusMenuImporter* importer = m_importers.value(m_activeWinId);
    if (importer) {
        service = importer->service();
        path = QDBusObjectPath(importer->path());
    } else {
        path = QDBusObjectPath("/");
    }
    return service;
}

void MenuBarItem::ActivateMenuItem(const QList<int>&)
{
}

QString MenuBarItem::DumpCurrentMenu() const
{
    return DumpMenu(m_activeWinId);
}

QString MenuBarItem::DumpMenu(xcb_window_t id) const
{
    MyDBusMenuImporter* importer = m_importers.value(id);
    QVariant variant = MenuUtils::variantFromMenu(importer ? importer->menu() : 0);
    //    QJson::Serializer serializer; TODO: port properly??
    return variant.toString();
}

void MenuBarItem::updateActiveWinId()
{
    WId id = KWindowSystem::activeWindow();

    qDebug() << "Plasma window id is " << m_plasmaDesktopWinId;
    qDebug() << "Active window id is " << id;

    if (id == m_activeWinId) {
        return;
    }

    if( /*!KWindowSystem::showingDesktop() &&*/ id == m_plasmaDesktopWinId) {
        // Do not update id if the active window is the one hosting this applet
        return;
    }

    m_activeWinId = id;
    createMenuBar();
}

static bool isDesktopWinId(WId winId)
{
    KWindowInfo info(winId, NET::WMWindowType);

    if(!info.valid()) {
        return false;
    }

    return NET::typeMatchesMask(info.windowType(NET::DesktopMask), NET::DesktopMask);
}

void MenuBarItem::createPlasmaContextMenu()
{
    if(!m_contaimentRef) {
        return;
    }

    QMenu* subMenu = m_plasmaMenu->actions().first()->menu();
    subMenu->clear();

    auto corona = m_contaimentRef->corona();

    auto desktop = corona->containmentForScreen( m_contaimentRef->screen() );

    if(!desktop) {
        return;
    }

    // get context menu
    auto rightButtonCtx = desktop->containmentActions().value(QString("RightButton;NoModifier"));

    for( auto action: rightButtonCtx->contextualActions() )
    {
        subMenu->addAction(action);
    }
}

static void fillMap(QVariantMap* map, QList<QAction*> actions, const QString &subMenuName = QString("root"))
{
    QVariantList children;

    for(QAction* action: actions)
    {
        QVariantMap child;

        if(action->isSeparator())
        {
            child.insert("separator", action->isVisible());
        }
        else
        {
            child.insert("label", QVariant::fromValue(action));
        }

        if(action->menu())
        {
            fillMap(&child, action->menu()->actions(), "submenu");
        }

        children << child;
    }

    map->insert(subMenuName, children);
}

void MenuBarItem::createMenuBar()
{
    qDebug() << "void MenuBarItem::createMenuBar() active " << m_activeWinId;

    WId winId = m_activeWinId;
    QMenu* menu = m_appMenuImporter->menuForWinId(winId);

    if(!menu)
    {
        qDebug() << "Valid menu";

        if (winId && !isDesktopWinId(winId)) {
            // We have an active window
            KWindowInfo info(winId, 0, NET::WM2TransientFor);
            if (!info.valid()) {
                return;
            }

            WId mainWinId = info.transientFor();

            if (mainWinId) {
                // We have a parent window, use a disabled version of its
                // menubar if it has one.
                QMenu* mainMenu = menuForWinId(mainWinId);
                if (mainMenu) {
                    m_menuCloner->setOriginalMenu(mainMenu);
                    menu = m_menuCloner->clonedMenu();

                    qDebug() << menu->objectName();
                }
            }
            if (!menu) {
                // No suitable menubar but we have a window, use the
                // generic window menu
                m_windowMenuManager->setWinId(winId);
                menu = m_windowMenu;
            }
        }
        else
        {
            // No active window, show a desktop menubar
            createPlasmaContextMenu();
            menu = m_plasmaMenu;
        }
    }

    m_rootItem.clear();

    fillMap( &m_rootItem, menu->actions() );

    qDebug() << "C++ menu dump: " << m_rootItem;

    emit menuModelChanged();

    //    if (useButtonFormFactor()) {
    //        createButtonsForButtonFormFactor(menu);
    //    } else {
    //        createButtonsForBarFormFactor(menu);
    //    }
}

QMenu* MenuBarItem::menuForWinId(xcb_window_t wid) const
{
    MyDBusMenuImporter* importer = m_importers.value(wid);
    return importer ? importer->menu() : nullptr;
}

#if 0
static void printItemsTree(QQuickItem* item)
{
    while(item)
    {
        qDebug() << "Item: " << item->objectName() << "  type = " << item->metaObject()->className();
        item = item->parentItem();
    }
}

static void printObjectTree(QObject *obj)
{
    while(obj)
    {
        qDebug() << "Obj: " << obj->objectName() << "  type = " << obj->metaObject()->className();
        obj = obj->parent();
    }
}

#endif

void MenuBarItem::setPlasmoidRef( QObject* applet )
{
    QQuickItem* qqi = qobject_cast<QQuickItem*>(applet);

//    printItemsTree(qqi);
//    printObjectTree(applet);

    if(qqi)
    {
        QQuickWindow *qqw = qqi->window();

        if(qqw)
        {
            QQuickView *qqv = qobject_cast<QQuickView *>(qqw);

            if(qqv) {
                qDebug() << "Plasma Window found!";
                m_plasmaDesktopWinId = qqv->winId();

                qDebug() << "m_engine = " << m_engine << "   plasmoid engine = " << qqv->engine();

                m_engine = qqv->engine();
            }
        }
    }

    // find contaiment object
    QObject *o = applet;
    while(o)
    {
        m_contaimentRef = qobject_cast<Plasma::Containment*>(o);
        if( m_contaimentRef )
        {
            qDebug() << "Contaiment found";
            break;
        }

        o = o->parent();
    }
}

void MenuBarItem::onMenuItemTriggered()
{
    qDebug() << "onMenuItemTriggered";

    QAction* qaction = sender()->property("_qaction").value<QAction*>();

    if(qaction) {
        qaction->triggered();
    }
}

void MenuBarItem::onMenuItemToggled(bool toggled)
{
    qDebug() << "onMenuItemToggled";

    QAction* qaction = sender()->property("_qaction").value<QAction*>();

    if(qaction) {
        qaction->toggled(toggled);
    }
}
