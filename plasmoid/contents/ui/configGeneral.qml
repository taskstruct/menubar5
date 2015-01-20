import QtQuick 2.0
import QtQuick.Controls 1.1 as QtControls

Item {
    width: childrenRect.width
    height: childrenRect.height

    property alias cfg_useButtonFormFactor: useButtonFormFactorCheckBox.checked

    QtControls.CheckBox {
        id: useButtonFormFactorCheckBox
        text: i18n("Use Button Form Factor")
    }
}

