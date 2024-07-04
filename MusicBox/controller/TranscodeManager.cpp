#include <QTime>
#include <QThread>

#include "TranscodeManager.h"
#include "model/FunModel/transcode/KGMusicConverter.h"
#include "qglobal.h"

TranscodeManager::TranscodeManager(QObject *parent)
    : QObject{parent}
{

}

QString TranscodeManager::getTargetPath(QUrl url, int mode)
{
    qDebug() << "getTargetPath: " << url << mode;
    QString path;
    QDir dir(url.toLocalFile());
    if(dir.exists())
    {
        switch (mode) {
        case 0:
        {
            break;
        }
        case 1:
        {
            dir.cdUp();
            break;
        }
        default:
            break;
        }
        path = dir.path() + "/MusicBox/";
    }
    else
    {
        path = QDir::rootPath() + "MusicBox/";
    }
    return path;
}

void TranscodeManager::test(QStringList srcFiles, QString destFile)
{
    qDebug() << "test " << QThread::currentThreadId() << tr("带转换目标: ") << srcFiles.size();
    QTime time;
    KGMusicConverter convertor;
    for(const auto &srcFile : srcFiles)
    {
        convertor.Decrypt(srcFile.toStdWString(), destFile.toStdWString());
    }
    qDebug() << "transcode cross: " << QTime::currentTime().msecsTo(time) << " ms...";
    emit resultRetSig("resultRetSig");
}
