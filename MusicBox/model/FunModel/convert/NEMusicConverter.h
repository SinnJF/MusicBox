#ifndef NEMUSICCONVERTER_H
#define NEMUSICCONVERTER_H

#include "Converter.h"

struct musicInfo;
class NEMusicConverter : public Converter
{
    Q_OBJECT
public:
    explicit NEMusicConverter();
    bool Decrypt(QString srcFile, QString dstDir, QString& dstFile) override;

private:

    bool CheakHeader(std::stringstream& ms);

    std::string GetRC4Key(std::stringstream& ms);

    musicInfo GetMusicInfo(std::stringstream& ms);

    int GetCRCCode(std::stringstream& ms);

    std::string GetImage(std::stringstream& ms);

    void DecodeAudio(std::stringstream& ms, std::ofstream& f, const std::string& RC4_key);

    void SetMusicInfo(QString filePath, musicInfo& info, bool write163Key);

    bool m_write163Key = true;

};

#endif // NEMUSICCONVERTER_H
