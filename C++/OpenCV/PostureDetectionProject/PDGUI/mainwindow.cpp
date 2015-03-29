#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cameraTimerId = 0; // helps to identify webcam is on or off
    frame = 0;
    foreground = 0;
    nrmImg = 0;
    fgClone = 0;
    dlgLearnBg = new LearnBackground(this);
    dlgLearnPostures = new LearnPostures(this);

    connect(dlgLearnBg, SIGNAL(startWebcamMW()), this, SLOT(startWebcam()));
    connect(dlgLearnBg, SIGNAL(frameRequest()), this, SLOT(setLearnBGFrame()));
    connect(dlgLearnBg, SIGNAL(fgRequest()), this, SLOT(setFG()));
    connect(dlgLearnPostures, SIGNAL(nrmRequest()), this, SLOT(setNRM()));

    /* <Main Window Shortcuts> */
    shortcut_esc = new QShortcut(QKeySequence("Escape"),this);
    connect(shortcut_esc,SIGNAL(activated ()),this,SLOT( exitFullScreen() ));

    shortcut_ctrl_f = new QShortcut(QKeySequence("Ctrl+F"),this);
    connect(shortcut_ctrl_f,SIGNAL(activated ()),this,SLOT( toggleFullScreen() ));

    shortcut_ctrl_w = new QShortcut(QKeySequence("Ctrl+W"),this);
    connect(shortcut_ctrl_w,SIGNAL(activated ()),this,SLOT( toggleWebcam() ));
    /* </Main Window Shortcuts> */


    /* <Main Window Widgets Configuration> */
    int initWidth = 256;
    int initHeight = 256;
    ui->widget00->resize(initWidth,initHeight);
    ui->widget01->resize(initWidth,initHeight);
    ui->widget11->resize(initWidth,initHeight);
    ui->widget10->resize(initWidth,initHeight);
    qimgColorBar.load(":/images/resources/images/colorbar.png");
    clearWidgets();
    /* </Main Window Widgets Configuration> */
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearWidgets()
{
    qimageWidget(tr("No Image"),qimgColorBar,ui->widget00);
    qimageWidget(tr("No Image"),qimgColorBar,ui->widget01);
    qimageWidget(tr("No Image"),qimgColorBar,ui->widget10);
    qimageWidget(tr("No Image"),qimgColorBar,ui->widget11);
}


void MainWindow::timerEvent(QTimerEvent*) {
    frame = cvQueryFrame(camera);
    cvFlip(frame,frame,1);
    imageWidget("Source Image",frame,ui->widget00);
    //dlgLearnBg->getFG(foreground);
    //imageWidget("Foreground",foreground,ui->widget01);
    //imageWidget("Source Image",frame,ui->widget10);
    //imageWidget("Source Image",frame,ui->widget11);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (cameraTimerId){
        killTimer(cameraTimerId);
        cameraTimerId = 0;
        cvReleaseCapture(&camera);
    }
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    if (!cameraTimerId){
        clearWidgets();
    }
    event->accept();
}
/*
void MainWindow::adjustWidgetSize(){
    IplImage* img = cvQueryFrame(camera);
    int frameMaxSize = (img->width>img->height)? img->width:img->height;
    int minWidth = int(double(128)*double(img->width)/double(frameMaxSize));
    int minHeight = int(double(128)*double(img->height)/double(frameMaxSize));
    ui->widget00->setMinimumWidth(minWidth);
    ui->widget00->setMinimumHeight(minHeight);
    ui->widget01->setMinimumWidth(minWidth);
    ui->widget01->setMinimumHeight(minHeight);
    ui->widget10->setMinimumWidth(minWidth);
    ui->widget10->setMinimumHeight(minHeight);
    ui->widget11->setMinimumWidth(minWidth);
    ui->widget11->setMinimumHeight(minHeight);
}
*/
void MainWindow::toggleFullScreen()
{

    if (this->isFullScreen()){
        ui->actionFullScreen->setChecked(false);
        this->showNormal();
    } else {
        ui->actionFullScreen->setChecked(true);
        this->showFullScreen();
    }

}

void MainWindow::toggleWebcam(){
    if(!cameraTimerId) {
        ui->actionToggleWebcam->setChecked(true);
        camera = cvCreateCameraCapture(0);
        //adjustWidgetSize(); // Adjusting minimun width and height of a widget
        cameraTimerId = startTimer(1000/30);  // supposed 30 frames per second
    } else {
        ui->actionToggleWebcam->setChecked(false);
        killTimer(cameraTimerId);
        cameraTimerId = 0;
        clearWidgets();
        cvReleaseCapture(&camera);
    }
}

void MainWindow::stopWebcam(){
    if(cameraTimerId) {
        ui->actionToggleWebcam->setChecked(false);
        killTimer(cameraTimerId);
        cameraTimerId = 0;
        clearWidgets();
        cvReleaseCapture(&camera);
    }
}

void MainWindow::startWebcam(){
    if(!cameraTimerId) {
        ui->actionToggleWebcam->setChecked(true);
        camera = cvCreateCameraCapture(0);
        //adjustWidgetSize(); // Adjusting minimun width and height of a widget
        cameraTimerId = startTimer(1000/30);  // supposed 30 frames per second
    }
}

void MainWindow::exitFullScreen(){
    if (this->isFullScreen()){
        ui->actionFullScreen->setChecked(false);
        this->showMaximized();
    }else if (this->isMaximized()){
        this->showNormal();
    }
}
void MainWindow::sayHello(){
    qDebug() << "Hello" ;
}

void MainWindow::saveSettings(){
    QSettings settings("Simasoft", "GreenPostures");
}

void MainWindow::setLearnBGFrame(){
    dlgLearnBg->setFrame(frame);
}

void MainWindow::setNRM(){
    dlgLearnPostures->getNRM(nrmImg);
    //cvShowImage("nrmImg",nrmImg);
    imageWidget("Normalized Image",nrmImg,ui->widget10);
    if ((!pyscript.state())&&((dlgLearnPostures->svmResult==4)||(dlgLearnPostures->svmResult==5)))
    pyscript.start("C:\\Programs\\Programming\\Python266\\python.exe C:\\Programs\\Programming\\Python266\\SendKeys-0.3\\examples\\firefox.py");
}

void MainWindow::setFG(){
    dlgLearnBg->getFG(foreground);
    imageWidget("Foreground",foreground,ui->widget01);
    //cvShowImage("foreground111",foreground);
    fgClone = cvCloneImage(foreground);
    trimImage(fgClone,1);
    if (fgClone){
        cvFlip(fgClone,fgClone,1);
        dlgLearnPostures->classify(fgClone);
        qDebug() << "svmResult= " << dlgLearnPostures->svmResult;
    }
    resetImage(fgClone);
}

void MainWindow::openOptions(){
    dlgOptions.setFixedSize(dlgOptions.size());
    dlgOptions.exec();
}

void MainWindow::openLearnSkin(){
    stopWebcam();
    //dlgLearnSkin.setFixedSize(dlgLearnSkin.size());
    //dlgLearnSkin.exec();

}
void MainWindow::openLearnBg(){
    //stopWebcam();
//    if(!cameraTimerId){
//        toggleWebcam();
//    }
    dlgLearnBg->setFixedSize(dlgLearnBg->size());
    //dlgLearnBg->startWebcam();
    dlgLearnBg->exec();
}

void MainWindow::openLearnPostures(){
    //dlgLearnPostures = new LearnPostures(this);
    dlgLearnPostures->setFixedSize(dlgLearnPostures->size());
    dlgLearnPostures->exec();
}

void MainWindow::openAbout()
{
    Ui::About uiAbout;
    QDialog dlgAbout;
    uiAbout.setupUi(&dlgAbout);
    dlgAbout.setFixedSize(dlgAbout.size());
    dlgAbout.exec();
}
