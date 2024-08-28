#include "KGMusicConverter.h"
#include "model/DataModel/KGMusicData.h"

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

#include "common/Common.h"

#include <QFileInfo>
//#include <codecvt>

KGMusicConverter::KGMusicConverter()
{

}

bool KGMusicConverter::Decrypt(QString srcFile, QString dstDir, QString &dstFile)
{
    qInfo() << "KGMusicConverter::Decrypt" << srcFile;
    std::ifstream f(srcFile.toStdString(), std::ios::binary);
    if (!f) {
        //throw std::runtime_error("打开文件失败");
        qWarning() << "open failed...";
        return false;
    };

    //读入文件
    std::stringstream ms;
    ms << f.rdbuf();
    f.close();

    //临时文件
    QString tempFilePath;
    QFileInfo srcFileInfo(srcFile);
    QString s = srcFileInfo.fileName();
    if(!dstDir.endsWith("/")) dstDir += "/";
    tempFilePath = dstDir + s;
    std::ofstream fs(tempFilePath.toStdString(), std::ios::out | std::ios_base::binary);

    //检查文件头
    if (!CheakHeader(ms)) { f.close(); return false; };
    //头部数据长度
    int length = HeaderLength(ms);
    //key
    auto key = GetKey(ms);

    //正式解密
    ms.seekg(length, std::ios_base::beg);
    DecodeAudio(ms, fs, key);

    auto info = GetMusicInfo(tempFilePath);
    bool ret = renameWithInfo(info, srcFile, tempFilePath, dstFile);
    if(!ret) qWarning() << "文件已存在，请先手动删除。" << dstFile;
    else qInfo() << "kg dstfile: " << dstFile;
    return ret;
}

bool KGMusicConverter::CheakHeader(std::stringstream &ms)
{
    char magic_hander[16];
    ms.read(magic_hander, 16);
    if (strncmp(magic_hander, (char*)KGMusicData::VprHeader, 16) == 0) { H = VPR; return true; };
    if (strncmp(magic_hander, (char*)KGMusicData::KgmHeader, 16) == 0) { H = OTHER; return true; };
    if ((strncmp(magic_hander, (char*)FLAC_HEADER, 3) == 0) or (strncmp(magic_hander, (char*)MP3_HEADER, 3) == 0)) { return false; }
    //throw std::runtime_error("文件已损坏或不是一个支持的文件");
    qWarning() << "KGMusicConverter: 文件已损坏或不支持该格式";
    return false;
}

int KGMusicConverter::HeaderLength(std::stringstream &ms)
{
    int length = 0;
    ms.read((char*)&length, 4);
    return length;
}

std::string KGMusicConverter::GetKey(std::stringstream &ms)
{
    std::string key(17, 0);
    ms.seekg(28, std::ios_base::beg);
    ms.read(key.data(), 16);
    return key;
}

void KGMusicConverter::DecodeAudio(std::stringstream &ms, std::ofstream &f, const std::string &key)
{
    size_t pos = 0, offset = 0;
    char med8, msk8;
    char buffer[4096];
    while (!ms.eof())
    {
        ms.read(buffer, 4096);

        for (int i = 0; i < 4096; i++)
        {
            med8 = (key)[(pos) % 17] ^ buffer[i];
            med8 ^= (med8 & 15) << 4;

            msk8 = 0;
            offset = pos >> 4;
            while (offset >= 0x11)
            {
                msk8 ^= KGMusicData::table1[offset % 272];
                offset >>= 4;
                msk8 ^= KGMusicData::table2[offset % 272];
                offset >>= 4;
            }
            msk8 = KGMusicData::MaskV2PreDef[pos % 272] ^ msk8;
            msk8 ^= (msk8 & 15) << 4;

            buffer[i] = med8 ^ msk8;
            if (H == VPR) { buffer[i] ^= KGMusicData::VprMaskDiff[pos % 17]; };
            pos++;
        }
        f.write(buffer, ms.gcount());
    }
    f.flush();
    f.close();
}

musicInfo KGMusicConverter::GetMusicInfo(QString filePath)
{
    using namespace TagLib;
    musicInfo info;
    std::ifstream file(filePath.toStdString());
    char header[3];
    file.read(header, 3);
    if (strncmp(header, (char*)FLAC_HEADER, 3) == 0)
    {
        file.close();
        info.format = "flac";
    }
    else
    {
        file.close();
        info.format = "mp3";
    }
    FileRef f(filePath.toStdString().c_str());
    auto tag = f.tag();
    info.artist.push_back(tag->artist().to8Bit(true));
    info.musicName = tag->title().to8Bit(true);
    return info;
}

bool KGMusicConverter::renameWithInfo(const musicInfo &info, const QString filePath, const QString orgFilePath, QString& dstFile)
{
    QString artistStr;
    for(const auto& s : info.artist)
    {
        artistStr = artistStr + "、" + QString::fromStdString(s);
    }
    artistStr = artistStr.sliced(1);
    artistStr.replace("\\", "、");
    artistStr.replace("/", "、");    //原生分隔

    //空文件名处理
    QFileInfo fi(filePath);
    QString name;
    if ("" == info.musicName)
    {
        name = QString("[unknown]%1.%2")
            .arg(fi.baseName())
            .arg(QString::fromStdString(info.format));
    }
    else
    {
        name = QString("%1 - %2.%3")
            .arg(artistStr)
            .arg(QString::fromStdString(info.musicName))
            .arg(QString::fromStdString(info.format));
    }

    fi.setFile(orgFilePath);
    QString dirPath = fi.path();
    if (!dirPath.endsWith("/")) {
        dirPath += "/";
    }
    dstFile = dirPath + name;
    return QFile::rename(orgFilePath, dstFile);
}
