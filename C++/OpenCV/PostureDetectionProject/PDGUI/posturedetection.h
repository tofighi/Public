#ifndef POSTUREDETECTION_H
#define POSTUREDETECTION_H

#include <cv.h>
#include <highgui.h>
#include <ml.h>
#include <helper.h>
#include <cvaux.h>
#include <QElapsedTimer>
#include <QProcess>

class PostureDetection
{
public:
        PostureDetection(int new_width, int new_height);
        ~PostureDetection();
        IplImage* pd_img_input;
        IplImage* pd_img_blob;
        IplImage* pd_img_skin;

        IplImage *pd_img_yuvBG, *pd_ImaskCodeBook, *pd_ImaskCodeBookCC;
        FILE* log;
        int total_train_samples;
        int train_samples;
        int classes;
        int nTrainSamples;
        int nTotalSamples;
        int pd_nframes; // Number of processed frames
        int pd_nframesToLearnBG;
        CvBGCodeBookModel* pd_modelBG;
        IplImage ** ClassImgArr;
        IplImage ** AllImgArr;
        IplImage ** AllImgArrNormal;
        IplImage ** TrainImgArr;
        char train_samples_path[255];
        char class_samples_path[255];
        char save_img_path[255];
        char cascades_path[255];
        char aGest[255];
        char cFileExtension[5];

        IplImage* FindPostures(IplImage* img);

        void LoadAllImages(char* train_samples_path);
        void LoadClassImages(char* class_samples_path);
        IplImage* SkinDetection(IplImage* img);
        CvHistogram* pdSkinHist;

        float knn_result;
        float svm_result;
        float knn_pca_result;
        float svm_pca_result;

        float knn_error;
        float svm_error;
        float knn_pca_error;
        float svm_pca_error;

        void classify(IplImage* img);

        //PCA
        IplImage * pAvgTrainImg; // the average image
        IplImage ** eigenVectArr; // eigenvectors
        CvMat * eigenValMat; // eigenvalues
        CvMat * projectedTrainMat; // projected training faces
        void trainPCA();

private:
        FILE *fp;
        int sub; // number of sub rows from EigenVector
        CvMat* trainData;
        CvMat* trainClasses;
        CvMat* trainPCAData;
        CvMat* trainPCAClasses;
        int nEigens;
        int size;
        int img_width;
        int img_height;
        static const int K=5;


        int pca_param;

        CvKNearest *knn;
        CvSVM svm;

        CvKNearest *knnPCA;
        CvSVM svmPCA;
        IplImage* ppTrain();
        IplImage* ppTest();
        void train();
        void test();
};

#endif // POSTUREDETECTION_H
