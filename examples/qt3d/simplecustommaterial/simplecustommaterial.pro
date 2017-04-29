!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dinput 3dquick 3dlogic qml quick 3dquickextras
CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc \
    shaders.qrc

DISTFILES += \
    AreaBar.qml

