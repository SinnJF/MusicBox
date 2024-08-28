import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
//import QtGraphicalEffects 1.15

Item {
    id: root
    //anchors.fill: parent
    opacity: 0.9

    property real scl
    property var imgs: ["qrc:/res/png/k0.png",
                        "qrc:/res/png/k1.png",
                        "qrc:/res/png/k2.png"]

    state: "ks0"
    states: [
        State { name: "ks0"; PropertyChanges { target: floatBtn.background.children[0]; source: imgs[0] } },    //...
        State { name: "ks1"; PropertyChanges { target: floatBtn.background.children[0]; source: imgs[1] } },
        State { name: "ks2"; PropertyChanges { target: floatBtn.background.children[0]; source: imgs[2] } }
    ]

    function addLog(log) {
        logEdit.append(log)
    }

    function clearLog() {
        logEdit.clear()
    }

    Rectangle {
        id:logWin
        width: 0
        height: 0
        x: floatBtn.x + floatBtn.width / 2
        y: floatBtn.y + floatBtn.height / 2
        opacity: 0.9
        border.color: Qt.lighter("grey")

        ScrollView {
            anchors.fill: parent
            TextArea {
                id: logEdit
                wrapMode: TextEdit.WrapAnywhere
                clip: true
                enabled: false
                font.pixelSize: 10
            }
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        }
    }
//    DropShadow {
//         anchors.fill: logWin
//         horizontalOffset: 3
//         verticalOffset: 3
//         radius: 8.0
//         color: "#80000000"
//         source: logWin
//    }
    RoundButton {
        id: floatBtn
        width: 30
        height: width
        x: root.width - floatBtn.width// / 2
        y: root.height - floatBtn.height * 3
        radius: width / 2
        z: 1
        background: /*Image {
            width: parent.width
            height: parent.height
            anchors.centerIn: parent
            source: "qrc:/res/svg/Rolling.svg"
        }*/
        Item {
            width: parent.width
            height: parent.height
            Image {
                //id: img
                source: imgs[0]
                width: parent.width
                height: parent.height
                anchors.centerIn: parent
            }
        }
        onClicked: {
            // if(btnRota.running) {
            //     btnRota.stop()
            if(timer.running) {
                timer.stop()
                //floatBtn.background.source = "qrc:/res/svg/Roll.svg"
                //floatIcon.source = "qrc:/res/svg/Roll.svg"
                logWinHide.stop()
                logWinShow.start()
            }
            else {
                //var c = floatBtn.background.source = "qrc:/res/svg/Rolling.svg"
                //floatIcon.source = "qrc:/res/svg/Rolling.svg"
                timer.start()
                //btnRota.start()
                logWinShow.stop()
                logWinHide.start()
            }
        }
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            onClicked: {
                floatBtn.clicked()
            }
            onPositionChanged: {
                //console.log(mouseX + " " + mouseY)
            }
            onReleased: {
                //console.log(floatBtn.x + " " + root.width)
                if(Math.abs(floatBtn.x) < floatBtn.width * 2) {
                    moveXAni.to = 0;//-1 * floatBtn.width / 2
                    moveXAni.start()
                    return
                } else if(Math.abs(root.width - floatBtn.x) < floatBtn.width * 2) {
                    moveXAni.to = root.width - floatBtn.width;// / 2
                    moveXAni.start()
                    return
                } else if(Math.abs(floatBtn.y) < floatBtn.height * 2) {
                    moveYAni.to = 0;//-1 * floatBtn.height / 2
                    moveYAni.start()
                } else if(Math.abs(root.height - floatBtn.y) < floatBtn.height * 2) {
                    moveYAni.to = root.height - floatBtn.height;// / 2
                    moveYAni.start()
                }
            }
        }
    }

    NumberAnimation {
        id: moveXAni
        target: floatBtn
        property: "x"
        duration: 500
    }
    NumberAnimation {
        id: moveYAni
        target: floatBtn
        property: "y"
        duration: 500
    }
    RotationAnimation {
        id: btnRota
        target: floatBtn
        from: floatBtn.rotation
        to: floatBtn.rotation + 360
        duration: 4000
        loops: Animation.Infinite
    }
    ParallelAnimation {
        id: logWinHide
        NumberAnimation {
            target: logWin
            property: "x"
            to: floatBtn.x + floatBtn.width / 2
        }
        NumberAnimation {
            target: logWin
            property: "y"
            to: floatBtn.y + floatBtn.height / 2
        }
        //WARNING:日志窗强行变小会有警告：scrollbar的visible和textarea的implicitHeight
        NumberAnimation {
            target: logWin
            property: "width"
            to: 0
        }
        NumberAnimation {
            target: logWin
            property: "height"
            to: 0
        }
        NumberAnimation {
            target: logWin
            property: "opacity"
            to: 0
        }
    }
    ParallelAnimation {
        id: logWinShow
        NumberAnimation {
            target: logWin
            property: "x"
            to: root.width / 3 / 2
        }
        NumberAnimation {
            target: logWin
            property: "y"
            to: root.height / 3 / 2
        }
        NumberAnimation {
            target: logWin
            property: "width"
            to: root.width * 2 / 3
        }
        NumberAnimation {
            target: logWin
            property: "height"
            to: root.height * 2 / 3
        }
        NumberAnimation {
            target: logWin
            property: "opacity"
            to: 0.9
        }
    }

    Component.onCompleted: {
        timer.start()
        //btnRota.start()
    }

    Timer {
        id: timer
       interval: 150
       running: false
       repeat: true
       onTriggered: {
           switch(root.state)
           {
               case "ks0": root.state = "ks1"; return;
               case "ks1": root.state = "ks2"; return;
               case "ks2": root.state = "ks0"; return;
           }
       }
    }
}
