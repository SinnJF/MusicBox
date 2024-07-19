#ifndef KGMUSICCONVERTER_H
#define KGMUSICCONVERTER_H

#include <filesystem>

#include "Converter.h"

struct musicInfo;
class KGMusicConverter : public Converter
{
    Q_OBJECT
public:
    explicit KGMusicConverter();
    bool Decrypt(const QString srcFile, const QString dstPath);

private:
    enum MType { VPR, OTHER };

    MType H;
    std::filesystem::path m_originalFilePath;
    std::filesystem::path m_outputPath;

    bool CheakHeader(std::stringstream& ms);

    int HeaderLength(std::stringstream& ms);

    std::string GetKey(std::stringstream& ms);

    void DecodeAudio(std::stringstream& ms, std::ofstream& f, const std::string& key);

    musicInfo GetMusicInfo(std::filesystem::path originalFilePath);

    void Rename(const musicInfo& info, const std::filesystem::path& originalFilePath, const std::filesystem::path& tempFilePath);

    void Save(const std::filesystem::path& originalFilePath, std::stringstream& ms, const std::filesystem::path& tempFilePath, std::ofstream& fs);
};

#endif // KGMUSICCONVERTER_H
