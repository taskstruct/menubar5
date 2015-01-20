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
#ifndef MENUCLONER_H
#define MENUCLONER_H

// Qt
#include <QHash>
#include <QObject>

class QMenu;

/**
 * A class which takes a menu and returns a disabled clone.
 * It owns the cloned menu.
 */
class MenuCloner : public QObject
{
Q_OBJECT
public:
    MenuCloner(QObject* parent);
    ~MenuCloner();

    void setOriginalMenu(QMenu*);

    QMenu* clonedMenu() const { return mClonedMenu; }

private Q_SLOTS:
    void slotAboutToShow();

private:
    QMenu* mOriginalMenu;
    QMenu* mClonedMenu;

    QHash<QMenu*, QMenu*> mOriginalMenuFromCloneMenu;
};

#endif /* MENUCLONER_H */
