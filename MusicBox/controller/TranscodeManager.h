#ifndef TRANSCODEMANAGER_H
#define TRANSCODEMANAGER_H

#include <QObject>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QUrl>

class TranscodeManager : public QObject
{
    Q_OBJECT
public:
    //据说C++11的静态局部变量是线程安全的，再加返回的是引用，保证返回的是同一个对象
    static TranscodeManager& getInatance(){
        static TranscodeManager instance;
        return instance;
    }

    void getTargetFolder(QUrl url, int mode);
    void handleMusicFiles(QVariant filesVar, QString destFile);

private:
    explicit TranscodeManager(QObject *parent = nullptr);
    ~TranscodeManager();

signals:
    void retTargetFolderSig(QString path);
    void retFilesSeleSig(QStringList files);
    void retMsgSig(QString result);
    void retMsgSig(QVariant results);
};

#endif // TRANSCODEMANAGER_H
