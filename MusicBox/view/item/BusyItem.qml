import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts

Rectangle {
    id: root
    width: 150
    height: width
    color: "transparent"

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

        SequentialAnimation {
            id: ani
            loops: Animation.Infinite
            running: true
            NumberAnimation {
                target: content
                property: "progress"
                easing.type: Easing.InQuad
                to: 1
                duration: 1000
            }

            ParallelAnimation {
                PropertyAnimation {
                    target: content
                    property: "progress"
                    to: 0
                    duration: 1000
                    easing.type: Easing.InQuad
                }
                PropertyAnimation {
                    target: content
                    property: "rotation"
                    to: 360
                    duration: 1000
                    easing.type: Easing.InQuad
                }
            }
        }
    }
}

