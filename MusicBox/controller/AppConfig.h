#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

class AppConfig
{
public:
    static AppConfig& getInstance()
    {
        static AppConfig a;
        return a;
    }
    QString getAppPath() const;
    void setAppPath(const QString &newAppPath);

    QString getCoverPath() const;
    void setCoverPath(const QString &newCoverPath);

private:
    AppConfig();

    QString appPath;
    QString coverPath;
};

#endif // APPCONFIG_H
