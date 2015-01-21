// -*- coding: iso-8859-1 -*-
/*
 *   Author: nikolay <nikolay@none.org>
 *   Date: нд дек. 21 2014, 16:13:13
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.2
import org.kde.plasma.plasmoid 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2 as QtControls
import QtQuick.Controls.Styles 1.2 as QtQuickControlStyle
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.private.menubar 1.0

Loader {
    id: rootItem

    property bool useButtonFormFactorCfg: Plasmoid.configuration.useButtonFormFactor

    // We want fill width for horizontal panels with bar layout or for vertical panels
    Layout.fillWidth: ( ( Plasmoid.formFactor === Plasmoid.Horizontal ) && !useButtonFormFactorCfg ) || ( Plasmoid.formFactor === Plasmoid.Vertical )
    // Fill height only in horizontal panels
    Layout.fillHeight: Plasmoid.formFactor === Plasmoid.Horizontal

    Layout.preferredWidth: item.width
    Layout.preferredHeight: item.height

    Plasmoid.backgroundHints: Plasmoid.NoBackground
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation

    source: {
        console.debug("Trying to load...",  Plasmoid.formFactor, "  ", useButtonFormFactorCfg )
        if( Plasmoid.Vertical === Plasmoid.formFactor ) {
            return Plasmoid.file( "ui", "ButtonRepresentation.qml")
        }
        else {
            if( useButtonFormFactorCfg ) {
                return Plasmoid.file( "ui", "ButtonRepresentation.qml")
            }
            else {
                return Plasmoid.file( "ui", "BarRepresentation.qml")
            }
        }
    }

    Component.onCompleted: {
        DBusMenuApi.init(plasmoid)
        console.debug("onCompleted!")
    }

    onSourceChanged: console.debug("Source is", source)
}
