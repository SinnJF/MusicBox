import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TextField {
    id: root

    height: 25
    hoverEnabled: true
    selectByMouse: true
    font.pixelSize: 13
    Layout.fillWidth: true
    background: Rectangle {
        color: "aliceblue"
        radius: 3
        border.width: 1
        border.color: "tomato"
    }
}
