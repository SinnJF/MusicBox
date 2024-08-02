#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>

class Logger: public QObject
{
    Q_OBJECT
public:
    static Logger& getInatance(){
        static Logger instance;
        return instance;
    }

private:
    Logger(){}
    ~Logger(){}
signals:
    void log(QString);
};

#endif // LOGGER_H
