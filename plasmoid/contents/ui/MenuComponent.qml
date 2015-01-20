import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.private.menubar 1.0

PlasmaComponents.Menu {
    id: menuCmp

    property var model

    onModelChanged: {
        clearMenuItems()

        for( var i = 0; i < model.length; ++i )
        {
            if( model[i].hasOwnProperty("label") )
            {
                var component2 = Qt.createComponent("MenuItemComponent.qml");
                if (component2.status === Component.Ready) {
                    var menuItem = component2.createObject(menuCmp);
                    menuItem.action = model[i].label

                    if( model[i].hasOwnProperty("submenu") ) {
                        var component1 = Qt.createComponent("MenuComponent.qml");
                        if (component1.status === Component.Ready) {
                            var subMenuItem = component1.createObject(menuItem);

                            subMenuItem.visualParent = menuItem.action
                            subMenuItem.model = model[i].submenu
                        }
                    }

                    menuCmp.addMenuItem(menuItem)
                }

                continue
            }

            if( model[i].hasOwnProperty("separator") )
            {
                var component3 = Qt.createComponent("MenuItemComponent.qml");
                if (component3.status === Component.Ready) {
                    var menuSep = component3.createObject(menuCmp);
                    menuSep.separator = true
                    menuSep.enabled = model[i].separator

                    menuCmp.addMenuItem(menuSep)
                }

                continue
            }
        }
    }
}
