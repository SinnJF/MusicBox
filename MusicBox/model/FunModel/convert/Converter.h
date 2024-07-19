#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QDebug>

class Converter : public QObject
{
    Q_OBJECT
public:
    explicit Converter(QObject *parent = nullptr){}
    virtual inline bool Decrypt(const QString, const QString){ qWarning() << tr("virtual Converter."); return false;};
//    virtual ~MusicConverter() = 0;

};

#endif // CONVERTER_H
