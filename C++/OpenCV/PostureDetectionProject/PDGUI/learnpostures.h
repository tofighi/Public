#ifndef LEARNPOSTURES_H
#define LEARNPOSTURES_H

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QVector>
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include <ml.h>
#include "helper.h"
#include "guihelper.h"


namespace Ui {
    class LearnPostures;
}

class LearnPostures : public QDialog
{
    Q_OBJECT

public:
    explicit LearnPostures(QWidget *parent = 0);
    ~LearnPostures();

    float knnResult;
    float svmResult;
    float knnPCAResult;
    float svmPCAResult;
    float knnError;
    float svmError;
    float knnPCAError;
    float svmPCAError;

    IplImage *imgNrmPP;

    void classify(IplImage *img);
    void getNRM(IplImage* &img);

signals:
        void nrmRequest();



private:
    Ui::LearnPostures *ui;

    QStringList imgFileFilters;
    QString testPath;
    QString trainPath;
    //QDir dirTrain;
    //QDir dirTest;
    QString *trainDirList, *testDirList;
    int *nTrainImgs, *nTestImgs; // Number of images in each train or test directory
    int nTrainDirs, nTestDirs;
    void listTrainDirs();
    void listTestDirs();
    void listDirs(QString dirPath,QString* &dirList, int &nDirs);
    void listImages(QString* dirList,int nDirs,int* &nDirImgs, QVector< QVector<QString> > &dirImages, int &nAllImages);
    QVector< QVector<QString> > trainImages;
    QVector< QVector<QString> > testImages;

    int nAllTrainImages;
    int nAllTestImages;

    int imgWidth, imgHeight;
    CvMat* trainData;
    CvMat* trainClasses;
    CvMat* trainPCAData;

    //Classifiers
    CvKNearest *knn;
    CvSVM *svm;
    CvKNearest *knnPCA;
    CvSVM *svmPCA;
    static const int K=5;
    double paramsC;
    double paramsGamma;
    QString svmKernel;


    void loadImages();
    void train();
    void test();

    //PCA
    int nEigens;
    IplImage **TrainImgArr;
    IplImage *pAvgTrainImg; // the average image
    IplImage ** eigenVectArr; // eigenvectors
    void doPCA();


private slots:
    void on_btnOk_clicked();
    void on_btnStart_clicked();
    void on_btnSelectTestFolder_clicked();
    void on_btnSelectTrainFolder_clicked();
};

#endif // LEARNPOSTURES_H
