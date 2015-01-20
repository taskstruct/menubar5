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

Item {
    id: rootItem

    Layout.fillWidth: true
    Layout.fillHeight: true

    onWidthChanged: layout.recreateLayout()

    Component {
        id: menuButtonCmp

        MenuButton {
        }
    }

    Connections {
        target: DBusMenuApi

        onMenuModelChanged: {
            changeAnimation.start()
            layout.recreateLayout()
        }
    }

    Item {
        id: layout

        anchors.fill: parent

        property real spacing: 2.0
        property var __buttonList: []

        function deleteOldItems() {
            console.log("items: ", children.length)
            console.log("dynitems: ", __buttonList.length)
            while(__buttonList.length ) {
                var item = __buttonList.pop()
                console.debug("destroy", item.toString())
                item.destroy()
                item = null
            }

//            for(var j = 0; j < __buttonList.length; ++j) {
//                __buttonList[j].destroy()
//                __buttonList[j] = null
//            }

            for(var i = 0; i < children.length; i++ ) {
                if( children[i] !== null ) {
                    children[i].destroy()
                    children[i] = null
                }
            }

            console.log("items: ", children.length)
            console.log("item type: ", children[0])
            console.log("dynitems: ", __buttonList.length)

            extender.visible = false

            console.log("rect: ", childrenRect)
        }

        function canAddItem(itemWidth, numberOfItems) {

            if( __buttonList.length == ( numberOfItems - 1 ) ) {
                // this is the last item.
                // check if it fit in layout
                return ( childrenRect.width <= layout.width )
            }
            else {
                // there is at least one more item, so we need room for extender
                return ( childrenRect.width <= ( layout.width - extender.width ) )
            }
        }

        function recreateLayout()
        {
            if(width == 0.0) return

            deleteOldItems()

            // add new items
            var numberOfItems = DBusMenuApi.menuModel["root"].length

            console.log("2 items: ", children.length)
            console.log("2 item type: ", children[0])
            console.log("2 dynitems: ", __buttonList.length)

            do{
                var modelData = DBusMenuApi.menuModel["root"][__buttonList.length]
                var instance = menuButtonCmp.createObject( layout, { "x" : childrenRect.width + spacing, "text" : modelData["label"].text.replace('&', '') } )

                if( canAddItem(instance.width, numberOfItems) ) {
                    // add item's menu
                    instance.submenu = modelData.submenu;
                    __buttonList.push(instance)
                }
                else {
                    // there is no place for item. Destroy it and exit loop
                    instance.destroy()
                    instance = null
                    break;
                }
            }
            while( __buttonList.length < numberOfItems )

            console.log("3 items: ", children.length)
            console.log("3 item type: ", children[0])
            console.log("3 dynitems: ", __buttonList.length)

            if( __buttonList.length < numberOfItems ) {
                // show extender after last item
                extender.x = childrenRect.width + spacing
                extender.visible = true

                // fill extender menu
                //TODO
            }
        }
    }

    PlasmaComponents.ToolButton {
        id: extender
        iconName: "arrow-down"
        visible: false;
    }

    PropertyAnimation {
        id: changeAnimation
        target: parent
        property: "opacity"
        duration: units.longDuration
        from: 0.0
        to: 1.0
    }
}
