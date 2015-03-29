#include "learnbackground.h"
#include "ui_learnbackground.h"

LearnBackground::LearnBackground(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LearnBackground)
{
    ui->setupUi(this);
    /* <Learn Background Shortcuts> */
    shortcut_esc = new QShortcut(QKeySequence("Escape"),this);
    connect(shortcut_esc,SIGNAL(activated()),this,SLOT( close() ));
    connect(shortcut_esc,SIGNAL(activated()),this,SLOT( closeLearnBG() ));
    /* </Learn Background Shortcuts> */

//    delayTimer = new QTimer(this);
//    learnBGTimer = new QTimer(this);
//    learnBGTimer->setInterval(25);
    bgCodeBookModel = 0;

    /* </Background Model> */
        cbModelCreated = false;
        cvReleaseBGCodeBookModel(&bgCodeBookModel);
        bgCodeBookModel = cvCreateBGCodeBookModel();
        //Set color thresholds to default values
        bgCodeBookModel->modMin[0] = 3;
        bgCodeBookModel->modMin[1] = bgCodeBookModel->modMin[2] = 3;
        bgCodeBookModel->modMax[0] = 10;
        bgCodeBookModel->modMax[1] = bgCodeBookModel->modMax[2] = 10;
        bgCodeBookModel->cbBounds[0] = bgCodeBookModel->cbBounds[1] = bgCodeBookModel->cbBounds[2] = 10;
    /* </Background Model> */

    frame = 0;
    foreground = 0;
    learningMode = false;
    cameraIsOn = false;
    countDown = false;

    delayTimerInterval = ui->spbDelayValue->value()*1000;
    elapsedTimeInterval = 1000;
    elapsedTime = (double)delayTimerInterval/(double)1000;
    learnBGTimerInterval = 30;

    nFramesToLearnBG = ui->spbNFramesToLearnBG->value();
    nFrames=0; // Number of processed frames for learning background
    ui->btnOk->setEnabled(false);

    /* <Main Window Widgets Configuration> */
    int initWidth = 400;
    int initHeight = 300;
    ui->srcImg->resize(initWidth,initHeight);
    ui->foreground->resize(initWidth,initHeight);
    infoText = QString(tr("Click Start button and leave the front of camera in %1 seconds.")).arg(QString::number(elapsedTime));
    ui->info->setText(infoText);
    qimgColorBar.load(":/images/resources/images/colorbar.png");
    clearWidgets();
    /* </Main Window Widgets Configuration> */
}


LearnBackground::~LearnBackground()
{
    delete ui;
}

void LearnBackground::closeEvent(QCloseEvent *event)
{
    closeLearnBG();
    event->accept();
}

void LearnBackground::getFG(IplImage* &img)
{
    img = foreground;
}


void LearnBackground::on_btnStart_clicked()
{
    ui->btnStart->setEnabled(false);
    ui->btnStart->setText(tr("Learning"));
    delayTimerInterval = ui->spbDelayValue->value()*1000;
    nFramesToLearnBG = ui->spbNFramesToLearnBG->value();
    ui->spbNFramesToLearnBG->setEnabled(false);
    ui->spbDelayValue->setEnabled(false);

    if (!learningMode)
    {
        if (!cameraIsOn){
            emit startWebcamMW();
            cameraIsOn = true;
            learnBGTimer = new QTimer(this);
            learnBGTimer->setInterval(learnBGTimerInterval);
            learnBGTimer->start();// Running startLearning every 25ms
            connect(learnBGTimer, SIGNAL(timeout()), this, SLOT(startLearning()));
            qDebug()<<"learnBGTimer started" ;

        }
        ui->learnBgProgressBar->setValue(0);
        elapsedTime = (double)delayTimerInterval/(double)1000;
        infoText = QString(tr("Learning will start in %1 seconds.")).arg(QString::number(elapsedTime));
        ui->info->setText(infoText);
        resetBGCodeBookModel();
        learningMode = true;
        if(!countDown){
            countDown = true;
            elapsedTimer = new QTimer(this); // for showing countdown on start button
            elapsedTimer->setInterval(elapsedTimeInterval);
            elapsedTimer->start();
            connect(elapsedTimer, SIGNAL(timeout()), this, SLOT(elapsedTimeOut()));
            qDebug() << "elapsedTimer Start" ;
        }else {
            elapsedTimer->start();
            qDebug() << "elapsedTimer Start" ;
        }

        delayTimer = new QTimer(this);
        delayTimer->setInterval(delayTimerInterval);
        delayTimer->setSingleShot(true);
        delayTimer->start(); //Wait for delayTimer which emits timeout() signal after its time out
        connect(delayTimer, SIGNAL(timeout()), this, SLOT(delayTimeOut()));
        qDebug() << "delayTimer Start" ;

    }

}

void LearnBackground::on_btnOk_clicked()
{
    //closeLearnBG();
    ui->btnStart->setFocus();
    this->window()->hide();
}

void LearnBackground::on_btnCancel_clicked()
{
    closeLearnBG();
}


void LearnBackground::setFrame(IplImage *img) {
    frame = img;
}


void LearnBackground::clearWidgets()
{
    qimageWidget(tr("No Image"),qimgColorBar,ui->srcImg);
    qimageWidget(tr("No Image"),qimgColorBar,ui->foreground);

}

void LearnBackground::delayTimeOut(){
    // Signal starting webcam if it is not started, implementation in Main Window

    qDebug() << "delayTimeOut";
    delayTimer->stop();
    elapsedTimer->stop();
    elapsedTime = 0;
    infoText = QString(tr("Learning...(Elapsed Time: %1 seconds)")).arg(QString::number(elapsedTime));
    ui->info->setText(infoText);
    ui->btnStart->setText(QString(tr("Learning: %1")).arg(QString::number(elapsedTime)));
    ui->spbNFramesToLearnBG->setEnabled(true);
    ui->spbDelayValue->setEnabled(true);
    elapsedTimer->start();
}

void LearnBackground::elapsedTimeOut(){
    //qDebug() << "Beep!" ;
    QString btnStartText;
    if (ui->learnBgProgressBar->value()==100){
        ui->btnStart->setText(tr("Restart"));
        infoText = QString(tr("Learning is done in %1 seconds.")).arg(QString::number(elapsedTime));
        ui->info->setText(infoText);
        elapsedTime = (double)delayTimerInterval/(double)1000;
        elapsedTimer->stop();
        qDebug() << "elapsedTimer stopped";
        ui->srcImg->imagelabel->setStyleSheet("border-style: hidden; border-width: 3px;");

    }
    else {
        if(delayTimer->isActive())
        {
            elapsedTime -= (double)elapsedTimeInterval/(double)1000;
            btnStartText = QString::number(elapsedTime);
            ui->btnStart->setText(btnStartText);
            infoText = QString(tr("Learning will start in %1 seconds.")).arg(QString::number(elapsedTime));
            ui->info->setText(infoText);
        }else{
            elapsedTime += (double)elapsedTimeInterval/(double)1000;
            btnStartText= QString(tr("Learning: %1")).arg(QString::number(elapsedTime));
            ui->btnStart->setText(btnStartText);
            infoText = QString(tr("Learning...(Elapsed Time: %1 seconds)")).arg(QString::number(elapsedTime));
            ui->info->setText(infoText);
            int toggle = int(elapsedTime*(double)1000/(double)elapsedTimeInterval)%2;
            if (toggle){
                ui->srcImg->imagelabel->setStyleSheet("border-style: solid; border-width: 3px; border-color: red;");
            }else{
                ui->srcImg->imagelabel->setStyleSheet("border-style: hidden; border-width: 3px;");
            }
        }
    }

}

void LearnBackground::startLearning(){
        if(learningMode){
            if (delayTimer->isActive()){
                emit frameRequest();

            }else{
                if (nFrames <nFramesToLearnBG)
                {
                    emit frameRequest();

                    nFrames++;

                    // Background Learning
                    if (nFrames==1){
                        imgYuvCb = cvCloneImage(frame);
                        imgMaskCb = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
                        cvSet(imgMaskCb,cvScalar(255));

                    }
                    learnBGCodeBookModel();


                    if (nFrames%10==0)
                        //qDebug()<< "nFrames" << nFrames;
                    ui->btnOk->setEnabled(false);
                    ui->learnBgProgressBar->setValue(100*nFrames/nFramesToLearnBG);
                }
                else
                {
                    // Find foreground by codebook method
                    learningMode = false;
                    nFrames= 0;
                    ui->btnStart->setText(tr("Restart"));
                    ui->btnStart->setEnabled(true);
                    ui->btnOk->setEnabled(true);
                    ui->btnOk->setFocus();
                }
            }
        }
        else
        {
            emit frameRequest();

            if (cbModelCreated) {
                prepareFg();

            }
        }
    imageWidget(tr("Source Image"),frame,ui->srcImg);

}

void LearnBackground::learnBGCodeBookModel(){
    //YUV For codebook method
    cvCvtColor( frame, imgYuvCb, CV_BGR2YCrCb );
    //This is where we build our background model
    if(nFrames < nFramesToLearnBG  ) {
        cvBGCodeBookUpdate( bgCodeBookModel, imgYuvCb );
        }
    if( nFrames == nFramesToLearnBG  ){
        cvBGCodeBookClearStale( bgCodeBookModel, bgCodeBookModel->t/2 );
        cbModelCreated = true;
    }

}

void LearnBackground::prepareFg(){
    // This part just to visualize bounding boxes and centers if desired
    int smoothValue = ui->spbSmoothValue->value();
    if (!(smoothValue%2)){
        --smoothValue;
        ui->spbSmoothValue->setValue(smoothValue);
    }
    cvCvtColor( frame, imgYuvCb, CV_BGR2YCrCb );
    cvBGCodeBookDiff( bgCodeBookModel, imgYuvCb, imgMaskCb );

    cvErode( imgMaskCb, imgMaskCb, NULL, ui->spbErosionValue->value());
    cvDilate( imgMaskCb, imgMaskCb, NULL, ui->spbDilationValue->value());
    //cvSmooth( imgMaskCb, imgMaskCb, CV_GAUSSIAN, smoothValue, smoothValue );
    if (ui->cbSimpleBG->isChecked()){
        IplImage* imgMaskCbThreshold = mythreshold(imgMaskCb,ui->spbThresholdValue->value(),0);
        foreground = imgMaskCbThreshold;
    }else{
        IplImage* imgMaskCbCC = SizeResize(imgMaskCb,256);
        cvSegmentFGMask( imgMaskCbCC );
        int size = (frame->width > frame->height)? frame->width:frame->height;
        IplImage* imgMaskCbCCResized = SizeResize(imgMaskCbCC,size);
        cvSmooth( imgMaskCbCCResized, imgMaskCbCCResized, CV_GAUSSIAN, smoothValue, smoothValue );
        IplImage* imgMaskCbCCResizedTh = mythreshold(imgMaskCbCCResized,ui->spbThresholdValue->value(),0);
        foreground = imgMaskCbCCResizedTh;
        cvReleaseImage(&imgMaskCbCC);
        cvReleaseImage(&imgMaskCbCCResized);
    }

    imageWidget(tr("Foreground"),foreground,ui->foreground);
    emit fgRequest();
    resetImage(foreground);
}

void LearnBackground::resetBGCodeBookModel(){
/* </Background Model> */
cbModelCreated = false;
cvReleaseBGCodeBookModel(&bgCodeBookModel);
bgCodeBookModel = cvCreateBGCodeBookModel();
//Set color thresholds to default values
bgCodeBookModel->modMin[0] = 3;
bgCodeBookModel->modMin[1] = bgCodeBookModel->modMin[2] = 3;
bgCodeBookModel->modMax[0] = 10;
bgCodeBookModel->modMax[1] = bgCodeBookModel->modMax[2] = 10;
bgCodeBookModel->cbBounds[0] = bgCodeBookModel->cbBounds[1] = bgCodeBookModel->cbBounds[2] = 10;
/* </Background Model> */
}

void LearnBackground::closeLearnBG(){

    if (cameraIsOn){
        learnBGTimer->stop();
        cameraIsOn = false;
        qDebug() << "camera was on and I stopped the learnBGTimer";
    }

    if (learningMode){
        if (delayTimer->isActive()){
         delayTimer->stop();
         qDebug() << "delayTimer was Active and I stopped the delayTimer";
        }
        learningMode = false;
    }
    if (countDown){
        elapsedTimer->stop();
        elapsedTime = (double)delayTimerInterval/(double)1000;
        qDebug() << "Countdown was Active and I stopped the elapsedTimer";
    }
    infoText = QString(tr("Click Start button and leave the front of camera in %1 seconds.")).arg(QString::number(elapsedTime));
    ui->info->setText(infoText);
    nFrames= 0;
    clearWidgets();
    ui->btnStart->setText(tr("Start"));
    ui->btnStart->setEnabled(true);
    ui->btnOk->setEnabled(false);
    ui->learnBgProgressBar->setValue(0);
    cvReleaseImage(&foreground);
}


