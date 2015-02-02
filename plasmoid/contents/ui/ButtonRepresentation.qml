import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.private.menubar 1.0

PlasmaComponents.ToolButton {
    id: button
    text: (Plasmoid.Vertical === Plasmoid.formFactor) ? "" : i18n("Menubar")
    iconName: "show-menu"

    MenuComponent {
        id: menu
        model: DBusMenuApi.menuModel["root"]
    }

    onClicked: {
        if(menu.status === PlasmaComponents.DialogStatus.Open ) {
            return
        }

        if (Qt.application.layoutDirection === Qt.RightToLeft) {
            menu.open(button.width, button.height, 0)
        }
        else {
            menu.open(0, button.height, 0)
        }
    }
}
