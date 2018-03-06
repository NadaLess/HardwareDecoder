#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "fileprocessor.h"
#include "videorenderer.h"
#include "openglinterop.h"

int main(int argc, char *argv[])
{

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    //Fnadales: Needed to check OpenGLExtensions on main Thread
    OpenGLInterop::initialize();

    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<VideoSource>("VideoHW", 0, 1, "VideoSource", "C++ Created");
    qmlRegisterSingletonType<FileProcessor>("VideoHW", 0, 1, "FileProcessor", FileProcessorInstance);
    qmlRegisterType<VideoFBORenderer>("VideoHW", 0, 1, "VideoRenderer");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
