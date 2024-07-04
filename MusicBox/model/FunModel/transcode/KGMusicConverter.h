#ifndef KGMUSICCONVERTER_H
#define KGMUSICCONVERTER_H

#include <string>
#include <sstream>
#include <fstream>
#include <codecvt>

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
#include "MusicConverter.h"

class KGMusicConverter : public MusicConverter
{
public:
    explicit KGMusicConverter(QObject *parent = nullptr);
    void Decrypt(const std::filesystem::path& originalFilePath, std::filesystem::path _outputPath);

private:
    enum MType { VPR, OTHER };

    MType H;

    bool CheakHeader(std::stringstream& ms);

    int HeaderLength(std::stringstream& ms);

    std::string GetKey(std::stringstream& ms);

    void DecodeAudio(std::stringstream& ms, std::ofstream& f, const std::string& key);

    musicInfo GetMusicInfo(std::filesystem::path originalFilePath);

    void Rename(const musicInfo& info, const std::filesystem::path& originalFilePath, const std::filesystem::path& tempFilePath);

    void Save(const std::filesystem::path& originalFilePath, std::stringstream& ms, const std::filesystem::path& tempFilePath, std::ofstream& fs);
};

#endif // KGMUSICCONVERTER_H
