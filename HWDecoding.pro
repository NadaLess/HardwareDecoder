QT += quick concurrent gui
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    hwdecoder.cpp \
    hwdecoderfactory.cpp \
    videorenderer.cpp \
    framerenderer.cpp \
    videosource.cpp \
    videoframe.cpp \
    surface.cpp \
    yuv2rgb.cpp

win32: SOURCES += d3d9decoder.cpp \
                  d3d9interop.cpp \
                  surfaced3d9.cpp

linux-g++ : SOURCES += vaapidecoder.cpp \
                       surfacevaapi.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    hwdecoder.h \
    hwdecoderfactory.h \
    videorenderer.h \
    framerenderer.h \
    videosource.h \
    videoframe.h \
    surface.h \
    yuv2rgb.h

win32: HEADERS += d3d9decoder.h \
                  d3d9interop.h \
                  surfaced3d9.h

linux-g++: HEADERS += vaapidecoder.h \
                      surfacevaapi.h

#Link with FFmpeg installed in Qt
LIBS += -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale

#Link with DX libs (Windows)
win32: LIBS += -ldxgi -ldxva2 -ld3d9 -ld3d11

#Link with libva libs (LINUX)
linux-g++: LIBS += -lX11 -lva -lva-glx -lva-x11 -lGLU
