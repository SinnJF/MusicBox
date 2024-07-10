#ifndef CONVERTERFACTORY_H
#define CONVERTERFACTORY_H

#include <QObject>

class Converter;
class ConverterFactory : public QObject
{
    Q_OBJECT
public:
    enum class MusicType { Undefined, KGMusic, NEMusic };
    Q_ENUM(MusicType)

    enum class LyricType { Undefined, KGLyric, NELyric };
    Q_ENUM(LyricType)

    const static QStringList KGMusicSuffix;
    const static QStringList NEMusicSuffix;

    ConverterFactory(){}
    virtual ~ConverterFactory(){}

    virtual Converter* createConverter(MusicType type) = 0;

};
#endif // CONVERTERFACTORY_H
