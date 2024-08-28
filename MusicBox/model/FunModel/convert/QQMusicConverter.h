#ifndef QQMUSICCONVERTER_H
#define QQMUSICCONVERTER_H

#include "Converter.h"

struct musicInfo;
class QQMusicConverter : public Converter
{
public:
    explicit QQMusicConverter(QObject *parent = nullptr);
    bool Decrypt(QString srcFile, QString dstDir, QString& dstFile) override;

private:
    class QmcMask
    {
    public:
        explicit QmcMask();
        void generateMask128from44(char* dst, const uchar *src);

        char Mask128[128];
        std::map<int, std::vector<int>> m_allMapping;
        std::map<int, std::vector<int>> m_mask128to44;
    };
    //static QmcMask qmcMask;
};

#endif // QQMUSICCONVERTER_H
