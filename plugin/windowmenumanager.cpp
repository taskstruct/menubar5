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
#include "windowmenumanager.h"
#include <windowmenumanager.moc>

// Qt
#include <QX11Info>

// KDE
#include <QDebug>
#include <QIcon>
#include <KI18n/KLocalizedString>
#include <KWindowSystem>
#include <netwm.h>

WindowMenuManager::WindowMenuManager(QObject* parent)
: QObject(parent)
, mMenu(new QMenu)
, mWid(0)
{
    connect(mMenu, SIGNAL(destroyed()), SLOT(deleteLater()));
    connect(mMenu, SIGNAL(aboutToShow()), SLOT(updateActions()));

    mCloseAction = mMenu->addAction(i18n("Close"), this, SLOT(closeWindow()));
    mCloseAction->setIcon(QIcon::fromTheme("window-close"));
}

WindowMenuManager::~WindowMenuManager()
{
    mMenu->deleteLater();
}

void WindowMenuManager::updateActions()
{
    if (!mWid) {
        qWarning() << "No winId!";
        return;
    }
    NET::Properties properties = 0;
    NET::Properties2 properties2 = NET::WM2AllowedActions;
    NETWinInfo ni(QX11Info::connection(), mWid, QX11Info::appRootWindow(), properties, properties2);
    mCloseAction->setEnabled(ni.allowedActions() & NET::ActionClose);
}

void WindowMenuManager::setWinId(WId wid)
{
    mWid = wid;
}

void WindowMenuManager::closeWindow()
{
    NETRootInfo ri( QX11Info::connection(), NET::CloseWindow );
    ri.closeWindowRequest(mWid);
}
