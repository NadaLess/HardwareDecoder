#include "hwdecoderfactory.h"
#if defined(Q_OS_WIN)
#include "hwwindowsdecoder.h"
#endif

HWDecoder *HWDecoderFactory::createDecoder(QObject *parent)
{
#if defined(Q_OS_WIN)
    return new HWWindowsDecoder(parent);
#endif
}
