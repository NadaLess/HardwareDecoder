#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "hwdecoderfactory.h"
#include "videorenderer.h"
#include "d3d9interop.h"

int main(int argc, char *argv[])
{

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    //Fnadales: Needed to check OpenGLExtensions for D3D9
    D3D9Interop::instance();

    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<VideoSource>("VideoHW", 0, 1, "VideoSource", "C++ Created");
    qmlRegisterSingletonType<HWDecoder>("VideoHW", 0, 1, "HWDecoder", hwDecoderInstance);
    qmlRegisterType<VideoFBORenderer>("VideoHW", 0, 1, "VideoRenderer");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
