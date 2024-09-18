import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import "../item"

Item {
    id: root

    readonly property int space: 10
    readonly property int baseContentH: width * 0.1
    readonly property int expandContentH: width * 0.6
    readonly property int contentH: baseContentH + expandContentH

    ColumnLayout {
        anchors.fill: parent
        spacing: space
        RowLayout {
            id: rowLayout
            Layout.alignment: Qt.AlignHCenter
            Layout.margins: 5
            GlowButton {
                id: chooseFilesBtn
                width: root.width * 0.5
                height: 30
                text: qsTr(" 选择数据源")
                Layout.alignment: Qt.AlignCenter
                onClicked: fileDialog.open()
            }
            Button {
                visible: false
                id: testBtn
                width: 50
                height: 30
                text: qsTr("test")
                onClicked: {
                    selectedFilesModel.append({path : "pathhhhhh", isDone: true, title:"title", artist:"artists", album:"album", cover:""})
                }
            }
        }

        ListView {
            id: seleListView
            clip: true
            spacing: 2

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft
            highlightFollowsCurrentItem: false
            model: ListModel {
                id: selectedFilesModel
                //ListElement {path:"path",isDone:false,title:"title",artist:"artists",album:"album",cover:"coverpath"}
            }
            delegate: SwipeDelegate {
                id: listDelegate
                width: seleListView.width
                height: baseContentH
                padding: 1
                background: Rectangle{
                    color: "transparent"
                }

                Rectangle {
                    width: listDelegate.width
                    height: baseContentH
                    anchors.fill: parent
                    color: "transparent"
                    clip: true
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0
                        anchors.bottomMargin: space
                        RowLayout {
                            anchors.leftMargin: space
                            anchors.rightMargin: space
                            Item {
                                height: baseContentH
                                width: height
                                Image {
                                    sourceSize: Qt.size(parent.width - space, parent.width - space)
                                    source: getIconPath(model.musicType)
                                    anchors.centerIn: parent
                                }
                            }

                            RollLabel {
                                height: baseContentH
                                text: model.path
                                color: model.isDone ? getClr(model.musicType) : "#aa696969"
                            }
                        }

                        Rectangle {
                            //id: infoEditor
                            width: listDelegate.width
                            height: expandContentH
                            Layout.fillWidth: true
                            //visible: false
                            color: "#77b0c4de"
                            GridLayout {
                                anchors.fill: parent
                                columnSpacing: space * 2
                                anchors.margins: 10
                                columns: 2
                                Text { text: qsTr("标题") }
                                MyTextField {
                                    height: baseContentH
                                    text: model.title
                                    //placeholderText: qsTr("输入标题")
                                    onTextEdited: model.title = text
                                }

                                Text { text: qsTr("艺人") }
                                MyTextField {
                                    height: baseContentH
                                    text: model.artist
                                    //placeholderText: qsTr("输入艺人，多个以;或/间隔")
                                    onTextEdited: model.artist = text
                                }

                                Text { text: qsTr("专辑") }
                                MyTextField {
                                    height: baseContentH
                                    text: model.album
                                    //placeholderText: qsTr("输入专辑")
                                    onTextEdited: model.album = text
                                }

                                Text { text: qsTr("封面") }
                                Rectangle {
                                    height: 50
                                    color: "transparent"
                                    //Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    RowLayout {
                                        anchors.fill: parent
                                        spacing: space * 2
                                        Rectangle {
                                            width: 1
                                            height: parent.height * 0.6
                                            color: "steelblue"
                                        }
                                        SpacerItem {}
                                        Image {
                                            sourceSize: Qt.size(50, 50)
                                            source: model.cover === "" ? "" : "file:///" + model.cover
                                        }
                                        SpacerItem {}
                                        Rectangle {
                                            width: 1
                                            height:  parent.height * 0.6
                                            color: "steelblue"
                                        }
                                        GlowButton {
                                            width: 50
                                            height: 25
                                            text: qsTr("选择")
                                            onClicked: coverSeletor.open()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                onClicked: {
                    height = height === baseContentH ? contentH : baseContentH
                }

                FileDialog {
                    id: coverSeletor
                    title: qsTr("选择所需转换的图像文件")
                    fileMode: FileDialog.OpenFile
                    nameFilters: ["*.jpg", "*.jpeg", "*.png"]
                    onAccepted: {
                        console.log(decodeURIComponent(selectedFile) + " " + index)
                        var p = appService.getRealPath(decodeURIComponent(selectedFile))    //TODO:appservice.geturl
                        selectedFilesModel.setProperty(index, "cover", p)
                    }
                }


                Component {
                    id: component

                    Image {
                        height: parent.height * 0.5
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
            Layout.alignment: Qt.AlignRight

            GlowButton {
                id: saveBtn
                width: 60
                height: 30
                btnIcon: ""
                text: qsTr("保存")
                //Layout.alignment: Qt.AlignRight
                onClicked: {
                    var infos = {}
                    for(var i = 0; i < selectedFilesModel.count; ++i)
                    {
                        var obj = selectedFilesModel.get(i);
                        console.log(obj["path"]+" "+obj["title"]+" "+obj["artist"]+" "+obj["album"]+" "+obj["cover"])

                        var info = {}
                        info["path"] = obj["path"]
                        info["title"] = obj["title"]
                        info["artist"] = obj["artist"]
                        info["album"] = obj["album"]
                        info["cover"] = obj["cover"]
                        infos[i] = info;
                    }
                    appService.editFileInfoSig(infos);
                }
            }
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

    FileDialog {
        id: fileDialog
        title: qsTr("选择所需转换的文件/文件夹")
        fileMode: FileDialog.OpenFiles
        nameFilters: ["All files (*.*)"]
        onAccepted: {
            busyPopup.open()
            selectedFilesModel.clear()
            appService.getFilesnfoSig(selectedFiles)
        }
    }

    Popup {
        id: busyPopup
        padding: 0
        modal: true
        focus: true
        anchors.centerIn: root
        width: root.width * 0.6
        height: width
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        background: Rectangle
        {
            color:"transparent"
        }

        contentItem: BusyItem
        {
            //z: 3
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
            z: 3
        }
    }

    Connections {
        target: appService
        function onRetFilesInfoSig(vars) {
            busyPopup.close()
            selectedFilesModel.clear()
            for(var i = 0; i < vars.length; ++i)
            {
                console.log(vars[i].getPath() + vars[i].getTitle() + vars[i].getArtist() + vars[i].getAlbum() + vars[i].getCoverPath())
                selectedFilesModel.append({})
                selectedFilesModel.setProperty(i, "isDone", true)
                selectedFilesModel.setProperty(i, "path", vars[i].getPath())
                selectedFilesModel.setProperty(i, "title", vars[i].getTitle())
                selectedFilesModel.setProperty(i, "artist", vars[i].getArtist())
                selectedFilesModel.setProperty(i, "album", vars[i].getAlbum())
                selectedFilesModel.setProperty(i, "cover", vars[i].getCoverPath())
            }
        }

        function onRetSig(retList){
            if(retList.length === 3) {
                selectedFilesModel.setProperty(retList[0], "isDone", retList[1]);
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
        default: return "#aa4682b4";
        }
    }
    function showInfo(str)
    {
        infoPopup.contentChildren[0].text = str
        infoPopup.open()
    }
}
