QT       += core gui multimedia multimediawidgets
QT += widgets network webenginewidgets
QT += multimedia
QT += quick quick3d
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport quickwidgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc \
    Resources.qrc

DISTFILES +=


