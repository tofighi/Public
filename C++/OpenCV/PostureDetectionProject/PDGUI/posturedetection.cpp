#include "posturedetection.h"


PostureDetection::PostureDetection(int new_width, int new_height)
{
    //initial
    pd_img_input = 0; //input from camera
    pd_img_blob = 0; // normalized blob
    pd_img_skin = 0; // image of skin area
    pd_img_yuvBG = pd_ImaskCodeBook = pd_ImaskCodeBookCC = 0;
    pdSkinHist = 0;

    //Start: Background subtraction //
    pd_nframes = 0; //Number of processed frames
    pd_nframesToLearnBG = 100;
    pd_modelBG = cvCreateBGCodeBookModel();
        //Set color thresholds to default values
        pd_modelBG->modMin[0] = 3;
        pd_modelBG->modMin[1] = pd_modelBG->modMin[2] = 3;
        pd_modelBG->modMax[0] = 10;
        pd_modelBG->modMax[1] = pd_modelBG->modMax[2] = 10;
        pd_modelBG->cbBounds[0] = pd_modelBG->cbBounds[1] = pd_modelBG->cbBounds[2] = 10;
    //End: Background subtraction //

    //log file
    log =fopen("D:/Users/Me/Programming/Qt/0DATA/PD/output.txt", "w");
    sprintf(class_samples_path , "D:/Users/Me/Programming/Qt/0DATA/PD/classes/");
    sprintf(train_samples_path , "D:/Users/Me/Programming/Qt/0DATA/PD/postures/");
    sprintf(save_img_path , "D:/Users/Me/Programming/Qt/0DATA/PD/image_results/");
    sprintf(cascades_path , "D:/Users/Me/Programming/Qt/0DATA/PD/cascades/");
    sprintf(aGest,"aGest.xml");
    sprintf(cFileExtension,"jpg");
    /*
     for each class we have equal number of training samples. For evaluating, we divide these training
     samples into train and test samples. For example, if we have 20 training samples (total_training_samples)
     for each class, we divide it into 5 train samples (train_samples) and 15 test samples.
    */
    total_train_samples = 50; // total train samples per class
    train_samples = 10; // train samples for current training
    classes= 10;
    nTrainSamples= train_samples*classes;
    nTotalSamples= total_train_samples*classes;
    // Number of PCA Eigen Vectors for training the model
    nEigens = nTrainSamples-1;// set the number of eigenvalues to use

    img_width = new_width;
    img_height = new_height;
    trainData = cvCreateMat(nTrainSamples, img_width*img_height, CV_32FC1);
    trainClasses = cvCreateMat(nTrainSamples, 1, CV_32FC1);
    //PCA Method
    pAvgTrainImg = 0; // the average image
    eigenVectArr = 0; // eigenvectors
    eigenValMat = 0; // eigenvalues
    projectedTrainMat = cvCreateMat( nTrainSamples, nEigens, CV_32FC1 ); // projected training images
    trainPCAData = cvCreateMat(nTrainSamples, nEigens, CV_32FC1);
    //trainPCAClasses = cvCreateMat(nTrainSamples, 1, CV_32FC1);

    //Load Train, Test and Class images
    //LoadClassImages(class_samples_path);
    qDebug() << "Class Sample Images Loaded!";

    //LoadAllImages(train_samples_path);
    qDebug() << "Ttain and Test Images Loaded!";
    //Training and Testing

    QElapsedTimer timer;
    qDebug() << "start-training!";

    timer.start();
    qDebug() << "trainPCA...";
    trainPCA();
    qDebug() << "trainPCA is done in" << timer.restart() << "ms.";

    qDebug() << "training...";
    train();
    qDebug() << "training is done in" << timer.restart() << "ms.";

    qDebug() << "testing...";
    test();
    qDebug() << "testing is done in" << timer.restart() << "ms.";

}

PostureDetection::~PostureDetection(){

}

void PostureDetection::LoadClassImages(char* class_samples_path){
    ClassImgArr = (IplImage **)cvAlloc((classes)*sizeof(IplImage *) );
    char file[255];
    int i;
    for(i =0; i<classes; i++){
        if(i<10)
            sprintf(file,"%s0%d.jpg",class_samples_path, i);
        else
            sprintf(file,"%s%d.jpg",class_samples_path, i);

        ClassImgArr[i] = cvLoadImage(file,CV_LOAD_IMAGE_COLOR);
        if(!ClassImgArr[i]){
            qDebug() << "Error: Cant load image"<< file;
            //exit(-1);
        }
    }
}


void PostureDetection::LoadAllImages(char* train_samples_path){

    // Load all images in postures directory

    AllImgArr = (IplImage **)cvAlloc((nTotalSamples)*sizeof(IplImage *) );
    char file[255];
    int i,j,index,train_index;
    CvMat row;
    for(i =0; i<classes; i++){
        for( j = 0; j< total_train_samples; j++){
            //Load file
            if(i<10){
                if ( j<10)
                    sprintf(file,"%s0%d/0%d00%d.%s",train_samples_path, i, i , j,cFileExtension);
                else if(j<100)
                    sprintf(file,"%s0%d/0%d0%d.%s",train_samples_path, i, i , j,cFileExtension);
                else if(j<1000)
                    sprintf(file,"%s%d/0%d%d.%s",train_samples_path, i, i , j,cFileExtension);
            }
            else if(i<100){
                if ( j<10)
                    sprintf(file,"%s0%d/%d00%d.%s",train_samples_path, i, i , j,cFileExtension);
                else if(j<100)
                    sprintf(file,"%s0%d/%d0%d.%s",train_samples_path, i, i , j,cFileExtension);
                else if(j<1000)
                    sprintf(file,"%s%d/%d%d.%s",train_samples_path, i, i , j,cFileExtension);
            }


            index = i*total_train_samples+j;

            //AllImgArr[total_train_samples*i+j] = cvLoadImage(file, CV_LOAD_IMAGE_GRAYSCALE);
            AllImgArr[index] = cvLoadImage(file,CV_LOAD_IMAGE_COLOR);

            if(!AllImgArr[i]){
                qDebug() << "Error: Cant load image"<< file;
                //exit(-1);
            }
        }
    }

    //Preprocess imgaes to create normal image array
    IplImage* img_pp = 0;
    train_index = 0;
    AllImgArrNormal = (IplImage **)cvAlloc((nTotalSamples)*sizeof(IplImage *) );
    for(i =0; i<classes; i++){
        for( j = 0; j< total_train_samples; j++){
            index= i*total_train_samples + j;
            //qDebug() << "prep:s";
            trimImage(AllImgArr[index],3);
            if (AllImgArr[index]){
                pd_img_blob = normalizeImg(pd_img_input,img_width,img_height);
                if (pd_img_blob){
                    img_pp = preprocessing(pd_img_blob);
                    AllImgArrNormal[train_index] = img_pp;
                    /*
                    AllImgArrNormal[train_in dex+1] = rotateImage(img_pp, 15);
                    AllImgArrNormal[train_index+2] = rotateImage(img_pp, -15);
                    train_index+=3;
                    //*/
                    train_index++;
                    resetImage(pd_img_input);
                    resetImage(pd_img_blob);
                }else{
                    qDebug() << "Error: Train image is empty!";
                    exit (-1);
                }
            }

            //qDebug() << "prep:end";
                        //Save Preprocessed images
                        char image_name[10];
                        if(j<10)
                            sprintf(image_name,"%i0%i",i,j);
                        else
                            sprintf(image_name,"%i%i",i,j);
                        SaveImage(save_img_path,image_name,AllImgArrNormal[index]);
        }
    }

    //Create train image array
    TrainImgArr = (IplImage **)cvAlloc((nTrainSamples)*sizeof(IplImage *) );
    train_index = 0;
    for(i =0; i<classes; i++){
        for( j = 0; j< train_samples; j++){
            index= i*total_train_samples + j;
//             qDebug() << "load:"<< train_index<< index << i;
            TrainImgArr[train_index] = AllImgArrNormal[index];
            //Set class label
            cvGetRow(trainClasses, &row, train_index);
            cvSet(&row, cvRealScalar(i));
            train_index++;
        }

    }
    //qDebug() << "train_index=" << train_index;
//LogMatrix("trainClasses",trainClasses,2, fp);
//qDebug() << "load:end"; */

}


void PostureDetection::train(){
    //process file
    //AllImgArrNormal = (IplImage **)cvAlloc((nTotalSamples)*sizeof(IplImage *) );
    int i,j,index,train_index;
    CvMat *img_train_pp_vector,*img_pca_classify_vector,row;

    //qDebug() << "a11";
    for(i =0; i<classes; i++){
        for( j = 0; j< train_samples; j++){
           train_index= i*total_train_samples + j;
           index =  i*train_samples + j;
//           //Set class label
//           cvGetRow(trainClasses, &row, index);
//           cvSet(&row, cvRealScalar(i));
           //Set train data
           //qDebug() << "train index=" << train_index;
           //qDebug() << "index=" << index;
           cvGetRow(trainData, &row, index);
           //img_train_pp_vector = Image2Vector(AllImgArrNormal[train_index],0.0039215);//change range from 0-255 to 0-1 (1/255 = 0.0039215)
           img_train_pp_vector = Image2Vector(TrainImgArr[index],0.0039215);//change range from 0-255 to 0-1 (1/255 = 0.0039215)
           cvCopy(img_train_pp_vector, &row, NULL);

           cvGetRow(trainPCAData, &row, index);
           img_pca_classify_vector = Image2PCAVector(TrainImgArr[index], nEigens,eigenVectArr, pAvgTrainImg);
            cvCopy(img_pca_classify_vector, &row, NULL);

           cvReleaseMat(&img_train_pp_vector);
           cvReleaseMat(&img_pca_classify_vector);
        }
    }

    //Training the model with KNN
     qDebug() <<"trainData: " << trainData->rows << trainData->cols;
     qDebug() <<"projectedTrainMat: " << projectedTrainMat->rows << projectedTrainMat->cols;
     qDebug() <<"trainPCAData: " << trainPCAData->rows << trainPCAData->cols;
    knn = new CvKNearest( trainData, trainClasses, 0, false, K );

//    knnPCA = new CvKNearest( projectedTrainMat, trainClasses, 0, false, K );
    knnPCA = new CvKNearest( trainPCAData, trainClasses, 0, false, K );


    //Setting up SVM parameters
    CvSVMParams params;
    params.kernel_type=CvSVM::LINEAR;
    params.svm_type=CvSVM::C_SVC;
    params.C=1;
    params.term_crit=cvTermCriteria(CV_TERMCRIT_ITER,100,0.000001);

    //Training the model with SVM
    //qDebug() <<"knnPCA:end";
    svm.train(trainData,trainClasses,cv::Mat(),cv::Mat(),params);
    //svmPCA.train(projectedTrainMat,trainClasses,cv::Mat(),cv::Mat(),params);
    svmPCA.train(trainPCAData,trainClasses,cv::Mat(),cv::Mat(),params);
    //We don't need these matrix after training
    cvReleaseMat(&trainData);
    cvReleaseMat(&trainClasses);
}

void PostureDetection::classify(IplImage* img_pp)
{

        CvMat* img_classify_vector;
        CvMat* img_pca_classify_vector;
        CvMat* nearest=cvCreateMat(1,K,CV_32FC1);

        //qDebug() << "classify:start";

        //Set data
        img_classify_vector = Image2Vector(img_pp,0.0039215);


        knn_result = knn->find_nearest(img_classify_vector,K,0,0,nearest,0);
        svm_result = svm.predict(img_classify_vector);
        cvReleaseMat(&img_classify_vector);

        img_pca_classify_vector = Image2PCAVector(img_pp, nEigens,eigenVectArr, pAvgTrainImg);
        knn_pca_result = knnPCA->find_nearest(img_pca_classify_vector,K,0,0,nearest,0);
        svm_pca_result = svmPCA.predict(img_pca_classify_vector);
        cvReleaseMat(&img_pca_classify_vector);
        //*/

//        int accuracy=0;
//        for(int i=0;i<K;i++){
//                if( nearest->data.fl[i] == result)
//                    accuracy++;
//        }
//        float pre=100*((float)accuracy/(float)K);
//        if(showResult==1){
//                printf("|\t%.0f\t| \t%.0f\t| \t%.2f%%  \t| \t%d of %d \t| \n",response,result,pre,accuracy,K);
//                printf(" -----------------------------------------------------------------------\n");
//        }

        cvReleaseMat(&nearest);


}

void PostureDetection::test(){
    //qDebug() << "test:start";
        int i,j;
        int index;
        int svm_error_count=0;
        int knn_error_count=0;
        int svm_pca_error_count=0;
        int knn_pca_error_count=0;
        int testCount=0;
        for(i =0; i<classes; i++){
                for( j = train_samples; j< total_train_samples; j++){
                        index = i*total_train_samples + j;
                        //qDebug() << "test:index:" << index ;
                        //cvShowImage("AllImgArrNormal[index]",AllImgArrNormal[index]);
                        classify(AllImgArrNormal[index]);
                        if((int)svm_result!=i) {svm_error_count++; /* qDebug()<< "SVM error image\t" << i << j << svm_result;*/}
                        if((int)knn_result!=i) knn_error_count++;
                        if((int)svm_pca_result!=i) {svm_pca_error_count++; qDebug()<< "svm pca error image\t" << i << j << svm_pca_result;}
                        if((int)knn_pca_result!=i) knn_pca_error_count++;
                        testCount++;
                }
        }

        svm_error= 100*(float)svm_error_count/(float)testCount;
        knn_error= 100*(float)knn_error_count/(float)testCount;

        svm_pca_error= 100*(float)svm_pca_error_count/(float)testCount;
        knn_pca_error= 100*(float)knn_pca_error_count/(float)testCount;
        //printf("System Error: %.2f%%\n", totalerror);
        qDebug() << "Test Count:\t"<<testCount;
        qDebug() << "SVM Error:\t"<<svm_error_count<< "\t"<< svm_error<<"%";
        qDebug() << "SVM PCA Error:\t"<<svm_pca_error_count << "\t"<< svm_pca_error<<"%";

        qDebug() << "KNN Error:\t"<< knn_error_count<< "\t"<< knn_error<<"%";
        qDebug() << "KNN PCA Error:\t"<<knn_pca_error_count <<"\t" << knn_pca_error<<"%";
}



void PostureDetection::trainPCA()
{
    int i,offset;

    // do PCA on the training samples
    CvTermCriteria calcLimit;
    CvSize ImgSize = cvSize(img_width,img_height);

    // allocate the eigenvector images
    eigenVectArr = (IplImage**)cvAlloc(sizeof(IplImage*) * nEigens);
    for(i=0; i<nEigens; i++)
        eigenVectArr[i] = cvCreateImage(ImgSize, IPL_DEPTH_32F, 1);

    // allocate the eigenvalue array
    eigenValMat = cvCreateMat( 1, nEigens, CV_32FC1 );

    // allocate the averaged image
    pAvgTrainImg = cvCreateImage(ImgSize, IPL_DEPTH_32F, 1);

    // set the PCA termination criterion
    calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, nEigens, 1);

    // compute average image, eigenvalues, and eigenvectors
    cvCalcEigenObjects(
            nTrainSamples,
            (void*)TrainImgArr,
            (void*)eigenVectArr,
            CV_EIGOBJ_NO_CALLBACK,
            0,
            0,
            &calcLimit,
            pAvgTrainImg,
            eigenValMat->data.fl);
    cvNormalize(eigenValMat, eigenValMat, 1, 0, CV_L1, 0);


  /*
        // Save Eigen Vector Array
        char image_name[25];
        for (i=0; i<nEigens; i++){
            if (i<10)
                sprintf(image_name,"eigenVect00%i",i);
            else if(i<100)
                sprintf(image_name,"eigenVect0%i",i);
            else if(i<1000)
                sprintf(image_name,"eigenVect%i",i);
            IplImage* eigenVectArr_255 = cvCreateImage( cvGetSize(eigenVectArr[i]), IPL_DEPTH_32F, 1 );
            cvConvertScale(eigenVectArr[i], eigenVectArr_255,25500, 0);
            SaveImage(save_img_path,image_name,eigenVectArr_255);
            LogImage(image_name,eigenVectArr_255,4,log);
            cvReleaseImage(&eigenVectArr_255);

    }
   //*/

        // project the training images onto the PCA subspace
        offset = projectedTrainMat->step / sizeof(float); // Mat->step returns number of bytes in a row
        for(i=0; i<nTrainSamples; i++)
        {
            //int offset = i * nEigens;
            cvEigenDecomposite(
                    TrainImgArr[i],
                    nEigens,
                    eigenVectArr,
                    0, 0,
                    pAvgTrainImg,
                    projectedTrainMat->data.fl + i*offset);
        }


        // */
        //  qDebug() << "Done!";
        // store the recognition data as an xml file
        //        storeTrainingData();
}




IplImage* PostureDetection::SkinDetection(IplImage* img){

    IplImage* img_skin= 0;
    if(!pdSkinHist) {
            char cascade_file[255];
            sprintf(cascade_file,"%s%s",cascades_path, aGest);
            pdSkinHist = FindSkinHist(img,cascade_file);
        }

        if(pdSkinHist){
           img_skin = BackProjection(img, pdSkinHist);
        }
        return img_skin;
}

IplImage* PostureDetection::FindPostures(IplImage* img)
{
    IplImage *img_pp = 0;
    resetImage(pd_img_input);
    resetImage(pd_img_blob);
    resetImage(pd_img_skin);
    //Remove black frame
    trimImage(img,3);
    if (img){
        ++pd_nframes;
        qDebug() << "pd_nframes:" << pd_nframes ;
            // Background Learning
            if (pd_nframes==1){
            pd_img_yuvBG = cvCloneImage(pd_img_input);
            pd_ImaskCodeBook = cvCreateImage( cvGetSize(pd_img_input), IPL_DEPTH_8U, 1 );
            pd_ImaskCodeBookCC = cvCreateImage( cvGetSize(pd_img_input), IPL_DEPTH_8U, 1 );
            cvSet(pd_ImaskCodeBook,cvScalar(255));
            }
            cvCvtColor( pd_img_input, pd_img_yuvBG, CV_BGR2YCrCb );//YUV For codebook method
            //This is where we build our background model
            if(pd_nframes-1 < pd_nframesToLearnBG  ) {
                cvBGCodeBookUpdate( pd_modelBG, pd_img_yuvBG );
                //qDebug() << "cvBGCodeBookUpdate:passed";
                }
            if( pd_nframes-1 == pd_nframesToLearnBG  )
                cvBGCodeBookClearStale( pd_modelBG, pd_modelBG->t/2 );
            //Find the foreground if any
            if( pd_nframes-1 >= pd_nframesToLearnBG  )
            {
                // Find foreground by codebook method
                cvBGCodeBookDiff( pd_modelBG, pd_img_yuvBG, pd_ImaskCodeBook );
                // This part just to visualize bounding boxes and centers if desired
                cvErode( pd_ImaskCodeBook, pd_ImaskCodeBook, NULL, 3);
                cvCopy(pd_ImaskCodeBook,pd_ImaskCodeBookCC);

                cvSegmentFGMask( pd_ImaskCodeBookCC );
            }

                cvShowImage( "pd_img_yuvBG",pd_img_yuvBG);
                cvShowImage( "ForegroundCodeBook",pd_ImaskCodeBook);
                cvShowImage( "CodeBook_ConnectComp",pd_ImaskCodeBookCC);

        //pd_img_skin = SkinDetection(pd_img_input);
        if (pd_img_skin){
            cvShowImage("pd_img_skin",pd_img_skin);
            pd_img_blob = normalizeImg(pd_img_input,img_width,img_height);
            if (pd_img_blob){
                img_pp = preprocessing(pd_img_blob);
                int threshold= find_threshold(pd_img_blob);
                IplImage* img_src_thresh = mythreshold(pd_img_blob,threshold,1);
                cvShowImage("img_src",img_src_thresh);
                cvSmooth( img_src_thresh, img_src_thresh, CV_GAUSSIAN, 21, 21 );
                IplImage* img_src_thresh_smooth = mythreshold(img_src_thresh,200,0);
                cvShowImage("img_src_thresh_smooth",img_src_thresh_smooth);
                cvReleaseImage(&img_src_thresh);
                cvReleaseImage(&img_src_thresh_smooth);
            }
        }
    }
    return img_pp;
}
