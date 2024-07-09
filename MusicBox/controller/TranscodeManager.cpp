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
    qDebug() << "test " << QThread::currentThreadId() << tr("带转换目标: ") << srcFiles.size();
    QTime time;
    ConverterFactory* factory = new MusicFactory();
    Converter* convertor = Q_NULLPTR;
    for(const auto &srcFile : srcFiles)
    {
        qDebug() << "11111" << srcFile;
        QFileInfo info(srcFile);//TODO:临时测试，待修改
        if(info.suffix() == "kgm") convertor = factory->createConverter(ConverterFactory::MusicType::KGMusic);
        else if(info.suffix() == "ncm") convertor = factory->createConverter(ConverterFactory::MusicType::NEMusic);
        else factory->createConverter(ConverterFactory::MusicType::Undefined);
        qDebug() << "2222";
        convertor->Decrypt(srcFile, destFile);//TODO::多测试下是需要towstdstring还是只需tostdstring

        qDebug() << "3333";
        convertor->deleteLater();
        qDebug() << "4444";
        convertor = Q_NULLPTR;
    }
    qDebug() << "transcode cross: " << QTime::currentTime().msecsTo(time) << " ms...";
    emit resultRetSig("resultRetSig");
}
