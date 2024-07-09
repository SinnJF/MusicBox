#ifndef MUSICFACTORY_H
#define MUSICFACTORY_H

#include "ConverterFactory.h"

class MusicFactory : public ConverterFactory
{
    Q_OBJECT
public:
    explicit MusicFactory();
    ~MusicFactory();

    Converter* createConverter(MusicType type);

signals:

};

#endif // MUSICFACTORY_H
