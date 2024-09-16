#include "InfoEditManager.h"
#include <QFileInfo>
#include <QString>
#include <QUrl>
#include <QQmlFile>
#include <QFile>
#include <QByteArray>
#include <QPixmap>
#include <QVariantList>
#include <QVariantMap>
#include <QCoreApplication>
#include <QDir>

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/flac/flacfile.h>
#include <taglib/flac/flacpicture.h>
#include <taglib/flac/flacmetadatablock.h>
#include <taglib/flac/flacproperties.h>
#include <taglib/flac/flacunknownmetadatablock.h>

#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/id3v2frame.h>
#include <taglib/mpeg/id3v2/id3v2header.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>
#include <taglib/mpeg/id3v2/id3v2framefactory.h>
#include <taglib/mpeg/id3v2/frames/textidentificationframe.h>

#include "model/DataModel/AudioInfo.h"

void InfoEditManager::getSrcInfo(QList<QUrl> vars)
{
    QFileInfo fileInfo;
    QVariantList retVars;
    using namespace TagLib;
    for(const auto &url : vars)
    {
        QString filePath;
        QString fileName;
#ifdef Q_OS_ANDROID
        filePath = getRealPathFromUri(url));
#else
        filePath = QQmlFile::urlToLocalFileOrQrc(url);
#endif
        fileInfo.setFile(filePath);
        fileName = fileInfo.completeBaseName();
        qDebug() << "InfoEditManager path: " << filePath;
        if(!fileInfo.isFile()) {
            qWarning() << "access file fail...";
            continue;
        }
        AudioInfo info;
        QString cover;
        {
        FileRef file(filePath.toStdWString().c_str());
        String title_string = file.tag()->title();
        String album_string = file.tag()->album();
        QString title = QString::fromStdWString(title_string.toWString());
        QString  album = QString::fromStdWString(album_string.toWString());
        QString artist = QString::fromStdWString(file.tag()->artist().toWString());
        QString comment = QString::fromStdWString(file.tag()->comment().toWString());
        QString genre = QString::fromStdWString(file.tag()->genre().toWString());
        QString year = QString::number(file.tag()->year());
        QString lengths = QString::number(file.audioProperties()->bitrate());

        qDebug() << title << album << artist;
        info = AudioInfo(filePath, title, artist, album, "");
        }

        QFile f(filePath);
        if(!f.open(QIODevice::ReadOnly)) continue;
        QByteArray header = f.read(4);
        f.close();

        if(header.left(4) == flacHeader)
        {
            qDebug() << "src: detect flac tag";
            FLAC::File file(filePath.toStdWString().c_str());

            qWarning() << "is valid: " << file.isValid()
                       << QString::fromStdWString(file.name().wstr());
            List<FLAC::Picture*> pics = file.pictureList();
            qWarning() << pics.size();
            for(const auto& pic : pics)
            {
                switch (pic->type()) {
                case FLAC::Picture::FileIcon: {
                    ByteVector data = pic->data();
                    //QByteArray ba(data.data(), data.size());
                    QPixmap pixmap;
                    pixmap.loadFromData((const uchar*)data.data(), data.size());
                    cover = coverTmpDir + fileName + "_fileIcon.png";
                    qWarning() << "save flac fileicon: " << pixmap.save(cover);
                    break;
                }
                case FLAC::Picture::OtherFileIcon: {
                    ByteVector data = pic->data();
                    //QByteArray ba(data.data(), data.size());
                    QPixmap pixmap;
                    pixmap.loadFromData((const uchar*)data.data(), data.size());
                    cover = coverTmpDir + fileName + "_otherFileIcon.png";
                    qWarning() << "save flac otherFileIcon: " << pixmap.save(cover);
                    break;
                }
                case FLAC::Picture::FrontCover: {
                    ByteVector data = pic->data();
                    //QByteArray ba(data.data(), data.size());
                    QPixmap pixmap;
                    pixmap.loadFromData((const uchar*)data.data(), data.size());
                    cover = coverTmpDir + fileName + "_fontCover.png";
                    qWarning() << "save flac fontCover: " << pixmap.save(cover);
                    break;
                }
                case FLAC::Picture::BackCover: {
                    ByteVector data = pic->data();
                    //QByteArray ba(data.data(), data.size());
                    QPixmap pixmap;
                    pixmap.loadFromData((const uchar*)data.data(), data.size());
                    cover = coverTmpDir + fileName + "_backCover.png";
                    qWarning() << "save flac backCover: " << pixmap.save(cover);
                    break;
                }
                default:
                    qWarning() << "flac file get no img";
                    break;
                }
            }
        }
        else if(header.left(3) == ID3Header)
        {
            qDebug() << "src: detect id3 tag";
            MPEG::File file(filePath.toStdWString().c_str());
            qWarning() << "isvalid: " << file.isValid()
                       << QString::fromStdWString(file.name().wstr());
            ID3v2::Tag* tag = file.ID3v2Tag();
            if(!tag) break;
            ID3v2::FrameList fList = tag->frameList("APIC");
            if(fList.size() > 0)
            {
                ID3v2::AttachedPictureFrame* coverFrame =
                    static_cast<ID3v2::AttachedPictureFrame*>(fList.front());
                QPixmap pixmap;
                pixmap.loadFromData((const uchar*)coverFrame->picture().data(),
                                    coverFrame->picture().size());
                cover = coverTmpDir + fileName + "_apic.png";
                qWarning() << "save id3 apic: " << pixmap.save(cover);
            }
        }
        info.setCoverPath(cover);
        retVars.append(QVariant::fromValue(info));
    }
    emit retSrcInfo(retVars);
}

//根据数据保存进文件并返回处理结果
void InfoEditManager::editInfo(QVariant vars)
{
    QVariantMap contentMap = vars.toMap();
    auto iter = contentMap.constKeyValueBegin();
    QVariantMap retMap;
    int total = contentMap.keys().size(), successCnt = 0, failCnt = 0;
    for(; iter != contentMap.constKeyValueEnd(); iter++){
        int index = iter->first.toInt();
        bool flag = false;
        QVariantMap map = iter->second.toMap();
        // qDebug() << map.value("path").toString()
        //          << map.value("title").toString()
        //          << map.value("artist").toString()
        //          << map.value("album").toString()
        //          << map.value("cover").toString();
        const auto& filePath = map.value("path").toString();
        const auto& title = map.value("title").toString();
        const auto& artist = map.value("artist").toString();
        const auto& album = map.value("album").toString();
        const auto& coverPath = map.value("cover").toString();

        QFile f(filePath);
        if(!f.open(QIODevice::ReadOnly)) {
            qWarning() << "access file fail...";
            failCnt++;
            continue;
        }
        QByteArray header = f.read(4);
        f.close();

        if(header.left(4) == flacHeader)
        {
            editFlacInfo(filePath, title, artist, album, coverPath)
                ? successCnt++ : failCnt++;
        }
        else if(header.left(3) == ID3Header)
        {
            editMpegInfo(filePath, title, artist, album, coverPath)
                ? successCnt++ : failCnt++;
        }
        else {
            qWarning() << "2暂不支持该格式或文件损坏：" << filePath;
            failCnt++;
        }
        retMap[QString::number(index)] = QVariant::fromValue(flag);
        emit retSig(QVariant::fromValue(retMap));
        emit retProgressSig(total, successCnt, failCnt);
    }


}

InfoEditManager::InfoEditManager(QObject *parent)
{
    createTmpDir();
}

InfoEditManager::~InfoEditManager()
{
    clearTmpFiles();
}

void InfoEditManager::createTmpDir()
{
    coverTmpDir = QCoreApplication::applicationDirPath() + "/cover/";
    QDir dir;
    dir.mkpath(coverTmpDir);
}

void InfoEditManager::clearTmpFiles()
{
    QDir dir(coverTmpDir);
    dir.removeRecursively();
}

bool InfoEditManager::editMpegInfo(QString filePath, QString title, QString artist, QString album, QString coverPath)
{
    using namespace TagLib;
    qDebug() << "edit: detect id3 tag";
    MPEG::File file(filePath.toStdWString().c_str());
    if(!file.isValid()){
        qWarning() << "暂不支持该格式或文件损坏：" << filePath;
        return false;
    }
    file.tag()->setTitle(title.toStdWString().c_str());
    file.tag()->setArtist(artist.toStdWString().c_str());
    file.tag()->setAlbum(album.toStdWString().c_str());
    QFile f(coverPath);
    if(f.open(QIODevice::ReadOnly))
    {
        QByteArray ba = f.readAll();
        ID3v2::Tag* tag = file.ID3v2Tag(true);
        auto img = new ID3v2::AttachedPictureFrame();
        img->setMimeType("image/jpeg");
        img->setType(ID3v2::AttachedPictureFrame::FrontCover);
        img->setPicture(ByteVector(ba.data(), ba.size()));
        tag->removeFrames("APIC");
        tag->addFrame(img);
    }
    file.save();
    return true;
}

bool InfoEditManager::editFlacInfo(QString filePath, QString title, QString artist, QString album, QString coverPath)
{
    using namespace TagLib;
    qDebug() << "edit: detect flac tag";
    FLAC::File file(filePath.toStdWString().c_str());
    if(!file.isValid()){
        qWarning() << "暂不支持该格式或文件损坏：" << filePath;
        return false;
    }
    file.tag()->setTitle(title.toStdWString().c_str());
    file.tag()->setArtist(artist.toStdWString().c_str());
    file.tag()->setAlbum(album.toStdWString().c_str());
    List<FLAC::Picture*> pics = file.pictureList();
    for (auto pic : pics) {
        file.removePicture(pic);
    }
    QFile f(coverPath);
    if(f.open(QIODevice::ReadOnly))
    {
        QByteArray ba = f.readAll();
        auto img = new FLAC::Picture();
        img->setData(ByteVector(ba.data(), ba.size()));
        img->setMimeType("image/jpeg");
        img->setType(FLAC::Picture::FrontCover);
        file.addPicture(img);
    }
    file.save();
    return true;
}
