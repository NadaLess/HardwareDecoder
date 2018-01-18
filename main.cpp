#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtAV/ZeroCopyChecker.h>
#include "hwdecoderfactory.h"

int main(int argc, char *argv[])
{

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QtAV::ZeroCopyChecker::instance();

    QQmlApplicationEngine engine;
    qmlRegisterSingletonType<HWDecoder>("Video", 0, 1, "HWDecoder", hwDecoderInstance);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
