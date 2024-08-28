import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform
import "../item"

Item {
    id: root

    readonly property int space: 10

    ColumnLayout {
        anchors.fill: parent
        spacing: space
        RowLayout {
            id: rowLayout
            Layout.alignment: Qt.AlignHCenter
            Layout.margins: 5
            Button {
                id:test
                text: "test"
                onClicked: {
                    progsPopup.open()
                    console.log(progsPopup.width)
                }
                visible: false
            }
            GlowButton {
                id: chooseFilesBtn
                width: root.width * 0.5
                height: 30
                text: qsTr(" 选择数据源")
                Layout.alignment: Qt.AlignCenter
                onClicked: fileDialog.open()
            }
        }

        ListView {
            id: seleListView
            spacing: 0
            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft
            highlightFollowsCurrentItem: false
            model: ListModel {
                id: selectedFilesModel
                //ListElement {path:"path",isDone:false,musicType:0}
            }
            delegate: SwipeDelegate {
                id: listDelegate
                width: seleListView.width
                height: 45
                padding: 1
                background: Rectangle{
                    color: "transparent"
                }

                contentItem: Rectangle {
                    width: listDelegate.width
                    height: listDelegate.height
                    color: "transparent"
                    //height: 25
                    RowLayout {
                        spacing: space
                        anchors.fill: parent
                        anchors.leftMargin: space
                        Item {
                            height: listDelegate.contentItem.height - space
                            width: height
                            Layout.alignment: Qt.AlignVCenter
                            Image {
                                height: parent.height
                                width: height
                                source: getIconPath(model.musicType)
                                Layout.alignment: Qt.AlignVCenter
                            }
                        }

                        RollLabel {
                            height: listDelegate.contentItem.height
                            text: model.path
                            color: model.isDone ? getClr(model.musicType) : "#aa696969"
                        }
                    }
                }
                Component {
                    id: component

                    Image {
                        height: parent.height * 0.7
                        width: height + space
                        source: "qrc:/res/svg/Cancel.svg"
                        anchors.right: listDelegate.right
                        anchors.verticalCenter: listDelegate.contentItem.verticalCenter
                        anchors.leftMargin: space
                        fillMode: Image.PreserveAspectFit
                        SwipeDelegate.onClicked: selectedFilesModel.remove(index)
                    }
                }
                swipe.right: component
                swipe.onPositionChanged: {
                    if(swipe.position === 0) swipe.rightItem.visible = false
                    else swipe.rightItem.visible = true
                }
            }
        }
        RowLayout {
            Layout.margins: 5
            Layout.alignment: Qt.AlignVCenter
            ComboBox {
                height: 30
                flat: true
                model: ["交换", "替换"]
                currentIndex: -1
                onActivated: {
                    if(currentIndex === 0)
                    {
                        switchBtn.visible = true
                        replaceItem.visible = false
                        showInfo("交换\" - \"两侧的内容（除后缀名），如A - B.mp3 => B - A.mp3\n暂不支持文件名含多个\"-\"或\".\"")
                    } else if(currentIndex === 1){
                        switchBtn.visible = false
                        replaceItem.visible = true
                        showInfo("指定字符串替换，如.flac替换为空，即A - B.kgm.flac => A - B.kgm")
                    }
                }
            }
            Item {
                Layout.fillWidth: true
            }

            GlowButton {
                id: switchBtn
                width: 60
                height: 30
                btnIcon: ""
                text: qsTr("交换")
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    if(selectedFilesModel.count < 1) return;
                    var paths = {}
                    for(var i = 0; i < selectedFilesModel.count; ++i) {
                        paths[i] = selectedFilesModel.get(i).path
                    }
                    appService.handleRenameSwitch(paths)
                }
            }

            Item {
                id: replaceItem
                visible: false
                Layout.fillWidth: true
                width: 20
                height: 30
                RowLayout {
                    anchors.fill: parent
                    TextField {
                        id: tobeRepl
                        height: 25
                        hoverEnabled: true
                        selectByMouse: true
                        //color:
                        font.pixelSize: 13
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "aliceblue"
                            radius: 3
                            border.width: 1
                            border.color: "tomato"
                        }
                    }
                    Label {
                        text: " => "
                    }
                    TextField {
                        id: repl
                        height: 25
                        hoverEnabled: true
                        selectByMouse: true
                        //color:
                        font.pixelSize: 13
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "aliceblue"
                            radius: 3
                            border.width: 1
                            border.color: "tomato"
                        }
                    }
                    GlowButton {
                        id: replBtn
                        width: 60
                        height: 30
                        btnIcon: ""
                        text: qsTr("替换")
                        onClicked: {
                            if(selectedFilesModel.count < 1) return;
                            var paths = {}
                            for(var i = 0; i < selectedFilesModel.count; ++i) {
                                paths[i] = selectedFilesModel.get(i).path
                            }
                            appService.handleRenameRepl(paths, tobeRepl.text, repl.text)
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("选择所需转换的文件/文件夹")
        fileMode: FileDialog.OpenFiles
        nameFilters: ["All files (*.*)"]
        onAccepted: {
            selectedFilesModel.clear()
            appService.getSelectedFilesSig(files);
            appService.getTargetFolderSig(decodeURIComponent(files[0]), 0)
        }
    }
    Popup {
        id: infoPopup
        padding: 10
        modal: true
        focus: true
        width: root.width * 0.6
        height: root.height * 0.1
        anchors.centerIn: root
        contentItem: Rectangle {
            width: parent.width
            height: parent.height
            radius: 10
            color: "transparent"
            Label {
                anchors.centerIn: parent
                anchors.fill: parent
                font.pixelSize: 10
                text: "conent"
                wrapMode: Text.WrapAnywhere
            }
        }
    }

    Popup {
        id: progsPopup
        padding: 0
        modal: true
        focus: true
        anchors.centerIn: root
        width: root.width * 0.6
        height: width
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        //closePolicy: Popup.NoAutoClose
        background: Rectangle
        {
            color:"transparent"
        }

        contentItem: CircleIndicator
        {
            //id: progsContent
            z: 3
        }
    }
    Connections {
        target: appService
        function onUpdateSeleFilesSig(files){
            selectedFilesModel.clear()
            for(var i in files) {
                selectedFilesModel.insert(0, {path : files[i], isDone: true, musicType: 0})
            }
        }

        function onRetMsgSig(result){
            console.log(result)
            //infoPopup.open()
            progsPopup.closePolicy = Popup.CloseOnPressOutside | Popup.CloseOnEscape
        }

        function onRetSig(retList){
            if(retList.length === 3) {
                selectedFilesModel.setProperty(retList[0], "isDone", retList[1]);
                selectedFilesModel.setProperty(retList[0], "path", retList[2]);
                selectedFilesModel.setProperty(retList[0], "musicType", 99);
                //console.log(retList[0] + retList[1] + retList[2]);
            }
        }
        function onRetProgressSig(total, succProgs, failProgs)
        {
            if(!progsPopup.opened) progsPopup.open()
            var progress = (succProgs + failProgs) * 1.0 / total
            progsPopup.contentItem.progress = progress
            progsPopup.contentItem.succProgs = succProgs
            progsPopup.contentItem.failProgs = failProgs
        }
    }

    function getIconPath(type){
        switch(type){
        case 0: return "qrc:/res/svg/Music.svg";
        case 1: return "qrc:/res/svg/KG.svg";
        case 2: return "qrc:/res/svg/NE.svg";
        case 3: return "qrc:/res/svg/QM.svg";
        default: return "qrc:/res/svg/Rename.svg";
        }
    }

    function getClr(type) {
        switch(type){
        case 0: return "#33f0f0f0";
        case 1: return "#aa66b2ff";
        case 2: return "#aaffcccc";
        case 3: return "#aa37e3be";
        default: return "#aaffffe0";
        }
    }
    function showInfo(str)
    {
        infoPopup.contentChildren[0].text = str
        infoPopup.open()
    }
}
