#include "AppService.h"
#include "TranscodeManager.h"
#include "common/AndroidHelper.h"
#include "InfoEditManager.h"

#include <QThread>
#include <QQmlFile>
#include <QMutex>

QMutex appMtx;
AppService* AppService::instance = nullptr;
AppService* AppService::getInstance()
{
    if(!instance){
        QMutexLocker lock(&appMtx);
        if(!instance)
        {
            instance = new AppService();
            static CgarRes gar;
        }
    }
    return instance;
}

AppService::AppService(QObject *parent)
    : QObject{parent}
{
    //------------------TransManager----------------
    transManThread = new QThread();
    TranscodeManager::getInstance().moveToThread(transManThread);

    connect(this, &AppService::getSelectedFilesSig, &TranscodeManager::getInstance(), &TranscodeManager::getSelectedFiles);
    connect(this, &AppService::getTargetFolderSig, &TranscodeManager::getInstance(), &TranscodeManager::getTargetFolder);
    connect(&TranscodeManager::getInstance(), &TranscodeManager::retTargetFolderSig, this, &AppService::updateTargetFolderSig);
    connect(&TranscodeManager::getInstance(), &TranscodeManager::retFilesSeleSig, this, &AppService::updateSeleFilesSig);
    connect(this, &AppService::handleMusicFiles, &TranscodeManager::getInstance(), &TranscodeManager::handleMusicFiles);
    connect(&TranscodeManager::getInstance(), static_cast<void(TranscodeManager::*)(QString)>(&TranscodeManager::retMsgSig),
            this, &AppService::retMsgSig);
    connect(&TranscodeManager::getInstance(), static_cast<void(TranscodeManager::*)(QVariant)>(&TranscodeManager::retMsgSig),
            this, &AppService::retSig);
    connect(&TranscodeManager::getInstance(), &TranscodeManager::retProgressSig, this, &AppService::retProgressSig);

    transManThread->start();

    //-----------------InfoEditManager----------------
    infoEditThread = new QThread();
    InfoEditManager::getInstance().moveToThread(infoEditThread);

    connect(this, &AppService::getFilesnfoSig, &InfoEditManager::getInstance(), &InfoEditManager::getSrcInfo);
    connect(&InfoEditManager::getInstance(), &InfoEditManager::retSrcInfo, this, &AppService::retFilesInfoSig);
    connect(this, &AppService::editFileInfoSig, &InfoEditManager::getInstance(), &InfoEditManager::editInfo);
    connect(&InfoEditManager::getInstance(), &InfoEditManager::retSig, this, &AppService::retSig);
    connect(&InfoEditManager::getInstance(), &InfoEditManager::retProgressSig, this, &AppService::retProgressSig);

    infoEditThread->start();
}

AppService::~AppService()
{
    if(transManThread)
    {
        transManThread->quit();
        transManThread->wait();
        transManThread->deleteLater();
        transManThread = nullptr;
    }
}

//Info: 暂不支持名字中含多个"." "-"的交换
void AppService::handleRenameSwitch(QVariant vars)
{
    int succ = 0,fail = 0, total = 0;
    QVariantMap files = vars.toMap();
    total = files.size();
    QFileInfo info;
    for(auto iter = files.constKeyValueBegin(); iter != files.constKeyValueEnd(); iter++)
    {
        info.setFile(iter->second.toString());
        if(info.isFile())
        {
            QString dir = info.absolutePath();
            if(!dir.endsWith("/")) dir += "/";
            QString basename = info.baseName();
            QString suffix = info.suffix();
            QStringList parts = basename.split("-");
            if(parts.size() != 2) continue;
            QString newName = QString("%1%2 - %3.%4")
                                  .arg(dir)
                                  .arg(parts[1].trimmed())
                                  .arg(parts[0].trimmed())
                                  .arg(suffix);

            bool r = QFile::rename(iter->second.toString(), newName);
            r ? succ++ : fail++;
            QVariantList retList;   //int index; bool result; MusicType type; QString path;
            retList.clear();
            retList.append(QVariant::fromValue(iter->first.toInt()));
            retList.append(QVariant::fromValue(r));
            retList.append(QVariant::fromValue(newName));
            emit retSig(QVariant::fromValue(retList));
            qInfo() << QString("%1->%2: %3").arg(iter->second.toString()).arg(newName).arg(r);
        }
        else qWarning() << "检查该路径：" << iter->second.toString();
    }
    emit retProgressSig(total, succ, fail);
}

void AppService::handleRenameRepl(QVariant vars, QString tobeRepl, QString repl)
{
    int succ = 0,fail = 0, total = 0;
    QVariantMap files = vars.toMap();
    total = files.size();
    QFileInfo info;
    for(auto iter = files.constKeyValueBegin(); iter != files.constKeyValueEnd(); iter++)
    {
        info.setFile(iter->second.toString());
        if(info.isFile())
        {
            QString dir = info.absolutePath();
            if(!dir.endsWith("/")) dir += "/";
            QString completeName = info.fileName();
            completeName.replace(tobeRepl, repl);
            QString newName = QString("%1%2").arg(dir).arg(completeName);

            bool r = QFile::rename(iter->second.toString(), newName);
            r ? succ++ : fail++;
            QVariantList retList;   //int index; bool result; MusicType type; QString path;
            retList.clear();
            retList.append(QVariant::fromValue(iter->first.toInt()));
            retList.append(QVariant::fromValue(r));
            retList.append(QVariant::fromValue(newName));
            emit retSig(QVariant::fromValue(retList));
            qInfo() << QString("%1->%2: %3").arg(iter->second.toString()).arg(newName).arg(r);
        }
        else qWarning() << "检查该路径：" << iter->second.toString();
    }
    emit retProgressSig(total, succ, fail);
}

void AppService::handleAddTail(QVariant vars, QString tail)
{
    int succ = 0,fail = 0, total = 0;
    QVariantMap files = vars.toMap();
    total = files.size();
    QFileInfo info;
    for(auto iter = files.constKeyValueBegin(); iter != files.constKeyValueEnd(); iter++)
    {
        info.setFile(iter->second.toString());
        if(info.isFile())
        {
            bool r = QFile::rename(iter->second.toString(), iter->second.toString() + tail);
            r ? succ++ : fail++;
            QVariantList retList;   //int index; bool result; MusicType type; QString path;
            retList.clear();
            retList.append(QVariant::fromValue(iter->first.toInt()));
            retList.append(QVariant::fromValue(r));
            retList.append(QVariant::fromValue(iter->second.toString() + tail));
            emit retSig(QVariant::fromValue(retList));
            qInfo() << QString("%1->%2: %3").arg(iter->second.toString())
                           .arg(iter->second.toString() + tail).arg(r);
        }
        else qWarning() << "检查该路径：" << iter->second.toString();
    }
    emit retProgressSig(total, succ, fail);
}

QString AppService::getRealPath(QVariant var)
{
    QString ret;
    QUrl url = var.toUrl();
#ifdef Q_OS_ANDROID
    ret = getRealPathFromUri(url);
#else
    ret = QQmlFile::urlToLocalFileOrQrc(url);
#endif
    return ret;
}

QStringList AppService::getRealPaths(QList<QUrl> vars)
{
    QStringList paths;
    for(auto& url : vars)
    {
#ifdef Q_OS_ANDROID
        paths.append(getRealPathFromUri(url));
#else
        paths.append(QQmlFile::urlToLocalFileOrQrc(url));
#endif
    }
    return paths;
}
