#ifndef MUSICFACTORY_H
#define MUSICFACTORY_H

#include <QObject>

class MusicFactory : public QObject
{
    Q_OBJECT
public:
    explicit MusicFactory(QObject *parent = nullptr);

signals:

};

#endif // MUSICFACTORY_H
