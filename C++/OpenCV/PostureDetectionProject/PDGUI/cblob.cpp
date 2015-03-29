#include "cblob.h"

CBlob::CBlob(IplImage* img){
    img_orig = img;
}

//Blob CBlob::FindContours (IplImage* img, double RefineFactor=0) {
//        CvMemStorage* storage = cvCreateMemStorage();
//
//        CvSeq* first_contour =NULL;
//        int Nc = cvFindContours(
//                img,
//                storage,
//                &(first_contour),
//                sizeof(CvContour),
//                CV_RETR_CCOMP,
//                CV_CHAIN_APPROX_SIMPLE );
//
//        double ImageArea = img->width*img->height;
//        double ContourArea=0;
//        Blob* BlobListTemp = new Blob[Nc];
//
//        int Nb = 0;
//        for(CvSeq* c=first_contour; c!=NULL; c=c->h_next ){
//                ContourArea = fabs(cvContourArea( c, CV_WHOLE_SEQ ));
//
//                if((RefineFactor==0)||(ContourArea/ImageArea)>RefineFactor ){
//                                BlobListTemp[Nb].ContArea = ContourArea;
//                                BlobListTemp[Nb].ContPointer = c;
//                                Nb++;
//                        }
//                else{ continue;}
//                }
//
//        // Make List of Blob which has considerable area
//        Blob* BlobList = new Blob[Nb];
//        for (int i=0; i<Nb; i++){
//            BlobList[i]= BlobListTemp[i];
//        }
//
//
//
//}
