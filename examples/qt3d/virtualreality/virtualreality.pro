TARGET   = virtualreality

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
    vrbackends/ovr/virtualrealityapiovr.cpp \
    vrbackends/ovr/framebufferovr.cpp \
    vrbackends/openvr/virtualrealityapiopenvr.cpp \
    qvirtualrealityapi.cpp \
    qheadmounteddisplay.cpp \
    frontend/qvirtualrealityaspect.cpp \
    frontend/qvirtualrealitycamera.cpp \
    frontend/qvirtualrealitymesh.cpp \
    frontend/querytrackedobjectsjob.cpp \
    qvirtualrealitygeometry.cpp \
    qvirtualrealitymesh.cpp \
    handler.cpp \
    frontend/qtrackedtransform.cpp \
    trackedtransform.cpp

HEADERS += \
    vrbackends/ovr/virtualrealityapiovr.h \
    vrbackends/ovr/framebufferovr.h \
    vrbackends/openvr/virtualrealityapiopenvr.h \
    qvirtualrealityapi.h \
    qvirtualrealityapi_p.h \
    qvirtualrealityapibackend.h \
    qheadmounteddisplay.h \
    qt3dvr_global.h \
    frontend/qvirtualrealityaspect.h \
    frontend/qvirtualrealityaspect_p.h \
    frontend/qvirtualrealitycamera.h \
    frontend/qvirtualrealitymesh.h \
    frontend/querytrackedobjectsjob_p.h \
    qvirtualrealitygeometry.h \
    qvirtualrealitygeometry_p.h \
    qvirtualrealitymesh.h \
    handler_p.h \
    frontend/qtrackedtransform.h \
    trackedtransform_p.h

target.path = $$[QT_INSTALL_EXAMPLES]/qt3d/$$TARGET
INSTALLS += target




###### OVR ######

#TODO: find_package(LibOVR)
LIBS += -L"C:/develop/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015" -lLibOVR
LIBS += -L"E:/devel/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015" -lLibOVR

INCLUDEPATH += "C:/develop/OculusSDK/LibOVR/Include"
INCLUDEPATH += "E:/devel/OculusSDK/LibOVR/Include"

###### OpenVR ######

#TODO: find_package(LibOpenVR)
LIBS += -L"C:/develop/openvr/lib/win64" -lopenvr_api
LIBS += -L"F:/devel/openvr/lib/win64" -lopenvr_api
LIBS += -L"C:/develop/openvr/bin/win64" -lopenvr_api
LIBS += -L"F:/devel/openvr/bin/win64" -lopenvr_api

INCLUDEPATH += "C:/develop/openvr/headers"
INCLUDEPATH += "F:/devel/openvr/headers"

#if windows
LIBS += -ldxgi
#endif
