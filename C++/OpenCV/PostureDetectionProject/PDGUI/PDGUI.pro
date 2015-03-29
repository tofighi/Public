#-------------------------------------------------
#
# Project created by QtCreator 2011-07-12T19:10:22
#
#-------------------------------------------------
QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = PDGUI
TEMPLATE = app

# OpenCV
# Note: to be changed according to your OpenCV configuration
#INCLUDEPATH += /usr/local/opencv2/

OPENCV_PATH = /usr/local/Cellar/opencv/2.4.10.1/
#INCLUDEPATH += $$OPENCV_PATH/include/core \ #core module
#    $$OPENCV_PATH/include/highgui \ #highgui module
#    $$OPENCV_PATH/include/objdetect #obj_detect module


INCLUDEPATH += .
INCLUDEPATH += $$OPENCV_PATH/include
INCLUDEPATH += $$OPENCV_PATH/include/opencv
INCLUDEPATH += $$OPENCV_PATH/include/opencv2
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/core
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/highgui
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/imgproc
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/flann
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/photo
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/video
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/features2d
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/objdetect
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/calib3d
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/ml
INCLUDEPATH += $$OPENCV_PATH/include/opencv2/contrib

LIBS += -L$$OPENCV_PATH/lib

LIBS += -lopencv_calib3d \
-lopencv_contrib \
-lopencv_core \
-lopencv_features2d \
-lopencv_flann \
-lopencv_gpu \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_legacy \
-lopencv_ml \
-lopencv_objdetect \
-lopencv_video

SOURCES += main.cpp\
        mainwindow.cpp\
        iplimagewidget.cpp\
        posturedetection.cpp\
        helper.cpp \
        contours.cpp \
        cblob.cpp \
        options.cpp \
        learnbackground.cpp \
    guihelper.cpp \
    learnpostures.cpp



HEADERS  += mainwindow.h\
            iplimagewidget.h\
            posturedetection.h \
            helper.h \
            contours.h \
            cblob.h \
            options.h \
            learnbackground.h \
    guihelper.h \
    learnpostures.h


FORMS    += mainwindow.ui \
            options.ui \
            learnbackground.ui \
    about.ui \
    learnpostures.ui

RESOURCES += PDGUI.qrc

win32 {
DESTDIR       = ./
RC_FILE       = PDGUI.rc
OBJECTS_DIR   = .build.win32/
MOC_DIR       = .build.win32/
RCC_DIR       = .build.win32/
}
unix {
!macx {
OBJECTS_DIR   = .build.unix/
MOC_DIR       = .build.unix/
RCC_DIR       = .build.unix/
}
}
macx {
ICON          = resources/icons/vsign.icns
#CONFIG       += x86 ppc
#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
#QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
}

message("OpenCV path: $$OPENCV_PATH")
message("Includes path: $$INCLUDEPATH")
message("Libraries: $$LIBS")
