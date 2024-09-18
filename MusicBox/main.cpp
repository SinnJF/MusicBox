#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QDir>
#include <QtGlobal>

#include "controller/AppConfig.h"
#include "controller/AppService.h"
#include "common/Logger.h"

#ifdef Q_OS_ANDROID
#include "common/AndroidHelper.h"
#endif

static QFile logFile;

void outputMsg(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    static QMutex mtx;
    QMutexLocker lock(&mtx);
    QString logStr;
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("(%1)").arg(current_date_time);
    switch (type) {
    case QtDebugMsg:
        //logStr = QString("%1 Debug: %2 %3\n").arg(current_date).arg(context.function).arg(msg);
        break;
    case QtInfoMsg:
        logStr = QString("%1 Info: %2 %3\n").arg(current_date).arg(context.function).arg(msg);
        break;
    case QtWarningMsg:
        logStr = QString("%1 Warning: %2 %3\n").arg(current_date).arg(context.function).arg(msg);
        break;
    case QtCriticalMsg:
        logStr = QString("%1 Critical: %2 %3\n").arg(current_date).arg(context.function).arg(msg);
        break;
    case QtFatalMsg:
        logStr = QString("%1 Fatal: %2 %3\n").arg(current_date).arg(context.function).arg(msg);
        break;
    }
    if(logFile.isOpen()) logFile.write(logStr.toUtf8());
    emit Logger::getInatance().log(logStr);
}

void endLog()
{
    if(logFile.isOpen())
    {
        qInfo("==================log end==================");
        logFile.close();
    }
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    qInstallMessageHandler(outputMsg);

    QString appPath;
    QString logPath;
#ifdef Q_OS_ANDROID
    checkPermission();
    accessAllFiles();
    appPath = "/sdcard/MusicBox";
    logPath = appPath + "/log.txt";
#else
    appPath = QCoreApplication::applicationDirPath();
    logPath = appPath + QDir::separator() + "log.txt";
#endif
    QDir dir;
    dir.mkdir(appPath);
    AppConfig::getInstance().setAppPath(appPath);
    dir.mkpath(AppConfig::getInstance().getCoverPath());

    logFile.setFileName(logPath);
    qInfo() << "open " << logPath << " : "
            << logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    qInfo("==================log==================");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appService", AppService::getInstance());
    engine.rootContext()->setContextProperty("log", &Logger::getInatance());

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::lastWindowClosed, endLog);

    engine.load(url);

    return app.exec();
}
