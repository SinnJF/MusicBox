﻿#include "NEMusicConverter.h"
#include "model/DataModel/NCMusicData.h"

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/toolkit/tpropertymap.h>
#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/id3v2frame.h>
#include <taglib/mpeg/id3v2/id3v2header.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>
#include <taglib/flac/flacfile.h>
#include <taglib/flac/flacpicture.h>
#include <taglib/ogg/xiphcomment.h>
#include <taglib/mpeg/id3v2/id3v2framefactory.h>
#include <taglib/mpeg/id3v2/frames/textidentificationframe.h>

#include "common/Cde.h"
#include "common/Common.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>

NEMusicConverter::NEMusicConverter()
{

}

bool NEMusicConverter::Decrypt(QString srcFile, QString dstDir, QString &dstFile)
{
    qInfo() << "NEMusicConverter::Decrypt" << srcFile;
    std::ifstream f(srcFile.toStdString(), std::ios::binary);
    if (!f) {
        qWarning() << "open fail... " ;
        return false;
    };

    std::stringstream ms;
    ms << f.rdbuf();
    f.close();

    if(!CheakHeader(ms)) return false;
    auto RC4_key = GetRC4Key(ms);
    auto info = GetMusicInfo(ms);
    int CRC = GetCRCCode(ms);
    Q_UNUSED(CRC)
    ms.seekg(5, std::ios::cur);
    //获取封面
    info.cover = GetImage(ms);

    //拼接文件名
    QString artistStr;
    for(const auto& s : info.artist)
    {
        artistStr = artistStr + " " + QString::fromStdString(s);
    }
    artistStr = artistStr.simplified();
    QString name = QString("%1 - %2.%3")
                   .arg(artistStr)
                   .arg(QString::fromStdString(info.musicName))
                   .arg(QString::fromStdString(info.format));

    if(!dstDir.endsWith("/")) dstDir += "/";
    dstFile = dstDir + name;

    //正式解密文件
    std::ofstream file(dstFile.toStdString(), std::ios::out | std::ios::binary);
    if (!file.good()) {
        qInfo() << "file open error: " << dstFile;
        return false;
    }
    DecodeAudio(ms, file, RC4_key);

    SetMusicInfo(dstFile, info, m_write163Key);
    qInfo() << "SetMusicInfo & save finish..." << dstFile;
    return true;
}

bool NEMusicConverter:: CheakHeader(std::stringstream &ms)
{
    char magic_hander[10];
    ms.read(magic_hander, 10);
    if (strncmp(NCMusicData::NCM_hander, magic_hander, 9) != 0)
    {
        qWarning() << "ncm已损坏或不支持该格式";
        return false;
    }
    return true;
}

std::string NEMusicConverter::GetRC4Key(std::stringstream &ms)
{
    int length = 0;//加密RC4密钥的密钥长度
    ms.read((char*)&length, 4);

    std::string data(length, '\0');//加密后的RC4密钥
    ms.read(data.data(), length);
    for (int i = 0; i <= (length - 1); i++)
    {
        data[i] ^= 0x64;//对每个字节与0x64进行异或
    }
    auto RC4_key = aes_ecb_decrypt(data, NCMusicData::_core_key).substr(17);//AES后 去除前17位
    return RC4_key;
}

musicInfo NEMusicConverter::GetMusicInfo(std::stringstream &ms)
{
    musicInfo inf;
    int length = 0;//音乐信息长度
    ms.read((char*)&length, 4);

    std::string data(length, '\0');//信息数据
    ms.read(data.data(), length);
    for (int i = 0; i < length; i++)
    { //对每个字节与0x63进行异或
        data[i] ^= 0x63;
    }
    inf.ncmkey = data;
    data = data.substr(22);//去除前22位
    data = base64_decode(data);//base64
    std::string info = aes_ecb_decrypt(data, NCMusicData::_info_key).substr(6);//aes后去除前6位

    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(info).toUtf8());
    qInfo() << doc;

    //音乐名
    if (doc["musicName"].isString())
    {
        QString str = doc["musicName"].toString();
        inf.musicName = str.toStdString();
    }

    //后缀名
    if (doc["format"].isString())
    {
        QString str = doc["format"].toString();
        inf.format = str.toStdString();
    }

    //歌手
    if (doc["artist"].isArray())
    {
        QJsonArray arr = doc["artist"].toArray();
        for (int i = 0; i < arr.size(); i++)
        {
            if(arr[i].isArray())
                inf.artist.push_back(arr[i].toArray().at(0).toString().toStdString());
            else
                qInfo() << "GetMusicInfo err: get artist err";
        }
    }

    //专辑
    if (doc["album"].isString())
    {
        QString str = doc["album"].toString();
        inf.album = str.toStdString();
    }

    return inf;
}

int NEMusicConverter::GetCRCCode(std::stringstream &ms)
{
    int CRC = 0;
    ms.read((char*)&CRC, 4);
    return CRC;
}

std::string NEMusicConverter::GetImage(std::stringstream &ms)
{
    int length = 0;
    ms.read((char*)&length, 4);

    std::string data(length, '\0');
    ms.read(data.data(), length);
    return data;
}

void NEMusicConverter::DecodeAudio(std::stringstream &ms, std::ofstream &f, const std::string &RC4_key)
{
    //初始化
    char S[256] = {};
    char K[256] = {};
    for (int i = 0; i < 256; i++) { S[i] = i; }

    //初始替换
    int j = 0;
    for (int i = 0, temp; i < 256; i++)
    {
        j = (j + S[i] + (RC4_key)[i % RC4_key.size()]) & 255;
        temp = S[i];
        S[i] = S[j];
        S[j] = temp;
    }

    //密钥流
    for (int i = 0, a, b; i < 256; i++)
    {
        a = (i + 1) & 255;
        b = S[(a + S[a]) & 255];
        K[i] = (S[(S[a] + b) & 255] & 255);
    }

    //解密流
    char buffer[256];
    while (!ms.eof())
    {
        ms.read(buffer, 256);
        for (int i = 0; i < 256; i++) { buffer[i] ^= K[i]; };
        f.write(buffer, ms.gcount());
    };
    f.flush();
    f.close();
}

//BUG:转出的歌名专辑等内容编码有问题
void NEMusicConverter::SetMusicInfo(QString filePath, musicInfo& info, bool write163Key)
{
    using namespace TagLib;
    if (info.format == "flac")
    {
        FLAC::File file(filePath.toStdString().c_str());
        auto img = new FLAC::Picture();
        img->setData(ByteVector(info.cover.data(), info.cover.length()));
        img->setMimeType("image/jpeg");
        img->setType(FLAC::Picture::FrontCover);
        file.addPicture(img);
        auto xiph = file.xiphComment(true);
        if (write163Key)xiph->addField("DESCRIPTION", String(info.ncmkey, String::UTF8));
        xiph->addField("ALBUM", String(info.album, String::UTF8));
        xiph->addField("ARTIST", String(join(info.artist, ";"), String::UTF8));

        file.save();
    }
    else
    {
        MPEG::File file(filePath.toStdString().c_str());
        auto tag = file.ID3v2Tag(true);
        auto img = new ID3v2::AttachedPictureFrame();
        img->setMimeType("image/jpeg");
        img->setType(ID3v2::AttachedPictureFrame::FrontCover);
        img->setPicture(ByteVector(info.cover.data(), info.cover.length()));
        tag->removeFrames("APIC");
        tag->addFrame(img);
        if (write163Key)tag->setComment(String(info.ncmkey, String::UTF8));
        tag->setArtist(String(join(info.artist, ";"), String::UTF8));
        tag->setAlbum(String(info.album, String::UTF8));

        file.save();
    }
}
