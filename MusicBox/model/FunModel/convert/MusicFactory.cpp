#include "MusicFactory.h"
#include "KGMusicConverter.h"
#include "NEMusicConverter.h"

#include <QDebug>
#include <QFileInfo>

MusicFactory::MusicFactory()
{

}

MusicFactory::~MusicFactory()
{

}

//factory只管生产，对象需自己管理
Converter *MusicFactory::createConverter(MusicType type)
{
    switch (type) {
    case MusicType::Undefined:
        qWarning() << tr("未接入该音乐类型");
        return new Converter();
    case MusicType::KGMusic:
        return new KGMusicConverter();
    case MusicType::NEMusic:
        return new NEMusicConverter();
    }
}
