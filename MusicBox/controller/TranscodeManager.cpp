#include <QTime>
#include <QThread>
#include <QPair>
#include <QMap>
#include <QVariant>

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
    ConverterFactory* factory = new MusicFactory();
    Converter* convertor = Q_NULLPTR;
    //QList<Converter*> convertors(srcFiles.size());

    using MPair = QPair<int, bool>;
    QFutureSynchronizer<MPair> futures;
    for(auto iter = files.constKeyValueBegin(); iter != files.constKeyValueEnd(); iter++)
    {
        QFileInfo info(iter->second.toString());//TODO:临时测试，待修改
        if(ConverterFactory::KGMusicSuffix.contains(info.suffix()))
            convertor = factory->createConverter(ConverterFactory::MusicType::KGMusic);
        else if(ConverterFactory::NEMusicSuffix.contains(info.suffix()))
            convertor = factory->createConverter(ConverterFactory::MusicType::NEMusic);
        else
            convertor = factory->createConverter(ConverterFactory::MusicType::Undefined);
        try {
            ++successCnt;
            //convertor->Decrypt(srcFile, destFile);//TODO::多测试下是需要towstdstring还是只需tostdstring
            futures.addFuture(
                QtConcurrent::run([=] () -> MPair {
                bool f = convertor->Decrypt(iter->second.toString(), destFile);
                convertor->deleteLater();
                MPair ret(iter->first.toInt(),f);
                return ret;
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

        convertor->deleteLater();
        convertor = Q_NULLPTR;
    }
    futures.waitForFinished();
    QList<QFuture<MPair>> results = futures.futures();
    QVariantMap retMap;
    for(const auto &ret : results)
    {
        if(ret.isValid())
        {
            auto pair = ret.resultAt(0);
            retMap.insert(QString::number(pair.first),
                QVariant::fromValue(pair.second));
        }
    }
    factory->deleteLater();

    QString str = QString("success: %1 fail: %2").arg(successCnt).arg(failCnt);
    qInfo() << str;
    qDebug() << "transcode cross: " << time.msecsTo(QTime::currentTime()) << " ms...";

    emit retMsgSig(str);
    emit retMsgSig(QVariant::fromValue(retMap));
}
