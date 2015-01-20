#ifndef MENUBARPLUGIN_H
#define MENUBARPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class QQmlEngine;

class MenuBarPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // MENUBARPLUGIN_H
