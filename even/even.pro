#-------------------------------------------------
#
# Project created by QtCreator 2013-10-09T20:48:04
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Even
TEMPLATE = app

macx: ICON = $$PWD/../extra/even.icns
win32: RC_ICONS = $$PWD/../extra/even.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    movingsplitter.cpp \
    baseedit.cpp \
    viewerwidget.cpp \
    pythonedit.cpp \
    findwidget.cpp \
    gowidget.cpp \
    lineedit.cpp \
    activitywidget.cpp \
    hidinglabel.cpp \
    pythonhighlighter.cpp \
    pageitem.cpp

HEADERS  += mainwindow.h \
    movingsplitter.h \
    baseedit.h \
    viewerwidget.h \
    pythonedit.h \
    findwidget.h \
    gowidget.h \
    lineedit.h \
    activitywidget.h \
    hidinglabel.h \
    pythonhighlighter.h \
    pageitem.h

INCLUDEPATH += $$PWD/../thirdparty/mupdf/include
DEPENDPATH += $$PWD/../thirdparty/mupdf/include

macx: LIBS += -L$$PWD/../thirdparty/mupdf/lib/mac/ -lmupdf -lmupdf-js-none -lfreetype -ljbig2dec -ljpeg -lopenjpeg -lz -lm
macx: PRE_TARGETDEPS += $$PWD/../thirdparty/mupdf/lib/mac/libmupdf.a

unix:!macx: LIBS += -L$$PWD/../thirdparty/mupdf/lib/linux/ -lmupdf -lmupdf-js-none -lfreetype -ljbig2dec -ljpeg -lopenjpeg -lz -lm
unix:!macx: PRE_TARGETDEPS += $$PWD/../thirdparty/mupdf/lib/linux/libmupdf.a

win32: LIBS += -L$$PWD/../thirdparty/mupdf/lib/win/ -lmupdf -lmupdf-js-none -lfreetype -ljbig2dec -ljpeg -lopenjpeg -lz -lm
win32: PRE_TARGETDEPS += $$PWD/../thirdparty/mupdf/lib/win/libmupdf.a

macx: {
    THIRDPARTY = $$OUT_PWD/$${TARGET}.app/Contents/Resources
} else {
    THIRDPARTY = $$OUT_PWD/thirdparty
}

win32: {
    QMAKE_POST_LINK += "cd $$shell_path($$PWD/../thirdparty/flat) && xcopy flat\\*.py $$shell_path($$THIRDPARTY/lib/flat/) /D /E"
} else {
    QMAKE_POST_LINK += $$quote(cd $$PWD/../thirdparty/flat && find ./flat -name '*.py' | cpio -pdm $$THIRDPARTY/lib;)
}

macx: QMAKE_POST_LINK += $$quote(unzip -n $$PWD/../thirdparty/pypy/pypy-2.1-osx64.zip -d $$THIRDPARTY;)
unix:!macx: QMAKE_POST_LINK += $$quote(unzip -n $$PWD/../thirdparty/pypy/pypy-2.1-linux64.zip -d $$THIRDPARTY;)
# win32: manually extract "pypy-2.1-win32.zip" next to Even.exe as Windows does not have unzip





