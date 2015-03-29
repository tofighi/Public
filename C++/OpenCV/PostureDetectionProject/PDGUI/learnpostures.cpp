#include "learnpostures.h"
#include "ui_learnpostures.h"

LearnPostures::LearnPostures(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LearnPostures)
{
    ui->setupUi(this);
    ui->btnStart->setEnabled(false);
    nAllTrainImages = 0;
    imgFileFilters << "*.jpg" << "*.bmp" << "*.png" << "*.gif";
    //image size for processing images
    imgWidth = 128;
    imgHeight = 128;
    paramsC = 100;
    paramsGamma = 1.0e-5;
    svmKernel = "LINEAR";

    imgNrmPP= 0;

    svmResult = -1;

//    trainPath = "";
//    testPath = "";
//    ui->edtTestFolder->setText(testPath);
//    ui->edtTrainFolder->setText(trainPath);
}

LearnPostures::~LearnPostures()
{
    delete ui;
}

void LearnPostures::on_btnSelectTrainFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Training Samples Directory"),ui->edtTrainFolder->text(),QFileDialog::ShowDirsOnly);
    if (dir!="")
    {
        ui->edtTrainFolder->setText(dir);
        trainPath = dir;
        if(testPath!="")
            ui->btnStart->setEnabled(true);
    }
}

void LearnPostures::on_btnSelectTestFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Testing Samples Directory"),ui->edtTestFolder->text(),QFileDialog::ShowDirsOnly);
    if (dir!="")
    {
        ui->edtTestFolder->setText(dir);
        testPath = dir;
        if(trainPath!="")
            ui->btnStart->setEnabled(true);
    }
}




void LearnPostures::on_btnStart_clicked()
{

    ui->btnStart->setEnabled(false);
    ui->btnSelectTrainFolder->setEnabled(false);
    ui->btnSelectTestFolder->setEnabled(false);

    //delete knn;
    //delete svm;
    //delete knnPCA;
    //delete svmPCA;
    knn = new CvKNearest();
    svm = new CvSVM();
    knnPCA = new CvKNearest();
    svmPCA = new CvSVM();

    qDebug() << "listImages...";
    listDirs(trainPath,trainDirList,nTrainDirs);
    listDirs(testPath,testDirList,nTestDirs);

    listImages(trainDirList,nTrainDirs,nTrainImgs,trainImages,nAllTrainImages);
    listImages(testDirList,nTestDirs,nTestImgs,testImages,nAllTestImages);
    qDebug() << "loadImages...";
    loadImages();
    qDebug() << "Performing PCA...";
    doPCA();
    qDebug() << "Training...";
    train();
    qDebug() << "Testing...";
    test();
    trainImages.clear();
    testImages.clear();
    ui->btnStart->setText(tr("Ready!"));
    ui->btnSelectTrainFolder->setEnabled(true);
    ui->btnSelectTestFolder->setEnabled(true);

                /*qDebug() << "nAllTrainImages: "  << nAllTrainImages;
                for (int i=0;i<nTrainDirs;i++)
                {
                    qDebug()<< i << nTrainImgs[i] << trainDirList[i] ;
                    for (int j=0;j<nTrainImgs[i];j++){
                        qDebug() << trainDirList[i]+"/"+trainImages.at(i).at(j);
                    }

                }

                listImages(testDirList,nTestDirs,nTestImgs,testImages,nAllTestImages);
                qDebug() << "nAllTestImages: "  << nAllTestImages;
                for (int i=0;i<nTestDirs;i++)
                {
                    qDebug()<< i << nTestImgs[i] << testDirList[i] ;
                    for (int j=0;j<nTestImgs[i];j++){
                        qDebug() << testDirList[i]+"/"+testImages.at(i).at(j);
                    }

                } */


}


void LearnPostures::listDirs(QString dirPath,QString* &dirList, int &nDirs)
{
    QDir dir(dirPath);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot |QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    //Number of Test Dirs
    nDirs = dir.entryInfoList().size();
    //List of Train Directories
    dirList = new QString[nDirs];
    for (int i = 0; i < nDirs; ++i) {
             dirList[i] = dir.entryInfoList().at(i).filePath();
             //qDebug() << "dirList["<<i<<"] =" << dirList[i];
     }
}

void LearnPostures::listImages(QString* dirList,int nDirs,int* &nDirImgs, QVector<QVector<QString> > &dirImages, int &nAllImages)
{

    dirImages.clear();
    nAllImages = 0;
    nDirImgs = new int[nDirs];
    QFileInfoList *listDirImgs = new QFileInfoList[nDirs];
    //QFileInfoList listDirImgs[nDirs];
    for (int i = 0; i < nDirs; ++i)
    {
        QDir dir(dirList[i]);
        dir.setFilter(QDir::Files |QDir::NoSymLinks);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(imgFileFilters);
        dir.entryInfoList();
        listDirImgs[i] = dir.entryInfoList();
        nDirImgs[i] = dir.entryInfoList().size();
        nAllImages += nDirImgs[i];
        QVector<QString> imgVect;
        for (int j = 0; j < nDirImgs[i]; ++j){
            QString imgName = listDirImgs[i].at(j).fileName();
            imgVect << imgName;
            //qDebug() << "img["<<i<<","<< j<<"] =" << imgName;
        }
        dirImages << imgVect;
    }
    delete [] listDirImgs;
}

void LearnPostures::loadImages(){

    trainData = cvCreateMat(nAllTrainImages, imgWidth*imgHeight, CV_32FC1);
    trainClasses = cvCreateMat(nAllTrainImages, 1, CV_32FC1);


    IplImage *trainImg = 0;
    IplImage *trainImgNrm = 0;
    IplImage *trainImgNrmPP = 0;
    //Create train image array
    TrainImgArr = (IplImage **)cvAlloc((nAllTrainImages)*sizeof(IplImage *) );
    int trainIdx = 0;
    CvMat row,*trainImgVect;
    for (int i=0; i<nTrainDirs; i++)
    {
        for (int j=0; j<nTrainImgs[i]; j++)
        {
            QString filePath = trainDirList[i]+"/"+trainImages.at(i).at(j);
            const char *file  = filePath.toAscii().data();
            trainImg = cvLoadImage(file,CV_LOAD_IMAGE_COLOR);
            if(!trainImg)
            {
                qDebug() << "Error: Cant load image"<< filePath;
                exit(-1);
            }
            trimImage(trainImg,3);
            trainImgNrm = normalizeImg(trainImg,imgWidth,imgHeight);
            trainImgNrmPP = preprocessing(trainImgNrm);
            if (trainImgNrmPP){
                TrainImgArr[trainIdx] = trainImgNrmPP;
                //Set class label
                cvGetRow(trainClasses, &row, trainIdx);
                cvSet(&row, cvRealScalar(i));
                //Set train vector
                cvGetRow(trainData, &row, trainIdx);
                trainImgVect = Image2Vector(trainImgNrmPP,0.0039215);//change range from 0-255 to 0-1 (1/255 = 0.0039215)
                cvCopy(trainImgVect, &row, NULL);
                cvReleaseMat(&trainImgVect);
                trainIdx++;
            }else{
                qDebug() << "Error: Train Image is empty!"<< filePath;
                exit(-1);
            }
            //Release memory
            resetImage(trainImg);
            resetImage(trainImgNrm);
            //resetImage(trainImgNrmPP);
        }

    }
}

void LearnPostures::train(){
    //Training the model with KNN
    knn->train(trainData, trainClasses, 0, false, K);
    knnPCA->train(trainPCAData, trainClasses, 0, false, K );


    //Setting up SVM parameters
    CvSVMParams params;
    params.svm_type=CvSVM::C_SVC; //SVM algorithm to use
    //SVM kernel form
    if (svmKernel == "LINEAR") params.kernel_type = CvSVM::LINEAR;
    if (svmKernel == "RBF") params.kernel_type = CvSVM::RBF;
    if (svmKernel == "SIGMOID") params.kernel_type = CvSVM::SIGMOID;

    params.C= paramsC;//C ratio for selecting support vectors
    params.gamma = paramsGamma; //Scale for polynomial, RBF or sigmoid kernel
    params.term_crit=cvTermCriteria(CV_TERMCRIT_ITER,100,0.000001);//Termination criteria in CvTermCriteria

    //Training the model with SVM
    svm->train(trainData,trainClasses,cv::Mat(),cv::Mat(),params);
    svmPCA->train(trainPCAData,trainClasses,cv::Mat(),cv::Mat(),params);

    //We don't need these matrix after training
    cvReleaseMat(&trainData);
    cvReleaseMat(&trainPCAData);
    cvReleaseMat(&trainClasses);
    cvFree(&TrainImgArr);
}

void LearnPostures::test(){
    int testIdx = 0;
    IplImage *testImg = 0;
    int svm_error_count=0;
    int knn_error_count=0;
    int svm_pca_error_count=0;
    int knn_pca_error_count=0;
    int testCount=0;
    int nKnn[10];
    int nKnnPCA[10];
    int nSVM[10];
    int nSVMPCA[10];
    for (int i=0; i<10; i++){
         nKnn[i] = 0;
         nKnnPCA[i] = 0;
         nSVM[i] = 0;
         nSVMPCA[i] = 0;
    }

    for (int i=0; i<nTestDirs; i++)
    {
        for (int j=0; j<nTestImgs[i]; j++)
        {
            QString filePath = testDirList[i]+"/"+testImages.at(i).at(j);
            const char *file  = filePath.toAscii().data();
            testImg = cvLoadImage(file,CV_LOAD_IMAGE_COLOR);
            if(!testImg)
            {
                qDebug() << "Error: Cant load image"<< filePath;
                exit(-1);
            }else{
                classify(testImg);
                if((int)svmResult!=i) {svm_error_count++; nSVM[i]++; /* qDebug()<< "SVM error image\t" << i << j << svm_result;*/}
                if((int)knnResult!=i) {knn_error_count++; nKnn[i]++;}
                if((int)svmPCAResult!=i) {svm_pca_error_count++; nSVMPCA[i]++; /*qDebug()<< "svm pca error image\t" << i << j << svmPCAResult;*/}
                if((int)knnPCAResult!=i) {knn_pca_error_count++; nKnnPCA[i]++;}
                testCount++;
                testIdx++;

            }
            resetImage(testImg);

         }
    }

    svmError= 100*(float)svm_error_count/(float)testCount;
    knnError= 100*(float)knn_error_count/(float)testCount;

    svmPCAError= 100*(float)svm_pca_error_count/(float)testCount;
    knnPCAError= 100*(float)knn_pca_error_count/(float)testCount;
    //printf("System Error: %.2f%%\n", totalerror);
    qDebug() << "Test Count:\t"<<testCount;
    qDebug() << "SVM Error:\t"<<svm_error_count<< "\t"<< svmError<<"%";
    qDebug() << "SVM PCA Error:\t"<<svm_pca_error_count << "\t"<< svmPCAError<<"%";

    qDebug() << "KNN Error:\t"<< knn_error_count<< "\t"<< knnError<<"%";
    qDebug() << "KNN PCA Error:\t"<<knn_pca_error_count <<"\t" << knnPCAError<<"%";

    qDebug() << "K Kernel\tGamma\tC" ;
    qDebug() << K << svmKernel << "\t" << paramsGamma <<"\t" << paramsC ;

    qDebug() << "Class" <<"\tnKnn" << "\tnKnnPCA" << "\tnSVM" <<  "\tnSVMPCA";
    for (int i=0; i<10; i++){
        qDebug() << i <<"\t" << nKnn[i]<< "\t" << nKnnPCA[i]<< "\t" << nSVM[i]<< "\t" << nSVMPCA[i];
    }

    qDebug() << "===========================================================" ;

}

void LearnPostures::getNRM(IplImage* &img)
{
    img = imgNrmPP;
}

void LearnPostures::classify(IplImage *img){
    CvMat* imgVect;
    CvMat* imgPCAVect;
    CvMat* nearest=cvCreateMat(1,K,CV_32FC1);

    IplImage *imgNrm = 0;
    imgNrmPP= 0;
    trimImage(img,3);

    if (img){
        imgNrm = normalizeImg(img,imgWidth,imgHeight);
        imgNrmPP = preprocessing(imgNrm);
        emit nrmRequest();

        //Set data
        imgVect = Image2Vector(imgNrmPP,0.0039215);
        knnResult = knn->find_nearest(imgVect,K,0,0,nearest,0);
        svmResult = svm->predict(imgVect);
        cvReleaseMat(&imgVect);

        imgPCAVect = Image2PCAVector(imgNrmPP, nEigens,eigenVectArr, pAvgTrainImg);
        knnPCAResult = knnPCA->find_nearest(imgPCAVect,K,0,0,nearest,0);
        svmPCAResult = svmPCA->predict(imgPCAVect);
        cvReleaseMat(&imgPCAVect);

        cvReleaseMat(&nearest);
        resetImage(img);
        resetImage(imgNrm);
        resetImage(imgNrmPP);
    }
}

void LearnPostures::doPCA()
{
    //PCA
    nEigens = nAllTrainImages-1;// set the number of eigenvalues to use
    pAvgTrainImg = 0; // the average image
    eigenVectArr = 0; // eigenvectors
    CvMat *eigenValMat = 0; // eigenvalues
    trainPCAData = cvCreateMat( nAllTrainImages, nEigens, CV_32FC1 ); // projected training images

    // do PCA on the training samples
    CvTermCriteria calcLimit;
    CvSize imgSize = cvSize(imgWidth,imgHeight);

    // allocate the eigenvector images
    eigenVectArr = (IplImage**)cvAlloc(sizeof(IplImage*) * nEigens);
    for(int i=0; i<nEigens; i++)
        eigenVectArr[i] = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

    // allocate the eigenvalue array
    eigenValMat = cvCreateMat( 1, nEigens, CV_32FC1 );

    // allocate the averaged image
    pAvgTrainImg = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

    // set the PCA termination criterion
    calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, nEigens, 1);
    //qDebug() << "cvCalcEigenObjects";
    // compute average image, eigenvalues, and eigenvectors
    cvCalcEigenObjects(
            nAllTrainImages,
            (void*)TrainImgArr,
            (void*)eigenVectArr,
            CV_EIGOBJ_NO_CALLBACK,
            0,
            0,
            &calcLimit,
            pAvgTrainImg,
            eigenValMat->data.fl);
    cvNormalize(eigenValMat, eigenValMat, 1, 0, CV_L1, 0);
    cvReleaseMat(&eigenValMat);

    // project the training images onto the PCA subspace
    int offset = trainPCAData->step / sizeof(float); // Mat->step returns number of bytes in a row
    for(int i=0; i<nAllTrainImages; i++)
    {
        cvEigenDecomposite(
                TrainImgArr[i],
                nEigens,
                eigenVectArr,
                0, 0,
                pAvgTrainImg,
                trainPCAData->data.fl + i*offset);
    }

}

void LearnPostures::on_btnOk_clicked()
{
    this->window()->hide();
}
