#include "AppService.h"
#include "TranscodeManager.h"

#include <QThread>

AppService::AppService(QObject *parent)
    : QObject{parent}
{
    transManThread = new QThread();
    TranscodeManager::getInatance().moveToThread(transManThread);

    connect(this, &AppService::getTargetFolderSig, &TranscodeManager::getInatance(), &TranscodeManager::getTargetFolder);
    connect(&TranscodeManager::getInatance(), &TranscodeManager::retTargetFolderSig, this, &AppService::updateTargetFolderSig);
    connect(&TranscodeManager::getInatance(), &TranscodeManager::retFilesSeleSig, this, &AppService::updateSeleFilesSig);
    connect(this, &AppService::handleMusicFiles, &TranscodeManager::getInatance(), &TranscodeManager::handleMusicFiles);
    connect(&TranscodeManager::getInatance(), static_cast<void(TranscodeManager::*)(QString)>(&TranscodeManager::retMsgSig),
            this, &AppService::retMsgSig);
    connect(&TranscodeManager::getInatance(), static_cast<void(TranscodeManager::*)(QVariant)>(&TranscodeManager::retMsgSig),
            this, &AppService::retSig);

    transManThread->start();
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
