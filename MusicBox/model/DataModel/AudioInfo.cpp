#include "AudioInfo.h"

AudioInfo::AudioInfo() {}

AudioInfo::AudioInfo(const AudioInfo &info)
{
    mPath = info.mPath;
    mTitle = info.mTitle;
    mArtist = info.mArtist;
    mAlbum = info.mAlbum;
    mCoverPath = info.mCoverPath;
}

AudioInfo::AudioInfo(QString path, QString title, QString artist, QString album, QString coverPath)
{
    mPath = path;
    mTitle = title;
    mArtist = artist;
    mAlbum = album;
    mCoverPath = coverPath;
}

QString AudioInfo::getPath() const
{
    return mPath;
}

void AudioInfo::setPath(const QString &newPath)
{
    if (mPath == newPath)
        return;
    mPath = newPath;
}

QString AudioInfo::getTitle() const
{
    return mTitle;
}

void AudioInfo::setTitle(const QString &newTitle)
{
    if (mTitle == newTitle)
        return;
    mTitle = newTitle;
}

QString AudioInfo::getArtist() const
{
    return mArtist;
}

void AudioInfo::setArtist(const QString &newArtist)
{
    if (mArtist == newArtist)
        return;
    mArtist = newArtist;
}

QString AudioInfo::getAlbum() const
{
    return mAlbum;
}

void AudioInfo::setAlbum(const QString &newAlbum)
{
    if (mAlbum == newAlbum)
        return;
    mAlbum = newAlbum;
}

QString AudioInfo::getCoverPath() const
{
    return mCoverPath;
}

void AudioInfo::setCoverPath(const QString &newCoverPath)
{
    if (mCoverPath == newCoverPath)
        return;
    mCoverPath = newCoverPath;
}
