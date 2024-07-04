#include "KGMusicConverter.h"
#include "model/DataModel/KGMusicData.h"

KGMusicConverter::KGMusicConverter(QObject *parent)
    : MusicConverter{parent}
{

}

void KGMusicConverter::Decrypt(const std::filesystem::path &originalFilePath, std::filesystem::path _outputPath)
{
    std::ifstream f(originalFilePath, std::ios::binary);
    if (!f) { throw std::runtime_error("打开文件失败"); return; };

    //读入文件
    std::stringstream ms;
    ms << f.rdbuf();
    f.close();

    //临时文件
    std::filesystem::path tempFilePath;
    if (_outputPath.empty())
    {
        tempFilePath = CreateTempFile(originalFilePath.parent_path());
    }
    else
    {
        tempFilePath = CreateTempFile(_outputPath);
    }
    std::ofstream fs(tempFilePath, std::ios::out | std::ios_base::binary);

    //检查文件头
    if (!CheakHeader(ms)) { f.close(); Save(originalFilePath, ms, tempFilePath, fs); return; };
    //头部数据长度
    int length = HeaderLength(ms);
    //key
    auto key = GetKey(ms);

    //正式解密
    ms.seekg(length, std::ios_base::beg);
    DecodeAudio(ms, fs, key);

    auto info = GetMusicInfo(tempFilePath);
    Rename(info, originalFilePath, tempFilePath);
}

bool KGMusicConverter::CheakHeader(std::stringstream &ms)
{
    char magic_hander[16];
    ms.read(magic_hander, 16);
    if (strncmp(magic_hander, (char*)KGMusicData::VprHeader, 16) == 0) { H = VPR; return true; };
    if (strncmp(magic_hander, (char*)KGMusicData::KgmHeader, 16) == 0) { H = OTHER; return true; };
    if ((strncmp(magic_hander, (char*)FLAC_HEADER, 3) == 0) or (strncmp(magic_hander, (char*)MP3_HEADER, 3) == 0)) { return false; }
    throw std::runtime_error("文件已损坏或不是一个支持的文件");
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

musicInfo KGMusicConverter::GetMusicInfo(std::filesystem::path originalFilePath)
{
    using namespace TagLib;
    musicInfo info;
    std::ifstream file(originalFilePath);
    char magic_hander[3];
    file.read(magic_hander, 3);
    if (strncmp(magic_hander, (char*)FLAC_HEADER, 3) == 0)
    {
        file.close();
        info.format = "flac";
    }
    else
    {
        file.close();
        info.format = "mp3";
    }
    FileRef f(originalFilePath.c_str());
    auto tag = f.tag();
    info.artist.push_back(tag->artist().to8Bit(true));
    info.musicName = tag->title().to8Bit(true);
    return info;
}

void KGMusicConverter::Rename(const musicInfo &info, const std::filesystem::path &originalFilePath, const std::filesystem::path &tempFilePath)
{
    std::string name;
    std::vector<std::string> artist;
    if (info.artist.size() > 3) { artist = { info.artist[0],info.artist[1],info.artist[2],"..." }; }
    else { artist = info.artist; };

    //空文件名处理
    if ("" == info.musicName)
    {
        name = ("[未命名]" + std::string((char*)originalFilePath.filename().u8string().c_str()) + "." + info.format);
    }
    else
    {
        name = (info.musicName + " - " + join(artist, (std::string)",") + "." + info.format);
    }

    //替换为全角字符,防止出错
    name = replace_(name, "?", { "？" });
    name = replace_(name, "*", {"＊"});
    name = replace_(name, ":", { "：" });
    name = replace_(name, "<", { "＜" });
    name = replace_(name, ">", { "＞" });
    name = replace_(name, "/", { "／" });
    name = replace_(name, "\\", { "＼" });
    name = replace_(name, "|", { "｜" });
    name = replace_(name, "\"", "＂");

    //utf-8文件名
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wideFilename = converter.from_bytes(name);

    std::filesystem::path save_path = tempFilePath.parent_path().append(wideFilename);
    rename(tempFilePath, save_path);
}

void KGMusicConverter::Save(const std::filesystem::path &originalFilePath, std::stringstream &ms, const std::filesystem::path &tempFilePath, std::ofstream &fs)
{
    auto info = GetMusicInfo(originalFilePath);

    //复制文件
    ms.seekg(0, std::ios_base::beg);
    fs << ms.rdbuf();
    fs.flush();
    fs.close();
    Rename(info, originalFilePath, tempFilePath);
}
