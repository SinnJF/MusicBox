#ifndef APPSERVICE_H
#define APPSERVICE_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QVariant>

class AppService : public QObject
{
    Q_OBJECT
public:
    explicit AppService(QObject *parent = nullptr);
    ~AppService();

private:
    QThread* transManThread = nullptr;

signals:
    void getTargetFolderSig(QUrl url, int mode);
    void updateTargetFolderSig(QString path);
    void updateSeleFilesSig(QStringList files);
    void handleMusicFiles(QVariant filesVar, QString destFile);
    void retMsgSig(QString result);
    void retSig(QVariant results);
};

#endif // APPSERVICE_H
