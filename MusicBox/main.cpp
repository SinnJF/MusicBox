#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>

#include "controller/TranscodeManager.h"


int main(int argc, char *argv[])
{
    qDebug() << "???";
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qDebug() << "main threadid: " << QThread::currentThreadId();
    //TranscodeManager
    TranscodeManager transcodeManager;
    QObject::connect(&transcodeManager, &TranscodeManager::startSig, &transcodeManager, &TranscodeManager::test, Qt::QueuedConnection);
    QThread transcodeThread;
    transcodeManager.moveToThread(&transcodeThread);
    transcodeThread.start();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("transcodeManager", &transcodeManager);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
