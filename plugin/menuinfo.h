/*
 * Copyright (c) 2010 Canonical Ltd.
 * Copyright (c) 2010 Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MENUINFO_H
#define MENUINFO_H

struct MenuInfo
{
    MenuInfo()
    : winId(0)
    , path("/")
    {}

    uint winId;
    QString service;
    QDBusObjectPath path;
};
Q_DECLARE_METATYPE(MenuInfo)

typedef QList<MenuInfo> MenuInfoList;
Q_DECLARE_METATYPE(MenuInfoList)
#endif /* MENUINFO_H */
