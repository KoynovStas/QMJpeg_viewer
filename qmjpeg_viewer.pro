TEMPLATE = app


QT  += core
QT  += gui
QT  += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



CONFIG += qt
CONFIG += debug_and_release  build_all



CONFIG(debug, debug|release) {
    TARGETDIR = debug
    DESTDIR   = debug
}



CONFIG(release, debug|release) {
    TARGETDIR = release
    DESTDIR   = release
}



# Input
SOURCES  += main.cpp \
            mainwindow.cpp \
            qmjpeg_viewer.cpp



HEADERS  += mainwindow.h \
            qmjpeg_viewer.h \
    mainwindow.h



FORMS    += mainwindow.ui
