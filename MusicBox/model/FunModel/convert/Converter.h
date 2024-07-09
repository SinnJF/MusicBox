#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QDebug>

class Converter : public QObject
{
    Q_OBJECT
public:
    explicit Converter(QObject *parent = nullptr){}
    virtual inline void Decrypt(const QString, const QString){ qWarning() << tr("Undefined Converter."); };
//    virtual ~MusicConverter() = 0;

};

#endif // CONVERTER_H
