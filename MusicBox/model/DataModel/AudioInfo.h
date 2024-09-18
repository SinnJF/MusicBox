#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <QObject>
#include <QMetaType>
#include <QVariant>

class AudioInfo
{
    Q_GADGET

    Q_PROPERTY(QString path READ getPath WRITE setPath)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QString artist READ getArtist WRITE setArtist)
    Q_PROPERTY(QString album READ getAlbum WRITE setAlbum)
    Q_PROPERTY(QString coverPath READ getCoverPath WRITE setCoverPath)
public:
    AudioInfo();
    AudioInfo(const AudioInfo& info);   //QVariant的转换中起作用
    AudioInfo(QString path, QString title, QString artist, QString album, QString coverPath);

    Q_INVOKABLE QString getPath() const;
    Q_INVOKABLE void setPath(const QString &newPath);

    Q_INVOKABLE QString getTitle() const;
    Q_INVOKABLE void setTitle(const QString &newTitle);

    Q_INVOKABLE QString getArtist() const;
    Q_INVOKABLE void setArtist(const QString &newArtist);

    Q_INVOKABLE QString getAlbum() const;
    Q_INVOKABLE void setAlbum(const QString &newAlbum);

    Q_INVOKABLE QString getCoverPath() const;
    Q_INVOKABLE void setCoverPath(const QString &newCoverPath);

signals:

private:
    QString mPath;
    QString mTitle;
    QString mArtist;
    QString mAlbum;
    QString mCoverPath;

};

Q_DECLARE_METATYPE(AudioInfo)

#endif // AUDIOINFO_H
