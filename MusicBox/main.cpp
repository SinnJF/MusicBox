#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QDateTime>

#include "controller/AppService.h"
#include "common/Logger.h"

void outputMsg(QtMsgType type, const QMessageLogContext& context, const QString& msg) {

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
    QString message = QString("%1 %2 %3 %4").arg(current_date).arg(context.function).arg(logStr).arg(msg);
    emit Logger::getInatance().log(message);
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    qInstallMessageHandler(outputMsg);
    qDebug() << "main threadid: " << QThread::currentThreadId();
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
    engine.load(url);

    return app.exec();
}
