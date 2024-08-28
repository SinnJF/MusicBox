#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QDebug>

class Converter : public QObject
{
    Q_OBJECT
public:
    explicit Converter(QObject *parent = nullptr){Q_UNUSED(parent)}
    virtual inline bool Decrypt(const QString srcFile, const QString dstDir, QString& dstFile){ qWarning() << tr("virtual Converter."); return false;};
//    virtual ~MusicConverter() = 0;

};

#endif // CONVERTER_H
