#ifndef HWDECODERFACTORY_H
#define HWDECODERFACTORY_H

#include <QQmlEngine>
#include "hwdecoder.h"

class HWDecoderFactory
{
public:
    static HWDecoder *createDecoder(QObject * parent = nullptr);

};

#endif // HWDECODERFACTORY_H
