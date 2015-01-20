#include "menubarplugin.h"
#include "menubaritem.h"

#include <QtQml>

static QObject *dbusmenuapi_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)

    MenuBarItem *dBusMenuInstance = new MenuBarItem(engine);
    return dBusMenuInstance;
}

void MenuBarPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.private.menubar"));

    //@uri org.kde.plasma.private.menubar
    qmlRegisterSingletonType<MenuBarItem>(uri, 1, 0, "DBusMenuApi", dbusmenuapi_singletontype_provider);
}
