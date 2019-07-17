# Addle source code
# Copyright 2019 Eleanor Hawk
# Modification and distribution permitted under the terms of the MIT License.
# See "LICENSE" for full details.


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
        src/servicelocator.h \
        src/models/document.h \
        src/models/layer.h \
        src/presenters/canvaspresenter.h \
        src/presenters/editorpresenter.h \
        src/presenters/editorviewportpresenter.h \
        src/presenters/helpers/zoomrotatehelper.h \
        src/services/fileservice.h \
        src/views/editorviewport.h \
        src/views/mainwindow.h \
        src/views/graphicsitems/canvasitem.h \
        src/views/zoomslider.h \
        src/utilities/registerqmetatypes.h


SOURCES += \
        src/servicelocator.cpp \
        src/addle/addle.cpp \
        src/addle/serviceconfiguration.cpp \
        src/models/document.cpp \
        src/models/layer.cpp \
        src/presenters/canvaspresenter.cpp \
        src/presenters/editorpresenter.cpp \
        src/presenters/editorviewportpresenter.cpp \
        src/presenters/helpers/zoomrotatehelper.cpp \
        src/services/fileservice.cpp \
        src/views/editorviewport.cpp \
        src/views/mainwindow.cpp \
        src/views/zoomslider.cpp \
        src/views/graphicsitems/canvasitem.cpp \
        src/utilities/registerqmetatypes.cpp

FORMS += \
        resources/forms/mainwindow.ui \
        resources/forms/mainwindow_zoomwidget.ui \
        resources/forms/layers.ui \
        resources/forms/navigator.ui \
        resources/forms/tooloptions_brush.ui \
        resources/forms/tooloptions_eraser.ui \
        resources/forms/tooloptions_eyedrop.ui \
        resources/forms/tooloptions_fill.ui \
        resources/forms/tooloptions_measure.ui \
        resources/forms/tooloptions_navigate.ui \
        resources/forms/tooloptions_select.ui \
        resources/forms/tooloptions_shapes.ui \
        resources/forms/tooloptions_stickers.ui \
        resources/forms/tooloptions_text.ui \
        resources/forms/tool_sizeselector.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
