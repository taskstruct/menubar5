import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.private.menubar 1.0

PlasmaComponents.ToolButton {
    text: i18n("Menubar")
    iconName: "show-menu"

    MenuComponent {
        model: DBusMenuApi.menuModel["root"]
    }
}
