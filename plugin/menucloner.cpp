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

#include "menucloner.h"
#include "menucloner.moc"

// Qt
#include <QMenu>
#include <QDebug>

MenuCloner::MenuCloner(QObject* parent)
: QObject(parent)
, mOriginalMenu(0)
, mClonedMenu(new QMenu)
{}

MenuCloner::~MenuCloner()
{
    mClonedMenu->deleteLater();
}

void MenuCloner::setOriginalMenu(QMenu* menu)
{
    mOriginalMenu = menu;
    mClonedMenu->clear();
    mOriginalMenuFromCloneMenu.clear();
    if (!mOriginalMenu) {
        return;
    }
    Q_FOREACH(QAction* action, mOriginalMenu->actions()) {
        if (action->menu()) {
            QMenu* clonedSubMenu = mClonedMenu->addMenu(action->text());
            mOriginalMenuFromCloneMenu[clonedSubMenu] = action->menu();
            connect(clonedSubMenu, SIGNAL(aboutToShow()), SLOT(slotAboutToShow()));
        }
    }
}

inline QAction* cloneAction(QAction* src)
{
    QAction* dst = new QAction(src->icon(), src->text(), 0);
    dst->setSeparator(src->isSeparator());
    dst->setShortcut(src->shortcut());
    dst->setEnabled(false);

    if (src->menu()) {
        dst->setMenu(new QMenu(0));
    }
    if (src->isCheckable()) {
        dst->setCheckable(true);
        dst->setChecked(src->isChecked());
        if (src->actionGroup() && src->actionGroup()->isExclusive()) {
            QActionGroup* group = new QActionGroup(dst);
            group->addAction(dst);
        }
    }

    return dst;
}

void MenuCloner::slotAboutToShow()
{
    QMenu* menu = qobject_cast<QMenu*>(sender());
    if (!menu) {
        qWarning() << "Not called from aboutToShow()!";
        return;
    }
    QMenu* originalMenu = mOriginalMenuFromCloneMenu.value(menu);
    if (!originalMenu) {
        qWarning() << "No original menu for this menu!";
        return;
    }

    Q_FOREACH(QAction* action, originalMenu->actions()) {
        menu->addAction(cloneAction(action));
    }

    // Let's assume we don't need to be updated again
    disconnect(menu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShow()));
}
