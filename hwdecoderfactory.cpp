#include "hwdecoderfactory.h"

#if defined(Q_OS_WIN)
#include "d3d9decoder.h"
#endif

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include "vaapidecoder.h"
#endif

HWDecoder *HWDecoderFactory::createDecoder(QObject *parent)
{
#if defined(Q_OS_WIN)
    return new D3D9Decoder(parent);
#endif

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    return new VAAPIDecoder(parent);
#endif
}
