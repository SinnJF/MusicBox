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
    explicit TranscodeManager(QObject *parent = nullptr);
    Q_INVOKABLE QString getTargetPath(QUrl url, int mode);
    Q_INVOKABLE void test(QStringList srcFiles, QString destFile);

signals:
    void startSig(QStringList srcFiles, QString destFile);
    void resultRetSig(QString result);
};

#endif // TRANSCODEMANAGER_H
