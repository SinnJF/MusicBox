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
    static AppService* getInstance();

    Q_INVOKABLE void handleRenameSwitch(QVariant vars);
    Q_INVOKABLE void handleRenameRepl(QVariant vars, QString tobeRepl, QString repl);
    Q_INVOKABLE void handleAddTail(QVariant vars, QString tail);

    Q_INVOKABLE QString getRealPath(QVariant var);
    Q_INVOKABLE QStringList getRealPaths(QList<QUrl> vars);

private:
    static AppService* instance;
    explicit AppService(QObject *parent = nullptr);
    ~AppService();

    class CgarRes   //类中嵌套，用来释放对象
    {
    public:
        ~CgarRes()
        {
            if (AppService::instance)
            {
                delete AppService::instance;
                AppService::instance = nullptr;
            }
        }
    };

    QThread* transManThread = nullptr;
    QThread* infoEditThread = nullptr;

signals:
    void getSelectedFilesSig(QList<QUrl> urls);
    void getTargetFolderSig(QUrl url, int mode);
    void updateTargetFolderSig(QString path);
    void updateSeleFilesSig(QStringList files);
    void handleMusicFiles(QVariant filesVar, QString destFile);

    void retMsgSig(QString result);
    void retSig(QVariant results);
    void retProgressSig(int, int, int);

    void getFilesnfoSig(QList<QUrl> urls);
    void retFilesInfoSig(QVariantList vars);
    void editFileInfoSig(QVariant vars);
};

#endif // APPSERVICE_H
