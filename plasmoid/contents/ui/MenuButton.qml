import QtQuick 2.0
//import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.plasma.components 2.0 as PlasmaComponents
//import org.kde.plasma.components.styles 2.0 as PlasmaComponentsStyles

FocusScope {
    id: button

    implicitWidth: label.implicitWidth + 16
    implicitHeight: label.implicitHeight + 6

    property alias text: label.text
    property alias iconSource: icon.source
    property alias submenu: menuDelegate.model

    //TODO: remove. All main menu elements don't have icons
    readonly property bool hasIcon: icon.status === Image.Ready || icon.status === Image.Loading

    property MenuComponent menu: MenuComponent {
        id: menuDelegate
    }

    function adjustAlpha(clr, a) {
        return Qt.rgba(clr.r, clr.g, clr.b, a);
    }

    function popupMenu() {
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

    SystemPalette { id: sysPalette; colorGroup: SystemPalette.Active }

    Binding {
        target: menu
        property: "visualParent"
        value: button
    }

    Rectangle {
        id: background
        anchors.centerIn: parent
        implicitWidth: parent.width - 2
        implicitHeight: parent.height - 2
        border.width: 1
        border.color: (button.activeFocus || behavior.containsMouse) ? sysPalette.highlight : button.adjustAlpha(sysPalette.windowText, 0.25)
        radius: 2.5
        color: behavior.pressed ? sysPalette.highlight : sysPalette.button
        visible: behavior.containsMouse || button.activeFocus
    }

    RowLayout {
        anchors.centerIn: parent

        Image {
            id: icon
            visible: hasIcon
        }

        Text {
            id: label
            color: behavior.pressed ? sysPalette.highlightedText : sysPalette.buttonText
        }
    }

    MouseArea {
        id: behavior

        hoverEnabled: true

        anchors.fill: parent

        onClicked: popupMenu()
    }
}
