﻿import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    id: root

    property alias text : contentLab.text
    //property alias bgClr: bgRect.color

    Layout.fillWidth: true
    Layout.fillHeight: true
    clip: true

    Rectangle {
        id: leftMask
        height: root.height
        width: height
        anchors.left: parent.left
        rotation: -90
        opacity: 0
        z: 1
        gradient: Gradient {
            GradientStop { position: 0; color: "#aaffffff" }
            GradientStop { position: 1; color: "#00ffffff" }
        }
    }
    Label {
        id:contentLab
        //height: 25
        //color: "black"
        //verticalAlignment: Text.AlignVCenter
        //horizontalAlignment: Text.AlignLeft
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: 12
        anchors.left: parent.left
        anchors.right: parent.right
        text: ""
        onTextChanged: roll()
    }

    Rectangle {
        id: rightMask
        height: root.height
        width: height
        anchors.right: parent.right
        rotation: 90
        opacity: 1
        z: 1
        gradient: Gradient {
            GradientStop { position: 0; color: "#aaffffff" }
            GradientStop { position: 1; color: "#00ffffff" }
        }
    }

    SequentialAnimation {
        id: sa
        running: false
        loops: Animation.Infinite
        ParallelAnimation{
            XAnimator {
                target: contentLab
                from: 0
                to: (root.width - contentLab.implicitWidth)
                duration: 2000
            }
            OpacityAnimator {
                target: leftMask
                from: 0
                to: 1
                duration: 2000
            }
        }
        OpacityAnimator {
            target: rightMask
            from: 1
            to: 0
            duration: 2000
        }
        ParallelAnimation {
            XAnimator {
                target: contentLab
                from: (root.width - contentLab.implicitWidth)
                to: 0
                duration: 2000
            }
            OpacityAnimator {
                target: rightMask
                from: 0
                to: 1
                duration: 2000
            }
        }
        OpacityAnimator {
            target: leftMask
            from: 1
            to: 0
            duration: 2000
        }
    }

    onWidthChanged: {
        roll()
    }

    function roll() {
        if(sa.running) sa.stop()
        if(contentLab.implicitWidth - root.width > 0) sa.start()
    }
}
