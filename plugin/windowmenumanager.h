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
#ifndef WINDOWMENU_H
#define WINDOWMENU_H

// Qt
#include <QMenu>

// KDE
#include <netwm.h>

class QAction;

class WindowMenuManager : public QObject
{
    Q_OBJECT
public:
    WindowMenuManager(QObject*);
    ~WindowMenuManager();

    QMenu* menu() const { return mMenu; }

    void setWinId(WId);

private Q_SLOTS:
    void updateActions();
    void closeWindow();

private:
    QMenu* mMenu;
    WId mWid;

    QAction* mCloseAction;
};



#endif /* WINDOWMENU_H */
