/*
 * Plasma applet to display application window menus
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "menuutils.h"

// Qt
#include <QMenu>
#include <QVariant>

namespace MenuUtils
{

static void fillMap(QVariantMap* map, QList<QAction*> actions)
{
    QVariantList children;
    Q_FOREACH(QAction* action, actions) {
        QVariantMap child;
        if (action->isSeparator()) {
            child.insert("separator", true);
        } else {
            child.insert("label", action->text());
        }
        if (action->menu()) {
            fillMap(&child, action->menu()->actions());
        }
        children << child;
    }
    map->insert("submenu", children);
}

QVariant variantFromMenu(QMenu* menu)
{
    QVariantMap root;
    if (menu) {
        fillMap(&root, menu->actions());
    }
    return root;
}

} // namespace
