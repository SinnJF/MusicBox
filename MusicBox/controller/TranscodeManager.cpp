#include <QTime>
#include <QThread>

#include "TranscodeManager.h"
#include "model/FunModel/convert/Converter.h"
#include "model/FunModel/convert/MusicFactory.h"
//#include "qglobal.h"

class KGMusicConverter;
class NEMusicConverter;

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
    qDebug() << "test " << QThread::currentThreadId() << tr("待转换目标: ") << srcFiles.size();
    int successCnt = 0, failCnt = 0;
    QTime time = QTime::currentTime();
    ConverterFactory* factory = new MusicFactory();
    Converter* convertor = Q_NULLPTR;
    for(const auto &srcFile : srcFiles)
    {
        QFileInfo info(srcFile);//TODO:临时测试，待修改
        if(ConverterFactory::KGMusicSuffix.contains(info.suffix()))
            convertor = factory->createConverter(ConverterFactory::MusicType::KGMusic);
        else if(ConverterFactory::NEMusicSuffix.contains(info.suffix()))
            convertor = factory->createConverter(ConverterFactory::MusicType::NEMusic);
        else
            convertor = factory->createConverter(ConverterFactory::MusicType::Undefined);
        try {
            ++successCnt;
            convertor->Decrypt(srcFile, destFile);//TODO::多测试下是需要towstdstring还是只需tostdstring==
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
    QString str = QString("success: %1 fail: %2").arg(successCnt).arg(failCnt);
    qInfo() << str;
    qDebug() << "transcode cross: " << time.msecsTo(QTime::currentTime()) << " ms...";
    emit resultRetSig(str);
}
