#ifndef CBLOB_H
#define CBLOB_H
#endif // CBLOB_H

#include <cv.h>
#include <highgui.h>

struct Blob {
    CvSeq* contour;
    CvPoint2D64f centroid; ///< Centroid.
    double primeter;
    double orientation;
    //Moments
    union
        {
          double area; ///< Area (moment 00).
          double m00; ///< Moment 00 (area).
        };
    double m10, m01, m20, m11, m02, m30, m21, m12, m03;
    double mu20, mu11, mu02, mu30, mu21, mu12, mu03;
    double p1; ///< Hu moment 1.
    double p2; ///< Hu moment 2.
};

class CBlob
{
    IplImage* img_orig;
public:
    CBlob(IplImage* img);
    Blob FindBlobs(IplImage* img, double RefineFactor=0);
};










