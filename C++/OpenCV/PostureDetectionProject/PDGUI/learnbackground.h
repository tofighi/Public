#ifndef LEARNBACKGROUND_H
#define LEARNBACKGROUND_H

#include <QDialog>
#include <QDebug>
#include <QDebug>
#include <QTimer>
#include <QCloseEvent>
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include "helper.h"
#include "guihelper.h"

namespace Ui {
    class LearnBackground;
}

class LearnBackground : public QDialog
{
    Q_OBJECT

public:
    explicit LearnBackground(QWidget *parent = 0);
    ~LearnBackground();
    void getFG(IplImage* &img);
    IplImage *foreground;

private:
    Ui::LearnBackground *ui;
    int timerID;

    bool cbModelCreated;
    CvBGCodeBookModel* bgCodeBookModel;
    int nFrames;
    int nFramesToLearnBG;

    IplImage *imgMaskCb, *imgYuvCb;
    void learnBGCodeBookModel();
    void resetBGCodeBookModel();
    void prepareFg();

    QString infoText;
    bool learningMode;
    bool cameraIsOn;
    bool countDown;
    IplImage *frame; // current frame

    CvCapture *camera;
    QShortcut *shortcut_esc;
    QTimer *delayTimer;
    int delayTimerInterval;
    QTimer *elapsedTimer;
    int elapsedTimeInterval;
    double elapsedTime;
    QTimer *learnBGTimer;
    int learnBGTimerInterval;
    QImage qimgColorBar;
    void clearWidgets();
    void stopTheTimer(QTimer *theTimer);


signals:
        void startWebcamMW();
        void frameRequest();
        void fgRequest();
        void nrmRequest();
protected:
        //void timerEvent(QTimerEvent*);
        void closeEvent(QCloseEvent *event);

public slots:
    void setFrame(IplImage *img);
private slots:
    void closeLearnBG();
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    void on_btnStart_clicked();
    void startLearning();
    void delayTimeOut();
    void elapsedTimeOut();
};

#endif // LEARNBACKGROUND_H
