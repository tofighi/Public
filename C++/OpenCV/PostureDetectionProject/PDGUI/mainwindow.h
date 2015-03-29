#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <cv.h>
#include <highgui.h>
#include <QDebug>
#include <QDialog>
#include <QtGui>
#include <QDir>
#include <QCloseEvent>
#include "iplimagewidget.h"
#include "helper.h"
#include "guihelper.h"
#include "contours.h"
#include "cblob.h"
#include "posturedetection.h"
#include "options.h"
#include "learnbackground.h"
#include "learnpostures.h"
#include "ui_about.h"



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    //explicit MainWindow(CvCapture *cam, QWidget *parent=0);
    ~MainWindow();

    int widget_width;
    int widget_height;
    IplImage* background;
    IplImage* foreground;
    IplImage* nrmImg;
    IplImage* fgClone;
    PostureDetection* pd;
    FILE* fp;
    QProcess pyscript;



private:
    Options dlgOptions;
    LearnBackground *dlgLearnBg;
    LearnPostures *dlgLearnPostures;
    Ui::MainWindow *ui;

    QImage qimgColorBar;
    IplImageWidget *widget00,*widget01,*widget02,*widget03, \
                   *widget10,*widget11,*widget12,*widget13, \
                   *widget20,*widget21,*widget22,*widget23;


    CvBGCodeBookModel* codeBookModelBG;
    void clearWidgets();
//    void adjustWidgetSize();
    void saveSettings();
    void stopWebcam();
    CvCapture *camera;
    IplImage* frame;
    int cameraTimerId;

    QShortcut *shortcut_esc;
    QShortcut *shortcut_ctrl_f;
    QShortcut *shortcut_ctrl_w;

public slots:
    void toggleFullScreen();
    void setLearnBGFrame();
    void setFG();
    void setNRM();
    void toggleWebcam();
    void startWebcam();
    void openOptions();
    void openLearnPostures();
    void openLearnBg();
    void openLearnSkin();
    void openAbout();
    void exitFullScreen();
    void sayHello();

//    void loadFile(const QString &fileName);
//    void openVideo();
//    void recordInput();
//    void recordOutput();
//    void pauze();
//    void play();
//    void changePosition();
//    void record();
//    void heartBeat();
//    void fullscreen();
//    void setSliderPosition(double position);
//    void about();

protected:
    void timerEvent(QTimerEvent*);
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * event);
};

#endif // MAINWINDOW_H
