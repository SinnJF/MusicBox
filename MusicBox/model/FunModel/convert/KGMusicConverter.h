#ifndef KGMUSICCONVERTER_H
#define KGMUSICCONVERTER_H

#include "Converter.h"

struct musicInfo;
class KGMusicConverter : public Converter
{
    Q_OBJECT
public:
    explicit KGMusicConverter();
    bool Decrypt(QString srcFile, QString dstDir, QString& dstFile) override;

private:
    enum MType { VPR, OTHER };

    MType H;

    bool CheakHeader(std::stringstream& ms);

    int HeaderLength(std::stringstream& ms);

    std::string GetKey(std::stringstream& ms);

    void DecodeAudio(std::stringstream& ms, std::ofstream& f, const std::string& key);

    musicInfo GetMusicInfo(QString filePath);

    bool renameWithInfo(const musicInfo& info, const QString filePath, const QString orgFilePath, QString& dstFile);
};

#endif // KGMUSICCONVERTER_H
