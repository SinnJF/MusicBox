#ifndef MUSICCONVERTER_H
#define MUSICCONVERTER_H

#include <QObject>

class MusicConverter : public QObject
{
    Q_OBJECT
public:
    explicit MusicConverter(QObject *parent = nullptr);

signals:

};

#endif // MUSICCONVERTER_H
