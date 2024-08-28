#include <QTime>
#include <QThread>
#include <QPair>
#include <QMap>
#include <QVariant>
#include <QVariantList>
#include <QSharedPointer>
#include <QQmlFile>
#include <QtConcurrent>
#include <QFutureSynchronizer>
#include <QMutexLocker>

#include "TranscodeManager.h"
#include "model/FunModel/convert/Converter.h"
#include "model/FunModel/convert/MusicFactory.h"
//#include "qglobal.h"
#include "common/AndroidHelper.h"

#ifdef Q_OS_WIN
#pragma execution_character_set("utf-8")
#endif

class KGMusicConverter;
class NEMusicConverter;
class QQMusicConverter;
TranscodeManager::TranscodeManager(QObject *parent)
    : QObject{parent}
{
    //connect(this, &TranscodeManager::decryptOneSig, this, &TranscodeManager::collectResults);
}

TranscodeManager::~TranscodeManager()
{

}

// void TranscodeManager::collectResults(bool f)
// {
//     QMutex mtx;
//     QMutexLocker locker(&mtx);
//     f ? successCnt++ : failCnt++;
//     double p = (successCnt + failCnt) * 1.0 / total;
//     emit retProgressSig(p, successCnt, failCnt);
//     if(successCnt + failCnt == total)
//     {
//         QString str = "finished...";
//         emit retMsgSig(str);
//     }
// }

void TranscodeManager::setProgressZero()
{
    total = 0;
    successCnt = 0;
    failCnt = 0;
}

void TranscodeManager::getSelectedFiles(QList<QUrl> urls)
{
    QStringList paths;
    for(auto& url : urls)
    {
#ifdef Q_OS_ANDROID
        paths.append(getRealPathFromUri(url));
#else
        paths.append(QQmlFile::urlToLocalFileOrQrc(url));
#endif
    }
    emit retFilesSeleSig(paths);
}

//url是文件时则返回指定路径，传入是文件夹时则返回用户所选。
void TranscodeManager::getTargetFolder(QUrl url, int mode)
{
    Q_UNUSED(mode)
    qDebug() << "getTargetFolder from: " << url;
    QString p = QDir::rootPath();
    QString localPath;
    QFileInfo info;
#ifdef Q_OS_ANDROID
    localPath = getRealPathFromUri(url);
    p += "sdcard/MusicBox/";
#else
    localPath = url.toLocalFile();
    p += "MusicBox/";
#endif
    qDebug() << localPath;
    info.setFile(localPath);
    if(info.isDir())
    {
        emit retTargetFolderSig(localPath);
    }
    else if(info.isFile())
    {
        QDir d;
        d.mkdir(p);
        emit retTargetFolderSig(p);
    }

    return;


//     QString localPath;
// #ifdef Q_OS_ANDROID
//     localPath = getRealPathFromUri(url);
// #else
//     if(QQmlFile::isLocalFile(url)) localPath = QQmlFile::urlToLocalFileOrQrc(url);
// #endif
//     qInfo() << "getTargetPath: " << url << mode << localPath;
//     QFileInfo info(localPath);
//     qInfo() << info.path();
//     QString path;
//     QDir dir(info.path());
//     if(dir.exists())
//     {
//         qInfo() << "dir exists";
//         switch (mode) {
//         case 0:
//         {
//             break;
//         }
//         case 1:
//         {
//             QStringList namesFilters;
//             for(const auto &n : ConverterFactory::KGMusicSuffix)
//                 namesFilters.append("*." + n);
//             for(const auto &n : ConverterFactory::NEMusicSuffix)
//                 namesFilters.append("*." + n);
//             QFileInfoList iList = dir.entryInfoList(namesFilters, QDir::Files);
//             QStringList fList;
//             for(const auto &info : iList)
//                 fList.append(info.absoluteFilePath());
//             emit retFilesSeleSig(fList);
//             dir.cdUp();
//             break;
//         }
//         default:
//             break;
//         }
//         dir.mkdir("MusicBox");
//         dir.cd("MusicBox");
//         path = dir.path() + "/";
//     }
//     else
//     {
// #ifdef Q_OS_ANDROID
//         path = "/sdcard/MusicBox/";
// #else
//         path = QDir::rootPath() + "MusicBox/";
// #endif
//         qWarning() << "dir not exists, redirect to: " << path;
//     }
//     emit retTargetFolderSig(path);
}

void TranscodeManager::handleMusicFiles(QVariant filesVar, QString destFile)
{
    setProgressZero();
    QVariantMap files = filesVar.toMap();
    total = files.size();
    qInfo() << tr("待转换目标: ") << total;
    int successCnt = 0, failCnt = 0;
    //QTime time = QTime::currentTime();
    QScopedPointer<ConverterFactory> factory(new MusicFactory());
    Converter* convertor = Q_NULLPTR;

    QFutureSynchronizer<void> futures;
    for(auto iter = files.constKeyValueBegin(); iter != files.constKeyValueEnd(); iter++)
    {
        int index = iter->first.toInt();
        QString path;
        QString dstDir;
#ifdef Q_OS_ANDROID
        path = getRealPathFromUri(iter->second.toString());
        dstDir = getRealPathFromUri(destFile);
#else
        path = iter->second.toString();
        dstDir = destFile;
#endif

        QFileInfo info(path);
        QString suffix = info.suffix();
        QString completeSuf = info.completeSuffix();
        qInfo() << "path: " << path
                << " isfile: " << info.isFile() << suffix << completeSuf;
        info.setFile(dstDir);
        qInfo() << "dir: " << dstDir << " isdir: " << info.isDir();

        using FMusicType = ConverterFactory::MusicType;
        FMusicType mType;
        if(ConverterFactory::KGMusicSuffix.contains(suffix) ||
            ConverterFactory::KGMusicSuffix.contains(completeSuf))
        {
            mType = FMusicType::KGMusic;
            convertor = factory->createConverter(FMusicType::KGMusic);
        }
        else if(ConverterFactory::NEMusicSuffix.contains(suffix))
        {
            mType = FMusicType::NEMusic;
            convertor = factory->createConverter(FMusicType::NEMusic);
        }
        else if(ConverterFactory::QQMusicSuffix.contains(suffix))
        {
            mType = FMusicType::QQMusic;
            convertor = factory->createConverter(FMusicType::QQMusic);
        }
        else
        {
            mType = FMusicType::Undefined;
            convertor = factory->createConverter(FMusicType::Undefined);
        }
        QMutex mtx;
        futures.addFuture(
            QtConcurrent::run([=, &successCnt, &failCnt, &mtx] () {
                QString dstFile;
                bool f = convertor->Decrypt(path, dstDir, dstFile);
                {
                    QMutexLocker locker(&mtx);
                    f ? successCnt++ : failCnt++;
                }
                convertor->deleteLater();
                QThread::msleep(1000);
                QVariantList retList;   //int index; bool result; MusicType type; QString path;
                retList.clear();
                retList.append(QVariant::fromValue(index));
                retList.append(QVariant::fromValue(f));
                retList.append(QVariant::fromValue(mType));
                retList.append(QVariant::fromValue(dstFile));
                emit retProgressSig(total, successCnt, failCnt);
                emit retMsgSig(QVariant::fromValue(retList));
            })
        );
    }
    futures.waitForFinished();
    //qInfo() << "transcode cross: " << time.msecsTo(QTime::currentTime()) << " ms...";

    QString str = "finished...";
    emit retMsgSig(str);
}
