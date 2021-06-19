QT       += core gui network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    helppage.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    helppage.h \
    mainwindow.h

FORMS += \
    helppage.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -LD:/OpenCV/opencv_v4.5.2/newbuild/install/x64/mingw/lib/ -llibopencv_world452.dll
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/OpenCV/opencv_v4.5.2/newbuild/install/x64/mingw/lib/ -llibopencv_world452.dlld
else:unix: LIBS += -LD:/OpenCV/opencv_v4.5.2/newbuild/install/x64/mingw/lib/ -llibopencv_world452.dll

INCLUDEPATH += D:/OpenCV/opencv_v4.5.2/newbuild/install/include
DEPENDPATH += D:/OpenCV/opencv_v4.5.2/newbuild/install/include

RC_ICONS = baiduLogo.ico

RESOURCES += \
    resource.qrc
