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
#ifndef MENUUTILS_H
#define MENUUTILS_H

class QMenu;
class QVariant;

namespace MenuUtils
{

QVariant variantFromMenu(QMenu* menu);

} // namespace
#endif /* MENUUTILS_H */
