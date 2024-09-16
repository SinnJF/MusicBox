import QtQuick
import QtQml.Models
import QtQuick.Controls
import QtQuick.Controls.Material
import "view/item"   //目录结构
import "view/page"

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 360//screen.width
    height: 640//screen.height
    title: qsTr("MucisBox")

    readonly property bool inPortrait: mainWindow.width < mainWindow.height
    readonly property string version: "v0.6_beta"

    background: Image {
        source: "qrc:/res/bg.jpg"
    }

    property var mainPages: ["view/page/AudioConvertPage.qml", "view/page/RenamePage.qml", "view/page/InfoEditPage.qml"]

    Loader {
        id: mainPageLoader
        width: mainWindow.width
        height: mainWindow.height
        source: "view/page/AudioConvertPage.qml"
    }

    /*SwipeView {
        id: mainPage
        //orientation: Qt.Vertical
        //anchors.fill: parent
        width: mainWindow.width
        height: mainWindow.height
        interactive: false      //关闭手动滑动可切换页面的功能，只能从抽屉页切换
        Component.onCompleted:{
            contentItem.highlightMoveDuration = 0       //将移动时间设为0
        }

        AudioConvertPage {
            id: audioConvertPage
        }

        Item {
            id: secondPage
            Rectangle {
                color: "grey"
                width: mainWindow.width
                height: mainWindow.height
            }
        }
        Item {
            id: thirdPage
            Flickable {
                id: flickable

                width: mainWindow.width
                height: mainWindow.height
                //contentHeight: column.height

                Column {
                    id: column
                    spacing: 20
                    anchors.margins: 20

                    Label {
                        font.pixelSize: 22
                        width: parent.width
                        elide: Label.ElideRight
                        horizontalAlignment: Qt.AlignHCenter
                        text: qsTr("Side Panel Example")
                    }
                }

                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
        RenamePage {
            id: renamePage
        }

    }*/

    Drawer {
        id: drawer

        width: mainWindow.width * 0.4
        height: mainWindow.height

        modal: inPortrait       //模态
        interactive: inPortrait
        visible: !inPortrait

        ListView {
            id: appListView
            spacing: 10
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: logSwitch.top
            anchors.leftMargin: spacing
            anchors.rightMargin: spacing
            // BorderImage {
            //     id: name
            //     source: "qrc:/res/png/k0.png"
            //     anchors.fill: parent
            //     border.left: 5; border.top: 5
            //     border.right: 5; border.bottom: 5
            // }

            //headerPositioning: ListView.OverlayHeader

            header: Pane {
                id: appHeader
                z: 1
                width: parent.width

                contentHeight: logo.height

                Image {
                    id: logo

                    height: 50
                    width: height
                    source: "qrc:/res/ironman.png"
                    fillMode: Image.Stretch
                    anchors.centerIn: parent
                }

                MenuSeparator {
                    parent: appHeader
                    width: parent.width
                    anchors.verticalCenter: parent.bottom
                    visible: !appListView.atYBeginning
                }
            }

           // footer: ItemDelegate {
           //  }

           MenuSeparator {
               width: parent.width - parent.spacing
               anchors.bottom: parent.bottom
               anchors.topMargin: 10
               anchors.horizontalCenter: parent.horizontalCenter
            }

            model: ObjectModel{
                GlowButton {
                    id: transcodeBtn
                    width: appListView.width
                    btnIcon: "qrc:/res/svg/Convert.svg"
                    text: " 格式转换"
                    onClicked: {
                        //mainPage.currentIndex = 0
                        mainPageLoader.source = mainPages[0]
                    }
                }
                // GlowButton {
                //     id: audioPlayBtn
                //     width: appListView.width
                //     btnIcon: "qrc:/res/svg/Play.svg"
                //     text: " 音频播放"
                //     onClicked: {
                //         //mainPage.currentIndex = 1
                //         mainPageLoader.source = mainPages[1]
                //     }
                // }
                GlowButton {
                    id: renameBtn
                    width: appListView.width
                    btnIcon: "qrc:/res/svg/Rename.svg"
                    text: " 批量命名"
                    onClicked: {
                        //mainPage.currentIndex = 3
                        mainPageLoader.source = mainPages[1]
                    }
                }
                GlowButton {
                    id: infoBtn
                    width: appListView.width
                    btnIcon: "qrc:/res/svg/Info.svg"
                    text: " 信息更改"
                    onClicked: {
                        //mainPage.currentIndex = 1
                        mainPageLoader.source = mainPages[2]
                    }
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
        Switch {
            id: logSwitch
            text: qsTr("日志浮窗")
            width: parent.width
            height: 30
            anchors.bottom: verLabel.top
            anchors.horizontalCenter: verLabel.horizontalCenter
            onCheckedChanged: {
                logLoader.sourceComponent = checked ? logComponent : undefined
            }
        }
        Label {
            id: verLabel
            padding: 10
            width: parent.width
            height: 30
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: version
            color: "lightgray"
            horizontalAlignment: Text.AlignHCenter
        }
    }
    Component {
        id: logComponent
        FloatBox {
            id: floatBox
            z: 1
            anchors.fill: parent

            Connections {
                target: log
                function onLog(msg){
                    floatBox.addLog(msg)
                }
            }
        }
    }

    Loader {
        id: logLoader
        anchors.fill: parent
    }

    //BUG: 手拉尺寸时页面会错位。Popup得手写，用锚来绑定。
    onInPortraitChanged: {
        if(inPortrait){
            mainPageLoader.width = mainWindow.width
            mainPageLoader.x = 0
            drawer.visible = false
        } else {
            mainPageLoader.width = mainWindow.width - drawer.width
            mainPageLoader.x = drawer.width
            drawer.visible = true
        }
    }
}
