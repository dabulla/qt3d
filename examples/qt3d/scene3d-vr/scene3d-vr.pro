!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += qml quick 3dinput

CONFIG += console

SOURCES += main.cpp \
           window_multithreaded.cpp \
           cuberenderer.cpp \
    qvrcamera.cpp \
    qvrmanager.cpp \
    qvrrendertarget.cpp \
    vrrenderer.cpp

HEADERS += window_multithreaded.h \
           cuberenderer.h \
    qvrcamera.h \
    qvrmanager.h \
    qvrrendertarget.h \
    vrrenderer.h

RESOURCES += scene3d-vr.qrc

LIBS += -L"E:/devel/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015" -lLibOVR

#if windows
LIBS += -ldxgi
#endif

INCLUDEPATH += "E:/devel/OculusSDK/LibOVR/Include"
