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

#include "TranscodeManager.h"
#include "model/FunModel/convert/Converter.h"
#include "model/FunModel/convert/MusicFactory.h"
//#include "qglobal.h"
#include "common/AndroidHelper.h"

#pragma execution_character_set("utf-8")

class KGMusicConverter;
class NEMusicConverter;
TranscodeManager::TranscodeManager(QObject *parent)
    : QObject{parent}
{

}

TranscodeManager::~TranscodeManager()
{

}

void TranscodeManager::getTargetFolder(QUrl url, int mode)
{
    QString localPath;
    if(QQmlFile::isLocalFile(url)) localPath = QQmlFile::urlToLocalFileOrQrc(url);
    qInfo() << "getTargetPath: " << url << mode << localPath;
    QFileInfo info(localPath);
    qInfo() << info.path();
    QString path;
    QDir dir(info.path());
    if(dir.exists())
    {
        qInfo() << "dir exists";
        switch (mode) {
        case 0:
        {
            break;
        }
        case 1:
        {
            QStringList namesFilters;
            for(const auto &n : ConverterFactory::KGMusicSuffix)
                namesFilters.append("*." + n);
            for(const auto &n : ConverterFactory::NEMusicSuffix)
                namesFilters.append("*." + n);
            QFileInfoList iList = dir.entryInfoList(namesFilters, QDir::Files);
            QStringList fList;
            for(const auto &info : iList)
                fList.append(info.absoluteFilePath());
            emit retFilesSeleSig(fList);
            dir.cdUp();
            break;
        }
        default:
            break;
        }
        dir.mkdir("MusicBox");
        dir.cd("MusicBox");
        path = dir.path() + "/";
    }
    else
    {
        path = QDir::rootPath() + "MusicBox/";
        qInfo() << "dir not exists, redirect to: " << path;
    }
    emit retTargetFolderSig(path);
}

void TranscodeManager::handleMusicFiles(QVariant filesVar, QString destFile)
{
    QVariantMap files = filesVar.toMap();
    qInfo() << "test " << QThread::currentThreadId() << tr("待转换目标: ") << files.size();
    int successCnt = 0, failCnt = 0;
    QTime time = QTime::currentTime();
    QScopedPointer<ConverterFactory> factory(new MusicFactory());
    Converter* convertor = Q_NULLPTR;

    QFutureSynchronizer<void> futures;
    for(auto iter = files.constKeyValueBegin(); iter != files.constKeyValueEnd(); iter++)
    {
        int index = iter->first.toInt();
        QString path = getRealPathFromUri(iter->second.toString());
        QString dstDir = getRealPathFromUri(destFile);

        QFileInfo info(path);    //TODO:临时测试，待修改
        qInfo() << "getRealPathFromUri: " << path << " isfile: " << info.isFile();
        QString suffix = info.suffix();
        info.setFile(dstDir);
        qInfo() << "getRealPathFromUri: " << dstDir << " isdir: " << info.isDir();

        using FMusicType = ConverterFactory::MusicType;
        FMusicType mType;
        if(ConverterFactory::KGMusicSuffix.contains(suffix))
        {
            mType = FMusicType::KGMusic;
            convertor = factory->createConverter(FMusicType::KGMusic);
        }
        else if(ConverterFactory::NEMusicSuffix.contains(suffix))
        {
            mType = FMusicType::NEMusic;
            convertor = factory->createConverter(FMusicType::NEMusic);
        }
        else
        {
            mType = FMusicType::Undefined;
            convertor = factory->createConverter(FMusicType::Undefined);
        }
        //convertor->Decrypt(srcFile, dstDir);//TODO::多测试下是需要towstdstring还是只需tostdstring
        futures.addFuture(
            QtConcurrent::run([=] () {
            bool f = convertor->Decrypt(path, dstDir);
            convertor->deleteLater();
            QVariantList retList;   //int index; bool result; MusicType type;
            retList.clear();
            retList.append(QVariant::fromValue(index));
            retList.append(QVariant::fromValue(f));
            retList.append(QVariant::fromValue(mType));
            emit retMsgSig(QVariant::fromValue(retList));
            })
        );
    }
    futures.waitForFinished();
    QString str = QString("success: %1 fail: %2").arg(successCnt).arg(failCnt); //FIXME:逻辑有问题。
    qInfo() << str;
    qInfo() << "transcode cross: " << time.msecsTo(QTime::currentTime()) << " ms...";

    emit retMsgSig(str);
}
