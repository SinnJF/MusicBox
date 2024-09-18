#include "AppConfig.h"

AppConfig::AppConfig() {}

QString AppConfig::getAppPath() const
{
    return appPath;
}

void AppConfig::setAppPath(const QString &newAppPath)
{
    appPath = newAppPath;
}

QString AppConfig::getCoverPath() const
{
    return appPath + "/cover";
}
