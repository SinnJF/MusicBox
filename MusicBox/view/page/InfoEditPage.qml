import QtQuick

Item {
    id: root
    Rectangle {
        anchors.fill: parent
        color: "gray"
        Text {
            id: name
            anchors.centerIn: parent
            text: qsTr("待续")
        }
    }
}
