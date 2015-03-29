#ifndef CONTOURS_H
#define CONTOURS_H

#endif // CONTOURS_H
#include <cv.h>
#include <highgui.h>

//contours
struct ContourList {
                int Nc;
                CvSeq* FirstContPointer;
        };

struct BlobList {
                //unsigned int Nb; //Number of Blobs
                double ContArea;
                CvSeq* ContPointer;
        };

ContourList FindContours (IplImage* img);
BlobList* SortContours(IplImage* img, ContourList cntr,double RefineFactor);
IplImage* DrawContour(IplImage* img, CvSeq* cntr,CvScalar color,int thickness);
BlobList* BubbleContours(BlobList OriginalContours[],int FirstNc);
//IplImage* ConvexHull (IplImage* img, CvSeq* cntr);
//IplImage* skeleton(IplImage* img);
