!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += qml quick 3dinput


SOURCES += main.cpp \
           window_singlethreaded.cpp \
           window_multithreaded.cpp \
           cuberenderer.cpp

HEADERS += window_singlethreaded.h \
           window_multithreaded.h \
           cuberenderer.h

RESOURCES += scene3d-rendercontrol.qrc \
    ../exampleresources/cubemaps.qrc \
    ../exampleresources/obj.qrc
