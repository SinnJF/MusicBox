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

    QString logPath;
#ifdef Q_OS_ANDROID
    //checkPermission();
    accessAllFiles();
    logPath = "/sdcard/MusicBox/log.txt";
    QDir dir;
    dir.mkdir("/sdcard/MusicBox/");
#else
    logPath = QDir::currentPath() + QDir::separator() + "log.txt";
#endif
    logFile.setFileName(logPath);
    qInfo() << "open " << logPath << " : " << logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    qInfo("==================log==================");
    //qInfo() << "main threadid: " << QThread::currentThreadId();
    AppService appService;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appService", &appService);
    engine.rootContext()->setContextProperty("log", &Logger::getInatance());

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::lastWindowClosed, endLog);

    engine.load(url);

    // QFile a("/storage/emulated/0/MusicBox/a.txt");
    // qInfo() << "===============000000000" << a.open(QIODevice::ReadOnly);
    // qInfo() << "==============111111111" << a.errorString() << "data: " << a.readAll();
    // QFileInfo aa(a);
    // qInfo() << "===============222222222" << aa.filePath() << aa.baseName() << aa.suffix();
    // a.close();
    // a.setFileName("/storage/emulated/0/MusicBox/b.txt");
    // qInfo() << "=============333333333" << a.open(QIODevice::WriteOnly);
    // qInfo() << "=============12313" << a.errorString();
    // a.write("bbbbbbbb");
    // a.close();


    return app.exec();
}
