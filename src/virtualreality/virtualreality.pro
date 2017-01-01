TARGET   = Qt3DVirtualReality

TEMPLATE = lib

DEFINES += QT3DVR_LIBRARY

#MODULE   = virtualreality
QT      += qml quick \
           core core-private 3dcore 3dcore-private 3drender 3drender-private 3dinput 3dlogic 3dquick \
           qml qml-private 3dquick 3drender 3drender-private 3dlogic

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

#load(qt_module)

CONFIG += c++11

SOURCES += \
    backends/ovr/vrapiovr.cpp \
    qvirtualrealityapi.cpp \
    qheadmounteddisplay.cpp \
    qvrhmdview.cpp

HEADERS += \
    backends/ovr/vrapiovr.h \
    qvirtualrealityapi.h \
    qvirtualrealityapi_p.h \
    qvirtualrealityapibackend.h \
    qheadmounteddisplay.h \
    qvrhmdview.h \
    qt3dvr_global.h
