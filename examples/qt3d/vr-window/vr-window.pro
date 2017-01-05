!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dinput 3dquick qml quick 3dquickextras

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    *.qml

RESOURCES += \
    vr.qrc

DEPENDPATH += . ../virtualreality
INCLUDEPATH += ../virtualreality

win32:CONFIG (release, debug|release): LIBS += -L$$PWD/../virtualreality/release/ -lvirtualreality
else:win32:CONFIG (debug, debug|release): LIBS += -L$$PWD/../virtualreality/debug/ -lvirtualreality
else:unix: LIBS += -L$$PWD/virtualreality/ -lvirtualrealityTODO
