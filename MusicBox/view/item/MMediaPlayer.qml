import QtQuick
import QtMultimedia

MediaPlayer {
    id: mediaPlayer

    property alias currPosText: mediaPlayer.mCurrPosText
    property alias  remainPosText: mediaPlayer.mRemainPosText

    property var mCurrPosText: formatToMinutes(mediaPlayer.position)
    property var mRemainPosText: formatToMinutes(mediaPlayer.duration - mediaPlayer.position)

    audioOutput: AudioOutput {
        id: audio
        //muted: playbackController.muted
        //volume: playbackController.volume
    }
    //! [6]
    source: ""

    function formatToMinutes(milliseconds) {
        const min = Math.floor(milliseconds / 60000)
        const sec = ((milliseconds - min * 60000) / 1000).toFixed(1)
        return `${min}:${sec.padStart(4, 0)}`
    }
}
