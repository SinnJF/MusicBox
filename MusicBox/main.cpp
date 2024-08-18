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
    QByteArray localMsg = msg.toUtf8();
    switch (type) {
    case QtDebugMsg:
        logStr = "Debug: ";
        break;
    case QtInfoMsg:
        logStr = "Info: ";
        break;
    case QtWarningMsg:
        logStr = "Warning: ";
        break;
    case QtCriticalMsg:
        logStr = "Critical: ";
        break;
    case QtFatalMsg:
        logStr = "Fatal: ";
        break;
    }
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4\n").arg(current_date).arg(context.function).arg(logStr).arg(msg);
    if(logFile.isOpen()) logFile.write(message.toUtf8());
    emit Logger::getInatance().log(message);
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
    //qInstallMessageHandler(outputMsg);

    QString logPath = QDir::currentPath() + QDir::separator() + "log.txt";
    logFile.setFileName(logPath);
    qInfo() << "open " << logPath << " : " << logFile.open(QIODevice::ReadWrite | QIODevice::Append);
    qInfo("==================log==================");
    qInfo() << "main threadid: " << QThread::currentThreadId();
    //TranscodeManager
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

#ifdef Q_OS_ANDROID
    //checkPermission();
    accessAllFiles();
#endif
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
