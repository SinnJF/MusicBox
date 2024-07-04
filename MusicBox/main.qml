import QtQuick 2.15
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import "page"   //目录结构

ApplicationWindow {
    id: mainWindow
    width: 360//screen.width
    height: 640//screen.height
    visible: true
    title: qsTr("MucisBox")

    readonly property bool inPortrait: mainWindow.width < mainWindow.height

    SwipeView {
        id: homePage
        orientation: Qt.Vertical
        anchors.fill: parent    //铺满父窗体，否则只会占一小部分
        interactive: false      //关闭手动滑动可切换页面的功能，只能从抽屉页切换
        Component.onCompleted:{
            contentItem.highlightMoveDuration = 0       //将移动时间设为0
        }

        AudioTranscodePage {    //名称对应文件名
            id: audioTranscodePage
            Component.onCompleted: console.log(width + "," + height)
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
                    anchors.left: parent.left
                    anchors.right: parent.right

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

    }

    Drawer {
        id: drawer

        width: mainWindow.width / 3
        height: mainWindow.height

        modal: inPortrait       //模态
        interactive: inPortrait
        visible: !inPortrait

        ListView {
            id: appListView
            anchors.fill: parent

            headerPositioning: ListView.OverlayHeader

            header: Pane {
                id: appHeader
                z: 1
                width: parent.width

                contentHeight: logo.height

                Image {
                    id: logo
                    horizontalAlignment: Image.AlignLeft
                    height: 50
                    width: parent.width
                    source: "images/ironman.png"
                    fillMode: implicitWidth > width ? Image.PreserveAspectFit : Image.Pad
                }

                MenuSeparator {
                    parent: appHeader
                    width: parent.width
                    anchors.verticalCenter: parent.bottom
                    visible: !appListView.atYBeginning
                }
            }

//            footer: ItemDelegate {
//                id: appFooter
//                text: qsTr("设置")
//                width: parent.width

//                MenuSeparator {
//                    parent: appFooter
//                    width: parent.width
//                    anchors.verticalCenter: parent.top
//                }
//            }

            model: ObjectModel{
                Button {
                    id: transcodeBtn
                    width: appListView.width
                    icon.source: "images/ironman.png"
                    text: "格式转换"
                    onClicked: {
                        homePage.currentIndex = 0   //todo:查下有没有解耦一点的写法或改其他组件
                    }
                }
                Button {
                    id: audioPlayBtn
                    width: appListView.width
                    icon.source: "images/ironman.png"
                    text: "音频播放"
                    onClicked: {
                        homePage.currentIndex = 1
                    }
                }
            }

//            delegate: ItemDelegate {
//                width: appListView.width
//            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
}
