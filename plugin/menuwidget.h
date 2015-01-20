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
#ifndef MENUWIDGET_H
#define MENUWIDGET_H

// Qt
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QToolButton>

// KDE
#include <Plasma/Applet>
#include <Plasma/ToolButton>

class QMenu;

class MenuButton : public Plasma::ToolButton
{
    Q_OBJECT
public:
    MenuButton(QGraphicsWidget* parent)
    : Plasma::ToolButton(parent)
    , mMenu(0)
    {}

    void setMenu(QMenu* menu) { mMenu = menu; }

    QMenu* menu() const { return mMenu; }

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
    {
        QSizeF sh = Plasma::ToolButton::sizeHint(which, constraint);
        if (which == Qt::MinimumSize || which == Qt::PreferredSize) {
            sh.setHeight(nativeWidget()->fontMetrics().height());
        }
        return sh;
    }

protected:
    // Let the event propagate so that the applet context menu is shown
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
    {
        event->ignore();
    }

private:
    QMenu* mMenu;
};


class MenuWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    MenuWidget(Plasma::Applet* applet);
    void setMenu(QMenu*);
    void activate();
    void activateAction(QAction*);
    void activateActionInMenu(QAction* action);

    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSize()) const;

protected:
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void resizeEvent(QGraphicsSceneResizeEvent* event);

private Q_SLOTS:
    void slotButtonClicked();
    void showMenu(MenuButton*);
    void checkMousePosition();
    void slotAboutToHideMenu();
    void updateButtons();

private:
    Plasma::Applet* mApplet;
    QTimer* mMouseChecker;
    QTimer* mUpdateButtonsTimer;
    QMenu* mRootMenu;
    QList<MenuButton*> mMenuButtonList;
    MenuButton* mOverflowButton;
    MenuButton* mCurrentButton;
    QPoint mLastMousePosition;

    void showNextPrevMenu(bool next);
    MenuButton* createButton();

    bool rootMenuEventFilter(QEvent*);
    bool subMenuEventFilter(QMenu*, QEvent*);

    void updateButtonsGeometries();
    void startMouseChecker();
};
#endif /* MENUWIDGET_H */
