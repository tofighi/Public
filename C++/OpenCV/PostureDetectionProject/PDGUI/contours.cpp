#include "helper.h"
#include "contours.h"
#include <QDebug>


//This function returns a struct which contains pointer to the first contour and number of contours
//in an image.

ContourList FindContours (IplImage* img) {
        CvMemStorage* storage = cvCreateMemStorage();
        ContourList first_contour;
        first_contour.FirstContPointer=NULL;
        first_contour.Nc = cvFindContours(
                img,
                storage,
                &(first_contour.FirstContPointer),
                sizeof(CvContour),
                //CV_RETR_CCOMP,
                CV_RETR_TREE,
                //CV_RETR_EXTERNAL, //retrives only the extreme outer contours
                //method
                CV_CHAIN_APPROX_SIMPLE );
        //cvReleaseMemStorage(&storage);
        return first_contour;
}

BlobList* SortContours(IplImage* img, ContourList cntr,double RefineFactor){
        double ImageArea = img->width*img->height;
        double ContourArea=0;
        BlobList* OriginalContours;
        OriginalContours = new BlobList[cntr.Nc];
        int Nb = 0; // Number of blobs with considrable area regarding RefineFactor
        int Nc = 0;
        for(CvSeq* c=cntr.FirstContPointer; c!=NULL; c=c->h_next ){
            ContourArea = fabs(cvContourArea( c, CV_WHOLE_SEQ ));

            if((RefineFactor==0)||(ContourArea/ImageArea)> RefineFactor){
                OriginalContours[Nb].ContArea = ContourArea;
                OriginalContours[Nb].ContPointer = c;
                Nb++;
            }
            else{
                // OriginalContours[Nc].ContArea = 0;
                // OriginalContours[Nc].ContPointer = NULL;
                continue;
            }
           Nc++;
        }

        qDebug() << "Nb is: "<< Nb;
        qDebug() << "cntr.Nc is: "<< cntr.Nc;
        qDebug() << "Nc is: "<< Nc;

        return (BubbleContours(OriginalContours,Nb));
}

// function is going to sort Contours by their area and return a struct
// which contains
BlobList* BubbleContours(BlobList OriginalContours[],int FirstNc){
        int size = FirstNc;
        qDebug() << "size is: "<< size;
        BlobList hold;
        int i,j;
   for(i = 0; i < size-1; i++)
   {
       for(j = i+1; j < size; j++)
       {
                   if( OriginalContours[i].ContArea < OriginalContours[j].ContArea)
           {
                 hold = OriginalContours[i];
                 OriginalContours[i] = OriginalContours[j];
                 OriginalContours[j] = hold;
           }
        }
    }
     return OriginalContours;
}


IplImage* DrawContour(IplImage* img, CvSeq* cntr,CvScalar color,int thickness){
    IplImage* img_contour = cvCreateImage(cvGetSize(img),8,3);
    cvZero(img_contour);

    //double maxArea=0,bndArea=0,density=0,whr=0,bwidth=0,bheight=0;
    //CvSeq* contour = NULL;
    // bndRect = cvBoundingRect( cntr , 0 );
    // cvSetImageROI(  OriginalImage, bndRect );

    // Draw current contour.

    //cvCvtColor( img, img_8uc3, CV_GRAY2BGR );
    cvDrawContours(
            img_contour,
            cntr,
            color,		// External Color
            PD_WHITE,		// Hole Color
            2,			// Vary max_level and compare results
            thickness, // when thickness is negative, contour will be fullfilled
            8 );
    return (img_contour);
}
