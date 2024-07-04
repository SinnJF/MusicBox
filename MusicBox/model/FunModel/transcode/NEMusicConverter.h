#ifndef NEMUSICCONVERTER_H
#define NEMUSICCONVERTER_H

#include <filesystem>

#include "MusicConverter.h"

struct musicInfo;
class NEMusicConverter : public MusicConverter
{
public:
    explicit NEMusicConverter(QObject *parent = nullptr);
    void Decrypt(const std::filesystem::path& originalFilePath, bool write163Key, std::filesystem::path _outputPath);

private:
    void CheakHeader(std::stringstream& ms);

    std::string GetRC4Key(std::stringstream& ms);

    musicInfo GetMusicInfo(std::stringstream& ms);

    int GetCRCCode(std::stringstream& ms);

    std::string GetImage(std::stringstream& ms);

    void DecodeAudio(std::stringstream& ms, std::ofstream& f, const std::string& RC4_key);

    void SetMusicInfo(std::filesystem::path& originalFilePath, musicInfo& info, bool write163Key);

};

#endif // NEMUSICCONVERTER_H
