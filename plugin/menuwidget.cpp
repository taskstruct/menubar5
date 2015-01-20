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
#include "menuwidget.moc"

// Qt
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsLinearLayout>
#include <QGraphicsView>
#include <QMenu>
#include <QTimer>
#include <QToolButton>

// KDE
#include <KDebug>
#include <Plasma/Containment>
#include <Plasma/Corona>
#include <Plasma/Theme>

MenuWidget::MenuWidget(Plasma::Applet* applet)
: QGraphicsWidget(applet)
, mApplet(applet)
, mMouseChecker(new QTimer(this))
, mUpdateButtonsTimer(new QTimer(this))
, mRootMenu(0)
, mOverflowButton(createButton())
, mCurrentButton(0)
, mLastMousePosition(-1, -1)
{
    mOverflowButton->setText(QString::fromUtf8("…"));
    mOverflowButton->setMenu(new QMenu);
    connect(mOverflowButton->menu(), SIGNAL(aboutToHide()), SLOT(slotAboutToHideMenu()));
    mOverflowButton->menu()->installEventFilter(this);

    mMouseChecker->setInterval(100);
    connect(mMouseChecker, SIGNAL(timeout()), SLOT(checkMousePosition()));

    mUpdateButtonsTimer->setSingleShot(true);
    connect(mUpdateButtonsTimer, SIGNAL(timeout()), SLOT(updateButtons()));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setMinimumSize(mOverflowButton->minimumSize());
}

void MenuWidget::setMenu(QMenu* menu)
{
    mRootMenu = menu;
    mRootMenu->installEventFilter(this);
    updateButtons();
}

void MenuWidget::updateButtons()
{
    if (mCurrentButton) {
        // Do not update if there is a current button: we do not want to change
        // the menubar while the user is using it.
        // It would be especially wrong because the opened menu would then not
        // be associated with its button anymore, causing the menu to stay
        // opened when navigating to the next menu with arrows.
        //
        // Start mUpdateButtonsTimer in the unprobable case that this method is
        // called from setMenu() and mCurrentButton is not NULL.
        mUpdateButtonsTimer->start();
        return;
    }
    mUpdateButtonsTimer->stop();

    // Create buttons for each actions in mRootMenu, reusing existing buttons if possible
    QList<MenuButton*>::Iterator
        it = mMenuButtonList.begin(),
        end = mMenuButtonList.end();
    Q_FOREACH(QAction* action, mRootMenu->actions()) {
        if (!action->isVisible()) {
            continue;
        }
        if (action->isSeparator()) {
            continue;
        }
        QMenu* menu = action->menu();
        if (!menu) {
            kWarning() << "No menu in action" << action->text();
            continue;
        }

        // Create or reuse a button
        MenuButton* button;
        if (it == end) {
            button = createButton();
            mMenuButtonList << button;
        } else {
            button = *it;
            ++it;
        }

        // Init button
        button->setText(action->text());
        button->setMenu(menu);
        disconnect(menu, 0, this, 0); // Ensure we are only connected to aboutToHide() once
        connect(menu, SIGNAL(aboutToHide()), SLOT(slotAboutToHideMenu()));
        menu->installEventFilter(this);
    }

    // Delete unused buttons
    int count = end - it;
    for (;count > 0; --count) {
        delete mMenuButtonList.takeLast();
    }
    updateButtonsGeometries();
    updateGeometry();
}

bool MenuWidget::eventFilter(QObject* object, QEvent* event)
{
    bool filtered;
    if (object == mRootMenu) {
        filtered = rootMenuEventFilter(event);
    } else {
        filtered = subMenuEventFilter(static_cast<QMenu*>(object), event);
    }
    return filtered ? true : QGraphicsWidget::eventFilter(object, event);
}

bool MenuWidget::rootMenuEventFilter(QEvent* event)
{
    switch (event->type()) {
    case QEvent::ActionAdded:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
        mUpdateButtonsTimer->start();
        break;
    default:
        break;
    }
    return false;
}

bool MenuWidget::subMenuEventFilter(QMenu* menu, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        menu->removeEventFilter(this);
        QApplication::sendEvent(menu, event);
        menu->installEventFilter(this);
        if (!event->isAccepted()) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            switch (keyEvent->key()) {
            case Qt::Key_Left:
                showNextPrevMenu(false);
                break;
            case Qt::Key_Right:
                showNextPrevMenu(true);
                break;
            default:
                break;
            }
        }
        return true;
    }
    return false;
}

MenuButton* MenuWidget::createButton()
{
    MenuButton* button = new MenuButton(this);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(5);
    shadow->setOffset(QPointF(0, 1));
    shadow->setColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::BackgroundColor));
    setGraphicsEffect(shadow);

    connect(button, SIGNAL(clicked()), SLOT(slotButtonClicked()));
    return button;
}

void MenuWidget::slotButtonClicked()
{
    MenuButton* button = qobject_cast<MenuButton*>(sender());
    if (!button) {
        kWarning() << "Not called by a MenuButton!";
        return;
    }
    showMenu(button);
}

void MenuWidget::showNextPrevMenu(bool next)
{
    int index;
    MenuButton* button = 0;
    if (mCurrentButton == mOverflowButton) {
        if (next) {
            button = mMenuButtonList.first();
        } else {
            Q_FOREACH(MenuButton* tmp, mMenuButtonList) {
                if (tmp->isVisible()) {
                    button = tmp;
                } else {
                    break;
                }
            }
        }
    } else {
        index = mMenuButtonList.indexOf(mCurrentButton);
        if (index == -1) {
            kWarning() << "Couldn't find button!";
            return;
        }
        if (next) {
            index = (index + 1) % mMenuButtonList.count();
        } else {
            index = (index == 0 ? mMenuButtonList.count() : index) - 1;
        }
        button = mMenuButtonList.at(index);
        if (!button->isVisible()) {
            button = mOverflowButton;
        }
    }
    if (button) {
        showMenu(button);
    }
}

void MenuWidget::showMenu(MenuButton* button)
{
    if (mCurrentButton) {
        bool justHide = mCurrentButton == button;
        mCurrentButton->menu()->hide();
        // Note: When we reach this line mCurrentButton is 0: it has been
        // updated by slotAboutToHideMenu().
        if (justHide) {
            return;
        }
    }
    QMenu* menu = button->menu();
    if (!menu) {
        return;
    }
    QPoint pos = mApplet->containment()->corona()->popupPosition(button, menu->sizeHint());

    mCurrentButton = button;
    mCurrentButton->nativeWidget()->setDown(true);
    // Process gui events so that the first opened button gets some time to
    // repaint itself. If we don't its highlight border stays there while
    // scrubling.
    qApp->processEvents();
    menu->popup(pos);

    startMouseChecker();
}

void MenuWidget::startMouseChecker()
{
    mLastMousePosition = mApplet->view()->mapFromGlobal(QCursor::pos());
    mMouseChecker->start();
}

void MenuWidget::checkMousePosition()
{
    // Checks whether mouse moved while a menu is opened. In this case when the
    // mouse moves over another button, we show this button menu.
    QPoint pos = mApplet->view()->mapFromGlobal(QCursor::pos());
    if (pos == mLastMousePosition) {
        // Do nothing if mouse hasn't moved. Otherwise if users press an arrow
        // key to open the next menu, we would come back to the menu under the
        // mouse cursor as soon as we came back here.
        // See: https://bugs.launchpad.net/plasma-widget-menubar/+bug/878786
        return;
    }
    mLastMousePosition = pos;

    QGraphicsItem* item = mApplet->view()->itemAt(pos);
    if (!item) {
        return;
    }

    MenuButton* buttonBelow = qobject_cast<MenuButton*>(item->toGraphicsObject());
    if (!buttonBelow) {
        return;
    }

    if (buttonBelow != mCurrentButton) {
        showMenu(buttonBelow);
    }
}

void MenuWidget::slotAboutToHideMenu()
{
    if (!mCurrentButton) {
        // This can happen when menu was opened and user activated another
        // window.
        mMouseChecker->stop();
        return;
    }
    if (mCurrentButton->menu() == sender()) {
        mCurrentButton->nativeWidget()->setDown(false);
        mCurrentButton = 0;
        mMouseChecker->stop();
    } else if (mCurrentButton != mOverflowButton) {
        kWarning() << "Not called from menu associated with mCurrentButton!";
    }
}

void MenuWidget::activate()
{
    MenuButton* button = mMenuButtonList.first();
    if (!button) {
        kWarning() << "No button found!";
        return;
    }

    button->nativeWidget()->animateClick();
}

void MenuWidget::activateAction(QAction* action)
{
    Q_FOREACH(MenuButton* button, mMenuButtonList) {
        if (button->menu() == action->menu()) {
            button->nativeWidget()->animateClick();
            break;
        }
    }
}

void MenuWidget::activateActionInMenu(QAction* action)
{
    MenuButton* button = mMenuButtonList.first();
    if (!button) {
        kWarning() << "No buttons!";
        return;
    }
    button->nativeWidget()->animateClick();
    button->menu()->setActiveAction(action);
}

void MenuWidget::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    QGraphicsWidget::resizeEvent(event);
    updateButtonsGeometries();
}

void MenuWidget::updateButtonsGeometries()
{
    qreal x = 0;
    bool overflow = false;
    int widgetWidth = size().width();
    int widgetHeight = size().height();
    QMenu* overflowMenu = mOverflowButton->menu();
    overflowMenu->clear();

    int lastVisibleIndex = -1;
    Q_FOREACH(MenuButton* button, mMenuButtonList) {
        // Note: we define geometry for both visible and invisible buttons because
        // the geometry of invisible buttons is used for sizeHint
        qreal buttonWidth = button->preferredWidth();
        button->setGeometry(QRectF(x, 0, buttonWidth, widgetHeight));
        x += buttonWidth;
        if (x <= widgetWidth) {
            button->show();
            ++lastVisibleIndex;
        } else {
            button->hide();
            overflowMenu->addMenu(button->menu());
            overflow = true;
        }
    }

    if (overflow) {
        mOverflowButton->show();
        qreal buttonWidth = mOverflowButton->preferredWidth();

        // Continue hiding buttons from the right until there is enough room for
        // the overflow button (we use a loop in the unlikely event that more
        // than one button need to be hidden)
        MenuButton* lastVisibleButton = 0;
        for (; lastVisibleIndex >= 0; --lastVisibleIndex) {
            lastVisibleButton = mMenuButtonList.at(lastVisibleIndex);
            if (lastVisibleButton->geometry().right() > widgetWidth - buttonWidth) {
                lastVisibleButton->hide();
                overflowMenu->insertMenu(overflowMenu->actions().first(), lastVisibleButton->menu());
            } else {
                break;
            }
        }
        x = lastVisibleIndex >= 0 ? lastVisibleButton->geometry().right() : 0;
        mOverflowButton->setGeometry(QRectF(x, 0, buttonWidth, widgetHeight));
    } else {
        mOverflowButton->hide();
    }
}

QSizeF MenuWidget::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    QSizeF fullSize(0, 0);
    Q_FOREACH(MenuButton* button, mMenuButtonList) {
        fullSize.rwidth() += button->minimumWidth();
        fullSize.setHeight(qMax(fullSize.height(), button->minimumHeight()));
    }

    if (which == Qt::MinimumSize) {
        return mOverflowButton->minimumSize();
    } if (which == Qt::PreferredSize) {
        return fullSize;
    } else {
        return QSize(fullSize.width(), constraint.height());
    }
}
