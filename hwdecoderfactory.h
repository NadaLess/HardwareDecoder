#ifndef HWDECODERFACTORY_H
#define HWDECODERFACTORY_H

#include <QQmlEngine>
#include "hwdecoder.h"

class HWDecoderFactory
{
public:
    static HWDecoder *createDecoder(QObject * parent = nullptr);

};

static QObject *hwDecoderInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return HWDecoderFactory::createDecoder();
}
#endif // HWDECODERFACTORY_H
