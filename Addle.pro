QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Addle
TEMPLATE = app

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = resources/forms/headers

INCLUDEPATH += \
        interfaces \
        resources \
        src

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

HEADERS += \
        src/configure/configureserviceregistry.h \
        src/configure/serviceregistry.h \
        src/drawing/drawingoperation.h \
        src/models/canvas.h \
        src/presenters/editorviewport.h \
        src/presenters/viewport.h \
        src/services/canvasservice.h \
        src/views/mainwindow.h

SOURCES += \
        src/configure/configureserviceregistry.cpp \
        src/configure/serviceregistry.cpp \
        src/models/canvas.cpp \
        src/presenters/editorviewport.cpp \
        src/views/mainwindow.cpp \
        src/services/canvasservice.cpp \
        src/main.cpp

FORMS += \
        resources/forms/mainwindow.ui

LIBS += -lsqlite3

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
