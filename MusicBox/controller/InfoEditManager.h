#ifndef INFOEDITMANAGER_H
#define INFOEDITMANAGER_H

#include <QObject>
#include <QVariant>

class InfoEditManager : public QObject
{
    Q_OBJECT
public:
    static InfoEditManager& getInstance()
    {
        static InfoEditManager manager;
        return manager;
    }

    void getSrcInfo(QList<QUrl> vars);
    void editInfo(QVariant vars);

private:
    explicit InfoEditManager(QObject *parent = nullptr);
    ~InfoEditManager();

    const QByteArray flacHeader = "fLaC";
    const QByteArray ID3Header = "ID3";

    QString coverTmpDir;
    void createTmpDir();
    void clearTmpFiles();

    bool editMpegInfo(QString filePath, QString title, QString artist, QString album, QString coverPath);
    bool editFlacInfo(QString filePath, QString title, QString artist, QString album, QString coverPath);

signals:
    void retSrcInfo(QVariantList vars);
    void retEditInfo(QVariantList vars);
    void retSig(QVariant var);
    void retProgressSig(int total, int succCnt, int failCnt);
};

#endif // INFOEDITMANAGER_H
