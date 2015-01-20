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
#ifndef MENUBARAPPLET_H
#define MENUBARAPPLET_H

// Qt
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGraphicsLinearLayout>
#include <QtWidgets/QGraphicsView>
#include <QDBusObjectPath>

// KDE
#include <Plasma/Applet>

class KAppMenuImporter;
class KConfigDialog;
class MenuCloner;
class MyDBusMenuImporter;
class WindowMenuManager;
class MenuWidget;

class MenuBarApplet : public Plasma::Applet
{
Q_OBJECT
public:
    MenuBarApplet(QObject* parent, const QVariantList& args);
    ~MenuBarApplet();

    virtual void init();

    virtual void createConfigurationInterface(KConfigDialog*);

public Q_SLOTS:
    // DBus interface
    QString GetCurrentMenu(QDBusObjectPath&) const;
    void ActivateMenuItem(const QList<int>&);
    QString DumpCurrentMenu() const;
    QString DumpMenu(WId id) const;
    // /DBus interface

private Q_SLOTS:
    void createMenuBar();
    void applyConfig();
    void slotActivated();
    void updateActiveWinId();
    void slotWindowRegistered(WId);
    void slotWindowUnregistered(WId);
    void slotActionActivationRequested(QAction* action);
    void fillDesktopMenu();

private:
    typedef QHash<WId, MyDBusMenuImporter*> ImporterForWId;

    QGraphicsLinearLayout* mLayout;
    KAppMenuImporter* mKAppMenuImporter;
    QMenu* mDesktopMenu;
    WindowMenuManager* mWindowMenuManager;
    QMenu* mWindowMenu;
    MenuCloner* mMenuCloner;

    ImporterForWId mImporters;
    WId mActiveWinId;
    MenuWidget* mMenuWidget;

    QCheckBox* mConfigButtonFormFactor;

    void createButtonsForBarFormFactor(QMenu*);
    void createButtonsForButtonFormFactor(QMenu*);
    void updateSizePolicy();

    bool useButtonFormFactor() const;

    void setupDesktopMenu();
    void setupWindowMenu();
    void setupDBusInterface();
    QMenu* menuForWinId(WId) const;
};

#endif /* MENUBARAPPLET_H */
