import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1
import "../item"

//NOTE:有两处提示Binding on contentItem is not deferred...
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
                //ListElement {path:"path",isDone:false,musicType:0}
            }
            delegate: SwipeDelegate {
                id: listDelegate
                width: seleListView.width
                height: 40
                padding: 5

                //property string bgClr
                //text: modelData
                contentItem: Rectangle {
                    id: contentItem
                    width: seleListView.width
                    //height: 25
                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Image {
                            id: icon
                            source: getIconPath(model.musicType)
                            Layout.alignment: Qt.AlignVCenter
//                            MouseArea {
//                                anchors.fill: parent
//                                onClicked: console.log("label: " + contentLab.implicitWidth + " content:" + content.width + " contentItem:" + contentItem.width)
//                            }
                        }
                        RollLabel {
                            id: rollLabel
                            height: icon.height
                            text: model.path
                            color: model.isDone ? getClr(model.musicType) : "transparent"
                        }
                        Image {
                            id: checkImage
                            source: "qrc:/res/svg/Done.svg"
                            height: icon.height / 2
                            width: height
                            Layout.alignment: Qt.AlignVCenter
                            fillMode: Image.PreserveAspectFit
                            visible: model.isDone
                        }
                    }
                }

                swipe.right: Image {
                    id: deleteLabel
                    height: icon.height / 2
                    width: icon.height
                    anchors.right: parent.right
                    source: "qrc:/res/svg/Cancel.svg"
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.PreserveAspectFit
                    SwipeDelegate.onClicked: selectedFilesModel.remove(index)
                }

                onClicked: {
                    seleListView.currentIndex = index;
                    console.log("index: " + index);
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
                selectedFilesModel.insert(0, {path : path, isDone: false, musicType: 0})
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
            selectedFilesModel.insert(0, {path : path, isDone: false, musicType: 0})
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
            height: parent.height / 7
            radius: 10
            color: "white"
            Label {
                id: lab1
                anchors.centerIn: parent
                font.bold: true
                font.pointSize: 11
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
            console.log(result)
            lab1.text = result
            infoPopup.open()
        }

        function onRetSig(retList){
            if(retList.length === 3) {
                selectedFilesModel.setProperty(retList[0], "isDone", retList[1]);
                selectedFilesModel.setProperty(retList[0], "musicType", retList[2]);
                //console.log(retList[0] + retList[1] + retList[2]);
            }
        }

    }

    function getIconPath(type){
        switch(type){
        case 0: return "qrc:/res/svg/Music.svg";
        case 1: return "qrc:/res/svg/KG.svg";
        case 2: return "qrc:/res/svg/NE.svg";
        default: return "qrc:/res/svg/Music.svg";
        }
    }

    function getClr(type) {
        switch(type){
        case 0: return "#aae0e0e0";
        case 1: return "#aa66b2ff";
        case 2: return "#aaffcccc";
        default: return "#aae0e0e0";
        }
    }
}

