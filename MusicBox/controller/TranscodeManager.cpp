#include <QTime>
#include <QThread>
#include <QPair>
#include <QMap>
#include <QVariant>
#include <QVariantList>
#include <QSharedPointer>

#include "TranscodeManager.h"
#include "model/FunModel/convert/Converter.h"
#include "model/FunModel/convert/MusicFactory.h"
//#include "qglobal.h"
#include <QtConcurrent>
#include <QFutureSynchronizer>

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
    qDebug() << "getTargetPath: " << url << mode;
    QString path;
    QDir dir(url.toLocalFile());
    if(dir.exists())
    {
        qDebug() << 21;
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
    }
    emit retTargetFolderSig(path);
}

void TranscodeManager::handleMusicFiles(QVariant filesVar, QString destFile)
{
    QVariantMap files = filesVar.toMap();
    qDebug() << "test " << QThread::currentThreadId() << tr("待转换目标: ") << files.size();
    int successCnt = 0, failCnt = 0;
    QTime time = QTime::currentTime();
    QScopedPointer<ConverterFactory> factory(new MusicFactory());
    Converter* convertor = Q_NULLPTR;
    //QList<Converter*> convertors(srcFiles.size());

    QFutureSynchronizer<void> futures;
    for(auto iter = files.constKeyValueBegin(); iter != files.constKeyValueEnd(); iter++)
    {
        QFileInfo info(iter->second.toString());//TODO:临时测试，待修改
        QString suffix = info.suffix();
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
        try {
            ++successCnt;
            //convertor->Decrypt(srcFile, destFile);//TODO::多测试下是需要towstdstring还是只需tostdstring
            futures.addFuture(
                QtConcurrent::run([=] () {
                bool f = convertor->Decrypt(iter->second.toString(), destFile);
                convertor->deleteLater();
                QVariantList retList;    //int index; bool result; MusicType type;
                retList.reserve(3);
                retList[0] = iter->first.toInt();
                retList[1] = f;
                retList[2] = QVariant::fromValue(mType);
                emit retMsgSig(QVariant::fromValue(retList));
             }));
        }
        catch(std::exception& e){
            ++failCnt;
            qWarning() << e.what();
        }
        catch (...) {
            ++failCnt;
            qWarning() << tr("decrypt exception");
        }
    }
    futures.waitForFinished();

    QString str = QString("success: %1 fail: %2").arg(successCnt).arg(failCnt);
    qInfo() << str;
    qDebug() << "transcode cross: " << time.msecsTo(QTime::currentTime()) << " ms...";

    emit retMsgSig(str);
}
