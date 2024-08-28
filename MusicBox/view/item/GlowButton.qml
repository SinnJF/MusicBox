import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

QtStyleLabel{
    id: root

    signal clicked()
    property alias text : label.text
    property alias btnIcon : label.icon.source
    property color mclr: "steelblue"

    width: 72
    height: 36
    fillcolor: mclr
    scale: mouseArea.pressed ? 0.9 : 1.0
    RectangularGlow{
        anchors.centerIn: parent
        width: parent.width - 8
        height: parent.height - 2
        cached: false
        spread: 0.2
        color: mclr
        glowRadius: 6
        cornerRadius: 14
    }
    IconLabel{
        id: label
        anchors.centerIn: parent
        icon.source: "qrc:/res/svg/Done.svg"
        icon.color: "#FFFFFF"
        icon.width: 20
        icon.height: 20
        //font.bold: true
        font.pixelSize: 15
        //font.italic: true
        text: qsTr(" 选择文件 ")
        color: "#FFFFFF"
    }
    MouseArea{
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            root.clicked()
        }
    }
}
