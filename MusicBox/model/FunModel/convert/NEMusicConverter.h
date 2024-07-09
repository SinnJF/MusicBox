#ifndef NEMUSICCONVERTER_H
#define NEMUSICCONVERTER_H

#include <filesystem>

#include "Converter.h"

struct musicInfo;
class NEMusicConverter : public Converter
{
public:
    explicit NEMusicConverter();
    void Decrypt(const QString srcFile, const QString dstPath);

private:
    std::filesystem::path m_originalFilePath;
    std::filesystem::path m_outputPath;

    void CheakHeader(std::stringstream& ms);

    std::string GetRC4Key(std::stringstream& ms);

    musicInfo GetMusicInfo(std::stringstream& ms);

    int GetCRCCode(std::stringstream& ms);

    std::string GetImage(std::stringstream& ms);

    void DecodeAudio(std::stringstream& ms, std::ofstream& f, const std::string& RC4_key);

    void SetMusicInfo(std::filesystem::path& originalFilePath, musicInfo& info, bool write163Key);

    bool m_write163Key = true;

};

#endif // NEMUSICCONVERTER_H
