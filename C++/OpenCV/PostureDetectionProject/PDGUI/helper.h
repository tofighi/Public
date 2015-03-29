#ifndef HELPER_H
#define HELPER_H
//Define Some Colors
#define PD_BLACK CV_RGB(0,0,0)
#define PD_RED CV_RGB(255,0,0)
#define PD_GREEN CV_RGB(0,255,0)
#define PD_BLUE CV_RGB(0,0,255)
#define PD_WHITE CV_RGB(255,255,255)

#include <QDebug>
#include <QImage>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <QDebug>

#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#endif // HELPER_H

IplImage* MakeQtImage(IplImage* iplImage,QImage &qtImage);
IplImage* MakeGray(IplImage* img);
IplImage* MakeRGB(IplImage* img);
void AddBorder(IplImage* &img);
IplImage* GetSubImage(IplImage* img, CvRect bb, float scale);
IplImage* SizeResize(IplImage* img,int size);
IplImage* imgResize(IplImage* img,double resizeFactor);

IplImage* mythreshold(IplImage* img, int threshold, int noise);
IplImage* myhist(IplImage* img);
int find_threshold(IplImage* img);

// Preprocessing
void erode_image(IplImage* &img);

IplImage* skeleton(IplImage* img);
//void normalize(IplImage* &img,IplImage* &img_binary,int new_width, int new_height);
bool normalize(IplImage* img_src, IplImage* &img, IplImage* &img_binary,int new_width, int new_height);
IplImage* normalizeImg(IplImage* img,int new_width, int new_height);

//Matrix and Image
CvMat* Image2Vector(IplImage* img,float ratio);
CvMat* Image2PCAVector(IplImage* img,int nEigens,IplImage **eigenVectArr,IplImage *pAvgTrainImg);
void LogImage(const char* name,IplImage* img,int decimal,FILE* fp);
void LogMatrix(const char* name,CvMat* Matrix,int decimal, FILE* fp);
void SaveImage(const char* path, const char* name,IplImage* img);
IplImage *rotateImage(const IplImage *img, float angleDegrees);
void resetImage(IplImage* &img);
void trimImage(IplImage* &img,int trimValue);
IplImage* preprocessing(IplImage* img);

int findObjects(const char *cascade_path,IplImage *img, CvRect** &objectArr );
void drawRect(IplImage *img, CvRect* rect,float scale);
CvRect findBB(IplImage* img);

CvHistogram* hsvHistogram(IplImage* img);
IplImage* BackProjection(IplImage* img, CvHistogram* hist);
CvHistogram* FindSkinHist(IplImage* img_colored,const char* cascade_file);
