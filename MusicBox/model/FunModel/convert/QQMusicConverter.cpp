/*
 * source:
MIT License

Copyright (c) 2020 Kcass

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "QQMusicConverter.h"
#include "model/DataModel/QQMusicData.h"

#include <QFile>
#include <QFileInfo>

QQMusicConverter::QQMusicConverter(QObject *parent)
    : Converter{parent}
{
    Q_UNUSED(parent)
}

bool QQMusicConverter::Decrypt(QString srcFile, QString dstPath, QString& dstFile)
{
    QFile file(srcFile);
    bool ret = file.open(QIODevice::ReadOnly);
    qInfo() << "open file: " << srcFile << ret;
    if(!ret) return false;
    QByteArray ba = file.readAll();
    file.close();

    char* data = ba.data();
    int size = ba.size();
    if(size < 1) return false;
    QmcMask qmcMask;
    qmcMask.generateMask128from44(qmcMask.Mask128, QQMusicData::QMCDefaultMaskMatrix);
    const char* matrix128 = qmcMask.Mask128;
    //qWarning() << size << QByteArray::fromRawData(qmcMask.Mask128, 128).toHex(' ');
    int index = -1;
    int maskIdx = -1;
    for (int cur = 0; cur < size; cur++) {
        index++;
        maskIdx++;
        if (index == 0x8000 || (index > 0x8000 && (index + 1) % 0x8000 == 0)) {
            index++;
            maskIdx++;
        }
        if (maskIdx >= 128)
            maskIdx -= 128;
        data[cur] ^= matrix128[maskIdx];
    }

    if(!dstPath.endsWith("/")) dstPath += "/";
    char head[5] = {};
    strncpy(head, data, 5);
    QString suffix = ".mp3";    //默认
    const char flacHead[5] = {0x66, 0x4c, 0x61, 0x43, 0x00};
    qWarning() << head << "---" << flacHead;
    if(!strcmp(head, flacHead)) suffix = ".flac";
    QFileInfo info(srcFile);
    dstFile= dstPath + info.baseName() + suffix;
    file.setFileName(dstFile);
    ret = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    if(!ret) { qWarning() << dstFile << file.errorString(); return false; }
    QDataStream stream(&file);
    int n = stream.writeRawData(data, size);
    file.close();
    ret = n == size;
    return ret;
}

QQMusicConverter::QmcMask::QmcMask()
{
    for (auto i = 0; i < 128; i++) {
        auto realIdx = (i * i + 27) % 256;
        if (this->m_allMapping.find(realIdx) != this->m_allMapping.end())
            this->m_allMapping[realIdx].push_back(i);
        else
            this->m_allMapping[realIdx] = { i };
    }

    auto idx44 = 0;
    for (auto& all128 : this->m_allMapping) {
        Q_UNUSED(all128)
        for (auto& _i128 : this->m_allMapping)
            this->m_mask128to44[_i128.first] = { idx44 };
        idx44++;
    };
}

void QQMusicConverter::QmcMask::generateMask128from44(char *dst, const uchar *src)
{
    auto idx44 = 0;
    for (auto& it256 : this->m_allMapping) {
        for (auto& m : it256.second)
            dst[m] = src[idx44];
        idx44++;
    };
}
