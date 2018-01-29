#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "hwdecoderfactory.h"
#include "videorenderer.h"

int main(int argc, char *argv[])
{

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<VideoFBORenderer>("VideoHW", 0, 1, "VideoRenderer");
    qmlRegisterSingletonType<HWDecoder>("VideoHW", 0, 1, "HWDecoder", hwDecoderInstance);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
