import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        RowLayout {
            id: rowLayout
            //anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignHCenter   //等价上面
            CheckBox {
                id: fileCheckBox
                text: qsTr("文件")
                checked: true
                checkable: false
                onCheckedChanged:{
                    if(checked) {
                        checkable = false;
                        folderCheckBox.checked = false;
                        folderCheckBox.checkable = true;
                    }
                }
            }
            CheckBox {
                id: folderCheckBox
                text: qsTr("文件夹")
                checked: false
                checkable: true
                onCheckedChanged: {
                    if(checked) {
                        checkable = false;
                        fileCheckBox.checked = false;
                        fileCheckBox.checkable = true;
                    }
                }
            }
            Button {
                id: chooseFilesBtn
                text: qsTr("选择数据源")
                Layout.alignment: Qt.AlignCenter    //布局下每个元素都得这样设置，有点反人类，待理解
                onClicked: {
                    if(fileCheckBox.checked) {
                        fileDialog.open()
                    }
                    else if(folderCheckBox.checked) {
                        folderDialog.open()
                    }
                }
            }
        }


        ListView {
            id: seleListView
            z: -1
            spacing: 5
            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft
            highlightFollowsCurrentItem: false
//            highlight: Rectangle {
//                color: "lightsteelblue";
//                radius: 5;
//                width: seleListView.width;
//                height: 35
//                y: seleListView.currentItem.y
//            }
//            focus: true
            model: ListModel {
                id: selectedFilesModel
                //ListElement {path:"path",isDone:false}
            }
            delegate: SwipeDelegate {
                id: listDelegate
                width: seleListView.width
                height: 30
                padding: 5

                //property bool isCurr: ListView.isCurrentItem
                //text: modelData
                contentItem: Rectangle {
                    id: contentRect
                    width: seleListView.width
                    //height: 25
                    color: model.isDone ? "#aaf9a9ff" : "transparent"
                    clip: true
                    Rectangle {
                        id: leftMask
                        height: parent.height
                        width: height
                        anchors.left: parent.left
                        rotation: -90
                        opacity: 0
                        z: 1
                        gradient: Gradient {
                            GradientStop { position: 0; color: "#dfe4ea" }
                            GradientStop { position: 1; color: "#00dfe4ea" }
                        }
                    }
                    Label {
                        id:contentLab
                        height: 25
                        color: "black"
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        text: model.path
                    }
                    Image {
                        //anchors.left: contentLab.right
                        anchors.right: contentRect.right
                        id: checkImage
                        source: "qrc:/images/Check.png"
                        height: sourceSize.height
                        width: sourceSize.width
                        anchors.verticalCenter: parent.verticalCenter
                        //fillMode: Image.PreserveAspectFit
                        visible: model.isDone
                    }
                    Rectangle {
                        id: rightMask
                        height: parent.height
                        width: height
                        anchors.right: parent.right
                        rotation: 90
                        opacity: 1
                        z: 1
                        gradient: Gradient {
                            GradientStop { position: 0; color: "#dfe4ea" }
                            GradientStop { position: 1; color: "#00dfe4ea" }
                        }
                    }
                    SequentialAnimation {
                        running: contentLab.implicitWidth > contentItem.width
                        loops: Animation.Infinite
                        property int dr: 2000
                        PauseAnimation {
                            duration: 1000
                        }
                        ParallelAnimation{
                            XAnimator {
                                target: contentLab
                                from: 0
                                to: contentItem.width - contentLab.implicitWidth
                                duration: 2000
                            }
                            OpacityAnimator {
                                target: leftMask
                                from: 0
                                to: 1
                                duration: 2000
                            }
                            OpacityAnimator {
                                target: rightMask
                                from: 1
                                to: 0
                                duration: 2000
                            }
                        }
                        PauseAnimation {
                            duration: 1000
                        }
                        ParallelAnimation {
                            XAnimator {
                                target: contentLab
                                from: contentItem.width - contentLab.implicitWidth
                                to: x
                                duration: 2000
                            }
                            OpacityAnimator {
                                target: leftMask
                                from: 1
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
                    }
                }

                onClicked: {
                    seleListView.currentIndex = index;
                    console.log("index: " + index);
                }

                swipe.right: Label {
                    id: deleteLabel
                    height: parent.height
                    anchors.right: parent.right
                    text: qsTr("删除")
                    color: "white"
                    verticalAlignment: Label.AlignVCenter
                    SwipeDelegate.onClicked: selectedFilesModel.remove(index)
                    background: Rectangle {
                        color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("#ff0000", 1.1) : "#ff0000"
                    }
                }

                SequentialAnimation {
                     id: removeAnimation
                     PropertyAction { target: listDelegate; property: "ListView.delayRemove"; value: true }
                     NumberAnimation { target: listDelegate; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                     PropertyAction { target: listDelegate; property: "ListView.delayRemove"; value: false }
                }
                ListView.onRemove: removeAnimation.start()
            }
        }
        RowLayout {
            Text {
                id: generPathText
                text: qsTr("生成路径：")
            }
            TextInput {
                id: targetPath
                Layout.fillWidth: true
            }
            Button {
                id: startBtn
                text: qsTr("转换")
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    var paths = {}
                    for(var i = 0; i < selectedFilesModel.count; ++i) {
                        paths[i] = selectedFilesModel.get(i).path
                    }
                    appService.handleMusicFiles(paths, targetPath.text)
                }

            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("选择所需转换的文件/文件夹")
        fileMode: FileDialog.OpenFiles
        nameFilters: ["kugou files (*.kgm *.kge *.flac *.kgtemp)", "netease files (*.ncm)"]
        onAccepted: {
            selectedFilesModel.clear()             //清空原来的元素
            for(var i in files) {
                var url = decodeURIComponent(files[i])
                var path = url.toString().replace("file:///", "")
                selectedFilesModel.insert(0, {path : path, isDone: false})
            }
            //console.log(files)
            console.log("selected " + selectedFilesModel.count + " files");
            appService.getTargetFolderSig(folder.toString(), 0)
        }
    }


    //Qt 6.3
    FolderDialog {
        id: folderDialog
        options: FolderDialog.ShowDirsOnly
        onAccepted: {
            selectedFilesModel.clear()
            var url = decodeURIComponent(folder)
            var path = url.toString().replace("file:///", "")
            selectedFilesModel.insert(0, {path : path, isDone: false})
            appService.getTargetFolderSig(folder, 1)
        }
    }

    Popup {
        id: infoPopup
        padding: 10
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
        contentItem: Rectangle {
            id: popupContent
            width: parent.width / 5 * 3
            height: parent.height / 5
            color: "#b0c0ac"
            Label {
                id: lab1
                anchors.centerIn: parent
                font.bold: true
                font.pointSize: 20
                text: "conent"
            }
            //verticalAlignment: Text.AlignVCenter
        }
    }

    Connections {
        target: appService
        function onUpdateTargetFolderSig(path) {
            targetPath.text = path
        }
        function onUpdateSeleFilesSig(files){
            selectedFilesModel.clear()             //清空原来的元素
            for(var i in files) {
                selectedFilesModel.insert(0, {path : files[i]})
            }
        }

        function onRetMsgSig(result){
            console.log("qml get: " + result)
            lab1.text = result
            infoPopup.open()
        }

        function onRetSig(retMap){
            for(var ret in retMap)
            {
                selectedFilesModel.setProperty(ret, "isDone", retMap[ret]);
            }
        }
    }
}

