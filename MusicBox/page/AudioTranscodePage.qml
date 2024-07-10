import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1

Item {
    //signal getTargetPathSig(string path, int mode)  //mode == 0 files ,== 1 folder
    signal transcodeSig(string srcPaths, string taregtPath)

    ColumnLayout {
        anchors.fill: parent
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
        ListView {//WARNING:item过多时，view的上下会与其它组件重合
            id: toBeDealsList

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: true
            model: ListModel {
                id: selectedFilesModel
                //ListElement {path:"path"}
            }
            delegate: SwipeDelegate {
                id: listDelegate
                width: parent.width     //TODO: 删除的时候会输出警告width为null

                text: modelData

                onClicked: {
                    toBeDealsList.currentIndex = index;
                    toBeDealsList.focus = true;
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


                ListView.onRemove: SequentialAnimation {
                    /*TODO: 有警告，暂时没搞懂什么原因
                    qt.qml.defaultmethod: Assigning an object to a signal handler is deprecated.
                    Instead, create the object, give it an id, and call the desired slot from the signal handler.*/
                    PropertyAnimation   //duration设置为0的话跟下面的Action差不多
                    {
                        target: listDelegate
                        property: "ListView.delayRemove"
                        to: true
                        duration: 0
                    }
                    NumberAnimation
                    {
                        target: listDelegate
                        easing.type: Easing.InOutQuad
                        properties: "height"
                        to: 0
                        duration: 250
                    }
                    PropertyAction
                    {
                        target: listDelegate
                        properties: "ListView.delayRemove"
                        value: false
                    }
                }
            }
        }
        RowLayout {
            Text {
                text: qsTr("生成路径：")
            }
            TextInput {
                id: targetPath
            }
        }

        Button {
            id: startBtn
            text: qsTr("转换")
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                var paths = []
                for(var i = 0; i < selectedFilesModel.count; ++i) {
                    paths.push(selectedFilesModel.get(i).path)
                }
                transcodeManager.startSig(paths, targetPath.text)
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
            for(var i in files)
            {
                var url = decodeURIComponent(files[i])
                var path = url.toString().replace("file:///", "")
                selectedFilesModel.insert(0, {path : path})
            }
            //console.log(files)
            console.log("selected " + selectedFilesModel.count + " files");

            targetPath.text = transcodeManager.getTargetPath(folder.toString(), 0)
        }
    }


    //Qt 6.3
    FolderDialog {
        id: folderDialog
        options: FolderDialog.ShowDirsOnly
        onAccepted: {
            console.log("1" + folderDialog.folder)
            selectedFilesModel.clear()
            var path = folder.toString().replace("file:///", "")
            selectedFilesModel.insert(0, {path : path})

            targetPath.text = transcodeManager.getTargetPath(path, 1)
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
            color: Qt.gray
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

    function handleResultRet(result){
        console.log("qml get: " + result)
        lab1.text = result  //TODO:看下id的作用范围
        infoPopup.open()
    }

//    Component.onCompleted: {
//        transcodeManager.resultRetSig.connect(handleResultRet)
//    }
}

