import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts

Rectangle {
    id: root
    width: 150
    height: width
    color: "transparent"
    property alias progress: content.progress
    property alias succProgs: upText.text
    property alias failProgs: downText.text
    // property var imgs: ["qrc:/res/png/k0.png",
    //                     "qrc:/res/png/k1.png",
    //                     "qrc:/res/png/k2.png"]
    ConicalGradient {
        id: content
        width: root.width * scal
        height: width
        x: (root.width - width) / 2
        y: x

        readonly property real scal: 0.9
        property double minValue: 0
        property double maxValue: 1
        property double progress: 0
        property string mainColor: "steelblue"
        property string errColor: "tomato"

        smooth: true
        source: Rectangle {
            width: content.width
            height: content.height
            color: "transparent"
            radius: width / 2
            border.width: width * 0.05
        }

        gradient: Gradient {
            GradientStop { position: 0.0; color: content.mainColor }
            GradientStop { position: content.progress - 0.001; color: content.mainColor }
            GradientStop { position: content.progress; color: Qt.rgba(content.mainColor.r, content.mainColor.g, content.mainColor.b, 0.33) }
            GradientStop { position: 1.0; color: Qt.rgba(content.mainColor.r, content.mainColor.g, content.mainColor.b, 0.33) }
        }
        Item {
            id: textArea
            width: content.width / 2
            height: width
            anchors.centerIn: content
            Text {
                id: upText
                text: "0"
                font.pixelSize: content.width / 8
                color: content.mainColor
                anchors.bottom: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: downText
                text: "0"
                font.pixelSize: content.width / 8
                color: content.errColor
                anchors.top: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        // onProgressChanged: {
        //     if(sqAni.running) sqAni.restart()
        //     else sqAni.start()
        //     switch(root.state)
        //     {
        //         case "ks0": root.state = "ks1"; return;
        //         case "ks1": root.state = "ks2"; return;
        //         case "ks2": root.state = "ks0"; return;
        //         default: root.state = "ks0"; return;
        //     }
        // }

        SequentialAnimation{
            id: sqAni

            ParallelAnimation
            {
                NumberAnimation {
                    target: content
                    properties: "x"
                    to: root.width * (1 - content.scal) / 2 + 10
                    duration: 50
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    target: content
                    properties: "y"
                    to: root.width * (1 - content.scal) / 2 - 10
                    duration: 50
                    easing.type: Easing.InOutQuad
                }
            }

            NumberAnimation {
                target: content
                properties: "x,y"
                to: root.width * (1 - content.scal) / 2
                duration: 50
                easing.type: Easing.InOutQuad
            }
        }
    }
    // Image {
    //     id: img
    //     source: imgs[0]
    //     width: root.width * 0.12
    //     height: width
    //     x: 0
    //     y: root.height - height
    // }
    // state: "ks0"
    // states: [
    //     State { name: "ks0"; PropertyChanges { target: img; source: imgs[0] } },
    //     State { name: "ks1"; PropertyChanges { target: img; source: imgs[1] } },
    //     State { name: "ks2"; PropertyChanges { target: img; source: imgs[2] } }
    // ]
}

