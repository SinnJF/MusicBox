import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import "../item"

Item {
    id: root

    readonly property int space: 10

    ColumnLayout {
        anchors.fill: parent
        spacing: space
        RowLayout {
            id: rowLayout
            //anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignHCenter   //等价上面
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
                btnIcon: "qrc:/res/svg/Done.svg"
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    fileDialog.open()
                    bottomSwipe.setCurrentIndex(0)
                }
            }
        }

        ListView {
            id: seleListView
            spacing: 0
            clip: true

            property int contentHeight: root.width * 0.1

            Layout.fillWidth: true
            Layout.fillHeight: true
            highlightFollowsCurrentItem: false
            highlight: Rectangle {
                color: "transparent"
                border.color: "steelblue";
                radius: 2;
                width: seleListView.currentItem.width;
                height: seleListView.currentItem.height
                y: seleListView.currentItem.y
            }
            model: ListModel {
                id: selectedFilesModel
                //ListElement {path:"path",isDone:false,musicType:0}
            }
            delegate: SwipeDelegate {
                id: listDelegate
                width: seleListView.width
                height: seleListView.contentHeight
                padding: 1
                background: Rectangle{
                    color: "transparent"
                }

                Rectangle {
                    width: listDelegate.width
                    height: listDelegate.height
                    color: "transparent"
                    //height: 25
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: space
                        anchors.rightMargin: space
                        Item {
                            height: seleListView.contentHeight
                            width: height
                            Image {
                                sourceSize: Qt.size(parent.height - root.space, parent.height - root.space)
                                source: getIconPath(model.musicType)
                                anchors.centerIn: parent
                            }
                        }

                        RollLabel {
                            height: seleListView.contentHeight
                            text: model.path
                            color: model.isDone ? getClr(model.musicType) : "#aa696969"
                        }
                        // Image {
                        //     id: okImg
                        //     source: "qrc:/res/svg/Done.svg"
                        //     height: icon.height / 2
                        //     width: height
                        //     Layout.alignment: Qt.AlignVCenter
                        //     fillMode: Image.PreserveAspectFit
                        //     visible: model.isDone
                        // }
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

                SequentialAnimation {
                     id: removeAnimation
                     PropertyAction { target: listDelegate; property: "ListView.delayRemove"; value: true }
                     NumberAnimation { target: listDelegate; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                     PropertyAction { target: listDelegate; property: "ListView.delayRemove"; value: false }
                }
                ListView.onRemove: removeAnimation.start()
                onClicked: {
                    seleListView.currentIndex = index
                    var src = selectedFilesModel.get(index)["path"]
                    console.log("clicked: " + src)
                    if(bottomSwipe.currentIndex === 1)
                    {
                        mediaPlayer.stop()
                        mediaPlayer.source = "file:///" + src
                        mediaPlayer.play()
                    }
                }
            }
        }

        SwipeView {
            id: bottomSwipe
            width: 40
            height: 40
            Layout.fillWidth: true
            orientation: Qt.Horizontal
            //interactive: false
            property int factor: 30
            property int margin: 5

            Item {
                width: bottomSwipe.width
                height: bottomSwipe.height
                RowLayout {
                    anchors.fill: parent
                    Layout.margins: 5
                    Item {
                        width: 30
                        height: 30
                        Image {
                            anchors.fill: parent
                            source: "qrc:/res/svg/Output.svg"
                        }
                    }

                    RollLabel {
                        id: targetPath
                        width: 20
                        height: 30
                        radius: 3
                        border.color: "lightcoral"
                    }

                    GlowButton {
                        id: genFolderBtn
                        width: 60
                        height: 30
                        text: qsTr("选择")
                        onClicked:  {
                            genFolderDialog.open()
                        }
                    }

                    GlowButton {
                        id: startBtn
                        width: 60
                        height: 30
                        text: qsTr("转换")
                        Layout.alignment: Qt.AlignCenter
                        onClicked: {
                            if(selectedFilesModel.count < 1 ) return;
                            var paths = {}
                            for(var i = 0; i < selectedFilesModel.count; ++i) {
                                paths[i] = selectedFilesModel.get(i).path
                            }
                            appService.handleMusicFiles(paths, targetPath.text)
                            bottomSwipe.setCurrentIndex(1)
                        }
                    }
                }
            }
            Item {
                width: bottomSwipe.width
                height: bottomSwipe.height
                Rectangle {
                    id: coverRect
                    width: bottomSwipe.factor
                    height: width
                    anchors.left: parent.left
                    anchors.leftMargin: bottomSwipe.margin
                    anchors.verticalCenter: parent.verticalCenter
                    color: "steelblue"
                }
                // RoundButton {
                //     id: playPauseBtn

                //     property bool playing: mediaPlayer.playbackState == mediaPlayer.PlayingState ? true : false

                //     width: swipe.factor
                //     height: width
                //     radius: width / 2
                //     flat: true

                //     icon.source: playing ? "qrc:/res/svg/Pause.svg" : "qrc:/res/svg/Play.svg"
                //     anchors.left: coverRect.right
                //     anchors.leftMargin: swipe.margin
                //     anchors.verticalCenter: parent.verticalCenter
                //     onClicked: {
                //         playing ? mediaPlayer.pause() : mediaPlayer.play()
                //     }
                // }
                Item {
                    id: playPauseBtn
                    property bool playing: mediaPlayer.playbackState === MMediaPlayer.PlayingState ? true : false
                    width: bottomSwipe.factor - 10
                    height: width
                    anchors.left: coverRect.right
                    anchors.leftMargin: bottomSwipe.margin
                    anchors.verticalCenter: parent.verticalCenter
                    Image {
                        anchors.fill: parent
                        source: playPauseBtn.playing ? "qrc:/res/svg/Pause.svg" : "qrc:/res/svg/Play.svg"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            playPauseBtn.playing ? mediaPlayer.pause() : mediaPlayer.play()
                        }
                    }
                }

                Item {
                    id: rightItem
                    width: 11
                    height: 11
                    anchors.left: playPauseBtn.right
                    anchors.leftMargin: bottomSwipe.margin
                    anchors.right: parent.right
                    anchors.rightMargin: bottomSwipe.margin
                    anchors.verticalCenter: parent.verticalCenter
                    RowLayout {
                        anchors.fill: rightItem
                        Text {
                            id: currentTime
                            width: bottomSwipe.factor
                            text: mediaPlayer.currPosText
                            horizontalAlignment: Text.AlignRight
                            Layout.alignment: Qt.AlignVCenter
                            font.pixelSize: bottomSwipe.factor / 3
                        }
                        Slider {
                            id: slider
                            property int lineWidth: 3
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            background: Rectangle {
                                x: slider.leftPadding
                                y: slider.topPadding + slider.availableHeight / 2 - height / 2
                                implicitWidth: 200
                                implicitHeight: slider.lineWidth
                                width: slider.availableWidth
                                height: implicitHeight
                                radius: slider.lineWidth
                                color: "#bdbebf"

                                Rectangle {
                                    width: slider.visualPosition * parent.width
                                    height: parent.height
                                    color: "steelblue"
                                    radius: slider.lineWidth
                                }
                            }

                            handle: Rectangle {
                                x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
                                y: slider.topPadding + slider.availableHeight / 2 - height / 2
                                implicitWidth: slider.lineWidth * 2
                                implicitHeight: slider.lineWidth * 2
                                radius: slider.lineWidth
                                color: slider.pressed ? "#f0f0f0" : "#f6f6f6"
                                border.color: "#bdbebf"
                            }

                            enabled: mediaPlayer.seekable
                            value: mediaPlayer.position / mediaPlayer.duration
                            onMoved: mediaPlayer.setPosition(value * mediaPlayer.duration)
                        }
                        Text {
                            id: remainingTime
                            width: bottomSwipe.factor
                            text: mediaPlayer.remainPosText
                            horizontalAlignment: Text.AlignLeft
                            Layout.alignment: Qt.AlignVCenter
                            font.pixelSize: bottomSwipe.factor / 3
                        }
                    }
                }
            }
        }
    }

    MMediaPlayer {
        id: mediaPlayer
        onSourceChanged: {
            console.warn(mediaPlayer.mediaStatus + " " + source)
        }
        onMetaDataChanged: {

            console.warn(metaData.stringValue(1) + metaData.stringValue(0))
            console.warn(metaData.stringValue(25))
            console.warn(metaData.keys())
            var img = metaData.value(24)
            console.warn(img)
        }

        onErrorOccurred: (err, errorString) => {
            showInfo(errorString)
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("选择所需转换的文件/文件夹")
        fileMode: FileDialog.OpenFiles
        nameFilters: ["kugou files (*.kgm *.kge *.kgm.flac *.kgtemp)", "netease files (*.ncm)", "qqmusic files (*.qmc0 *.qmc3 *.qmcflac)", "normal (*.mp3 *.flac)"]
        onAccepted: {
            selectedFilesModel.clear()
            // for(var i in files) {
            //     var url = decodeURIComponent(files[i])
            //     var path = url.toString().replace("file:///", "")
            //     selectedFilesModel.insert(0, {path : path, isDone: true, musicType: 0})
            // }
            appService.getSelectedFilesSig(selectedFiles);
            //folder属性为空
            appService.getTargetFolderSig(decodeURIComponent(selectedFiles[0]), 0)
        }
    }

    FolderDialog {
        id: genFolderDialog
        options: FolderDialog.ShowDirsOnly
        onAccepted: {
            var url = decodeURIComponent(folder)
            targetPath.text = url.toString().replace("file:///", "")
            appService.getTargetFolderSig(url.toString(), 1)
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
        anchors.centerIn: Overlay.overlay
        width: root.width * 0.6
        height: width
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        closePolicy: Popup.NoAutoClose
        background: Rectangle
        {
            color:"transparent"
        }

        contentItem: CircleIndicator
        {
            //id: progsContent
        }
    }

    Connections {
        target: appService
        function onUpdateTargetFolderSig(path) {
            targetPath.text = path
        }
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
            if(retList.length === 4) {
                selectedFilesModel.setProperty(retList[0], "isDone", retList[1])
                selectedFilesModel.setProperty(retList[0], "musicType", retList[2])
                selectedFilesModel.setProperty(retList[0], "path", retList[3])
                //console.log(retList[0] + retList[1] + retList[2]);
            } else console.warn("onRetSig param count err...")
        }
        function onRetProgressSig(total, succProgs, failProgs)
        {
            if(!progsPopup.opened) progsPopup.open()
            var progress = (succProgs + failProgs) * 1.0 / total
            //console.log("onRetProgressSig:" + total + " " + succProgs + " " + failProgs + " " + progress)
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
        default: return "qrc:/res/svg/Music.svg";
        }
    }

    function getClr(type) {
        switch(type){
        case 0: return "#33f0f0f0";
        case 1: return "#aa66b2ff";
        case 2: return "#aaffcccc";
        case 3: return "#aa37e3be";
        default: return "#aae0e0e0";
        }
    }

    function showInfo(str)
    {
        infoPopup.contentChildren[0].text = str
        infoPopup.open()
    }
}
