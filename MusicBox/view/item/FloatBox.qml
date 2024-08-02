import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import Qt5Compat.GraphicalEffects

Item {
    id: root
    anchors.fill: parent
    opacity: 0.6

    property real scl

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
        x: root.width / 3 / 2
        y: root.height / 3 / 2
        opacity: 0.6
        border.color: Qt.lighter("grey")

        ScrollView {
            anchors.fill: parent
            TextArea {
                id: logEdit
                wrapMode: TextEdit.WrapAnywhere
                clip: true
            }
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
        x: root.width - width / 2
        y: root.height - height / 2
        radius: width / 2
        z: 1
        background: Image {
            //id: floatIcon //NOTE:使用id会有提示
            width: parent.width
            height: parent.height
            anchors.centerIn: parent
            source: "qrc:/res/svg/Rolling.svg"
        }
        onClicked: {
            if(btnRota.running) {
                btnRota.stop()
                floatBtn.background.source = "qrc:/res/svg/Roll.svg"
                //floatIcon.source = "qrc:/res/svg/Roll.svg"
                logWinHide.stop()
                logWinShow.start()
            }
            else {
                var c = floatBtn.background.source = "qrc:/res/svg/Rolling.svg"
                //floatIcon.source = "qrc:/res/svg/Rolling.svg"
                btnRota.start()
                logWinShow.stop()
                logWinHide.start()
            }
            //logEdit.append(",，啊时代看咯杰卡斯空间卡索拉省的就卡死的你")
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
                    moveXAni.to = -1 * floatBtn.width / 2
                    moveXAni.start()
                    return
                } else if(Math.abs(root.width - floatBtn.x) < floatBtn.width * 2) {
                    moveXAni.to = root.width - floatBtn.width / 2
                    moveXAni.start()
                    return
                } else if(Math.abs(floatBtn.y) < floatBtn.height * 2) {
                    moveYAni.to = -1 * floatBtn.height / 2
                    moveYAni.start()
                } else if(Math.abs(root.width - floatBtn.x) < floatBtn.height * 2) {
                    moveYAni.to = root.height - floatBtn.height / 2
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
            to: floatBtn.x + floatBtn.width
        }
        NumberAnimation {
            target: logWin
            property: "y"
            to: floatBtn.y + floatBtn.height
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
            to: 0.8
        }
    }

}
