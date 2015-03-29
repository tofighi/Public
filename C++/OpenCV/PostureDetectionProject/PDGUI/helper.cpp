#include "helper.h"
void LogImage(const char* name,IplImage* img,int decimal,FILE* fp)
{
    float value;
    int width = img->width;
    int height = img->height;
    char decimal_text[10];
    sprintf(decimal_text,"%%.%if\t",decimal);
    fprintf(fp,"<%s type='IplImage'>\n",name);
    fprintf(fp,"<width>%i</width>\n",width);
    fprintf(fp,"<height>%i</height>\n<data>\n",height);
    for(int i=0;i<width;i++)
    {
        for(int j=0;j<height;j++)
        {
            value = cvGet2D(img,i,j).val[0];
            fprintf (fp, decimal_text,value);
        }

        fprintf (fp, "\n");
    }
    fprintf(fp,"</data>\n</%s>\n",name);
}
void SaveImage(const char* path, const char* name,IplImage* img){
    char file_name[255];
    sprintf(file_name,"%s%s.jpg",path,name);
    cvSaveImage(file_name, img);
}



void LogMatrix(const char* name,CvMat* Matrix,int decimal, FILE* fp)
{
    float *value = Matrix->data.fl;
    int Rows = Matrix->rows;
    int Cols = Matrix->cols;
    char decimal_text[10];
    sprintf(decimal_text,"%%.%if\t",decimal);
    fprintf(fp,"<%s type='CvMat'>\n",name);
    fprintf(fp,"<rows>%i</rows>\n",Rows);
    fprintf(fp,"<cols>%i</cols>\n<data>\n",Cols);
    for(int i=0;i<Rows;i++)
    {
        for(int j=0;j<Cols;j++)
        {
//            qDebug()  << i << j<< value[i*Cols+j] ;

            //value = cvmGet(Matrix,i,j);
            //value = cvGet2D(Matrix,i,j).val[0];
            fprintf (fp, decimal_text,value[i*Cols+j]);
        }

        fprintf (fp, "\n");
    }
    fprintf(fp,"</data>\n</%s>\n",name);
}

//Convert IplImage to QtImage
IplImage* MakeQtImage(IplImage* iplImage,QImage &qtImage){
    IplImage* iplImage_rgb = MakeRGB(iplImage);
    // feed into QImage
    uchar *cv = (uchar*) (iplImage_rgb->imageData);
    qtImage = QImage(cv, iplImage_rgb->width, iplImage_rgb->height, iplImage_rgb->widthStep, QImage::Format_RGB888);
    return iplImage_rgb;
}


CvMat* Image2Vector(IplImage* img,float ratio){
    IplImage* img_gray = MakeGray(img);
    IplImage* img_gray_32 = cvCreateImage( cvGetSize(img_gray), IPL_DEPTH_32F, 1 );
    cvConvertScale(img_gray, img_gray_32, ratio, 0);
    int vector_size = img->width*img->height;
    //qDebug() << "Image2Vector2" << vector_size ;
    CvMat* img_matrix = cvCreateMat(1,vector_size,CV_32FC1);
    float *data = img_matrix->data.fl;
    for(int i=0;i<img->width;i++){
        for(int j=0;j<img->height;j++){
             data[i*img->width+j] = cvGet2D(img_gray_32,i,j).val[0];
             //qDebug() << "Image2Vector2 data["<<i << j <<i*img->width+j << data[i*img->width+j] ;
        }

    }
    cvReleaseImage(&img_gray);
    cvReleaseImage(&img_gray_32);
    return img_matrix;
}

CvMat* Image2PCAVector(IplImage* img,int nEigens,IplImage **eigenVectArr,IplImage * pAvgTrainImg){

    float *projectedImg = (float *)cvAlloc( nEigens*sizeof(float) );
    CvMat *pcaVect = cvCreateMat(1,nEigens,CV_32FC1);

    // projecting test images onto the PCA subspace
    float *value = pcaVect->data.fl;
    cvEigenDecomposite(
            img,
            nEigens,
            eigenVectArr,
            0, 0,
            pAvgTrainImg,
            projectedImg);

    for(int i=0; i<nEigens; i++){
        value[i]=projectedImg[i];
        //qDebug() << i << value[i];
    }
    cvFree(&projectedImg);
    return pcaVect;
}

//Function for making any image in gray level format
IplImage* MakeGray(IplImage* img) {
    IplImage* gray=0;
    if (img->nChannels >1) {
        gray = cvCreateImage( cvGetSize(img), img->depth, 1 );
        cvCvtColor(img, gray, CV_BGR2GRAY);
    }else{
        gray = cvCloneImage(img);
    }
    return (gray);
}

IplImage* MakeRGB(IplImage* img) {
    //qDebug()<<"MakeRGB: img->nChannels= "<<img->nChannels;
    IplImage* img_rgb= cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
    switch(img->nChannels)
    {
    case 1:
        cvCvtColor( img, img_rgb, CV_GRAY2BGR);
        break;
    case 3:
        cvCvtColor( img, img_rgb, CV_BGR2RGB);
        break;
    case 4:
        cvCvtColor(img, img_rgb, CV_BGRA2RGB);
        break;
    }
    return (img_rgb);
}
IplImage* GetSubImage(IplImage* img, CvRect bb, float scale){
    CvMat data;
    CvMat data_dst;
    CvPoint center;

    center.x= bb.x + bb.width/2;
    center.y= bb.y + bb.height/2;
    bb.width *= scale;
    bb.height *= scale;
    bb.x = center.x - (bb.width/2);
    bb.y = center.y - (bb.height/2);

    CvSize img_sub_size = cvSize(bb.width,bb.height);
    IplImage* img_sub=cvCreateImage(img_sub_size, img->depth, img->nChannels);
    cvGetSubRect(img, &data, cvRect(bb.x,bb.y,bb.width,bb.height));
    cvGetSubRect(img_sub, &data_dst, cvRect(0,0,img_sub->width,img_sub->height));
    cvCopy(&data, &data_dst, NULL);
    return img_sub;
}

//Make a black frame around threshold image to be prepared for contour finding
void AddBorder(IplImage* &img){
  CvMat data;
  CvMat dataA;
  int border_thickness = 1;
  CvSize img_size = cvGetSize(img);
  CvSize img_sub_size= cvSize(img_size.width-(2*border_thickness),img_size.height-(2*border_thickness));
  IplImage* img_sub=cvCreateImage(img_sub_size, img->depth, img->nChannels );
  cvResize(img, img_sub, CV_INTER_NN);
  cvGetSubRect(img_sub, &data, cvRect(0,0,img_sub->width,img_sub->height));

  cvZero(img);
  CvRect sub_rect = cvRect(border_thickness,border_thickness,img_size.width-(2*border_thickness), img_size.height-(2*border_thickness));
  cvGetSubRect(img, &dataA, sub_rect);
  cvCopy(&data, &dataA, NULL);
  cvReleaseImage(&img_sub);
}

IplImage* SizeResize(IplImage* img,int size){
   CvSize img_size = cvGetSize(img);
   int new_width, new_height;
    double w2h= (double)((double)img_size.width/(double)img_size.height);
   if (img_size.height>img_size.width){
       new_height = size;
       new_width = (int)(w2h*size);
   } else{
       new_width = size;
       new_height = (int)((1/w2h)*size);
   }

IplImage *img_resized = cvCreateImage(cvSize(new_width,new_height),img->depth,img->nChannels);
cvResize(img,img_resized,CV_INTER_NN);
return img_resized;
}

IplImage* imgResize(IplImage* img,double resizeFactor){
    CvSize img_size = cvGetSize(img);
    int new_height = (int)(resizeFactor*(double)img_size.height);
    int new_width =  (int)(resizeFactor*(double)img_size.width);
    IplImage *img_resized = cvCreateImage(cvSize(new_width,new_height),img->depth,img->nChannels);
    cvResize(img,img_resized,CV_INTER_NN);
    return img_resized;
}



// Function for applying threshold
IplImage* mythreshold(IplImage* img, int threshold, int noise) {
    IplImage* gray = MakeGray(img);
    cvThreshold( gray, gray, threshold, 255, CV_THRESH_BINARY );
    if (noise>0){
        IplConvKernel* structuringElement0;
        IplConvKernel* structuringElement1;
        IplConvKernel* structuringElement2;
        structuringElement0 = cvCreateStructuringElementEx( 5, 5, 1, 1,CV_SHAPE_RECT, NULL );
        structuringElement1 = cvCreateStructuringElementEx( 3, 3, 1, 1,CV_SHAPE_RECT, NULL );
        structuringElement2 = cvCreateStructuringElementEx( 3, 3, 1, 1,CV_SHAPE_ELLIPSE, NULL );
        cvErode( gray, gray, NULL, 1);
        cvDilate( gray, gray, NULL, 1);
//        cvErode( gray, gray, structuringElement0, 1);
//        cvDilate( gray, gray, structuringElement0, 1);
        //cvMorphologyEx(gray,gray, NULL,structuringElement2, CV_MOP_CLOSE, 5);
        //cvMorphologyEx(gray,gray, NULL,structuringElement2, CV_MOP_CLOSE, 10);
        //cvMorphologyEx(gray,gray, NULL,structuringElement2, CV_MOP_CLOSE, 15);
        cvReleaseStructuringElement(&structuringElement0);
        cvReleaseStructuringElement(&structuringElement1);
        cvReleaseStructuringElement(&structuringElement2);

    }
    return gray;
}

// Function for drawing histogram
IplImage* myhist(IplImage* img) {
    int i, bin_w;
    float max_value = 0;
    int hist_size = 256;
    float range_0[]={0,256};
    float* ranges[] = { range_0 };
    IplImage* gray = MakeGray(img);
    IplImage  *hist_image = cvCreateImage(cvSize(320,200), IPL_DEPTH_8U, 3);

    CvHistogram *hist;
    hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

    cvCalcHist( &gray, hist, 0, 0 );
    int max_idx = 0;
    cvGetMinMaxHistValue( hist, 0, &max_value, 0, &max_idx );
    //find threshold point
    bool toggle = true;
    int IndexForLookupdensity = max_idx;
    float bin_diff=0;
    while (toggle){
      IndexForLookupdensity++;
      float histValue = cvQueryHistValue_1D( hist,IndexForLookupdensity);
          if (max_value-histValue>bin_diff){
              bin_diff = max_value-histValue;
          }else{
          toggle = false;
          }
      }

    cvScale( hist->bins, hist->bins, ((double)hist_image->height)/max_value, 0 );
    cvSet( hist_image, cvScalarAll(255), 0 );
    double threshold =0;
    cvThreshHist( hist, threshold);
         bin_w = cvRound((double)hist_image->width/hist_size);
         for( i = 0; i < hist_size; i++ )
             cvRectangle( hist_image, cvPoint(i*bin_w, hist_image->height),
                          cvPoint((i+1)*bin_w, hist_image->height - cvRound(cvGetReal1D(hist->bins,i))),
                          cvScalarAll(0), -1, 8, 0 );

     cvReleaseImage(&gray);
     cvReleaseHist(&hist);
     return(hist_image);
}


// Function for finding best threshold for dark background subtraction
int find_threshold(IplImage* img) {
    float max_value = 0;
    float min_value = 0;
    int hist_size = 256;
    float range_0[]={0,256};
    float* ranges[] = { range_0 };
    //cvSetImageROI(img, cvRect(45, 5, 230, 230));
    IplImage* img_gray = MakeGray(img);
    CvHistogram *hist;
    hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

    cvCalcHist( &img_gray, hist, 0, 0 );
    int max_idx = 0;
    int min_idx = 0;
    cvGetMinMaxHistValue( hist, &min_value, &max_value, &min_idx, &max_idx );
    //find threshold point
    bool toggle = true;
    int IndexForLookupdensity = max_idx;
    //int index_temp=max_idx;
    float bin_diff=0;
    float histValue,histValue3;
    //float bin_diff_temp=0;
    //check background is dark or light
    while (toggle){
        if  (max_idx<hist_size/2){
            IndexForLookupdensity++;// For dark background, max_idx is lower than the hist_size/2
            histValue3 = cvQueryHistValue_1D( hist,IndexForLookupdensity+3);
        }else{
            IndexForLookupdensity--;// For light background, max_idx is lower than the hist_size/2
            histValue3 = cvQueryHistValue_1D( hist,IndexForLookupdensity-3);
        }
       histValue= cvQueryHistValue_1D( hist,IndexForLookupdensity);
         if ((max_value-histValue>bin_diff)||(max_value-histValue3>bin_diff)){
        //if (max_value-histValue>bin_diff){
              bin_diff = max_value-histValue;
              //bin_diff_temp = bin_diff;
              //index_temp = IndexForLookupdensity;
          }else if (bin_diff>(0.95*max_value)){
          //}else if (histValue<(3.0*min_value)){
          toggle = false;
        }
      }
     int my_threshold= IndexForLookupdensity;
     cvReleaseImage(&img_gray);
     cvReleaseHist(&hist);
     return my_threshold;
}

IplImage* skeleton(IplImage* img){
    //cvNot(img,img);
    IplImage* img_skeleton_32F = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
    IplImage* img_skeleton_8U = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
    cvDistTransform( img, img_skeleton_32F, CV_DIST_L2, CV_DIST_MASK_PRECISE, NULL,NULL );
    cvConvertScale( img_skeleton_32F,img_skeleton_8U, 1,0);
    cvAdaptiveThreshold ( img_skeleton_8U, img_skeleton_8U, 255, CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY, 9, 0 );
    //cvNot(img_skeleton_8U,img_skeleton_8U);
    erode_image(img_skeleton_8U);
    cvReleaseImage(&img_skeleton_32F);
    return img_skeleton_8U;
}

void erode_image(IplImage* &img){
    //cvNot(img,img);
    int KernelSize = 3;
    IplConvKernel* structuringElement;
    structuringElement = cvCreateStructuringElementEx( KernelSize, KernelSize, 1, 1,CV_SHAPE_CROSS, NULL );
    //cvMorphologyEx(img,img, NULL,structuringElement, CV_MOP_CLOSE, 3);
    cvErode(img,img,0,1);
    cvDilate(img,img,0,1);
    //cvNot(img,img);

}

//receives a binary image and new image size, finds bounding box and returns an image with
//the conents of bounding box which is located in the center.
bool normalize(IplImage* img, IplImage* &img_normal, IplImage* &img_normal_thresh,int new_width, int new_height){
    bool empty_img = false;
    IplImage* result_img;
    IplImage* scaledResult_img;
    IplImage* result;
    IplImage* scaledResult;
    CvMat data_img;
    CvMat data;
    CvMat data_img_dst;
    CvMat data_dst;
    CvRect bb;//bounding box
    int threshold= find_threshold(img);
    IplImage* img_thresh = mythreshold(img,threshold,1);
    //Find bounding box
    bb = cvBoundingRect(img_thresh);
    //qDebug() <<bb.x << bb.y << "bb.width= " << bb.width << "bb.height" << bb.height;
    //Get bounding box data and no with aspect ratio, the x and y can be corrupted
    if((bb.height<1)||(bb.width<1)){
        cvReleaseImage(&img_thresh);
        empty_img = true;
        return empty_img;
    }else{
        cvGetSubRect(img_thresh, &data, cvRect(bb.x, bb.y, bb.width, bb.height));
        cvGetSubRect(img, &data_img, cvRect(bb.x, bb.y, bb.width, bb.height));

        //Create image with this data with width and height with aspect ratio 1
        //then we get highest size betwen width and height of our bounding box
        int size=(bb.width>bb.height)?bb.width:bb.height;
        result=cvCreateImage( cvSize( size, size ), IPL_DEPTH_8U, img_thresh->nChannels );
        result_img=cvCreateImage( cvSize( size, size ), IPL_DEPTH_8U, img->nChannels );
        cvZero(result);
        cvZero(result_img);
        //Copy the data in center of image
        int x=(int)floor((float)(size-bb.width)/2.0f);
        int y=(int)floor((float)(size-bb.height)/2.0f);
        cvGetSubRect(result, &data_dst, cvRect(x,y,bb.width, bb.height));
        cvCopy(&data, &data_dst, NULL);
        cvGetSubRect(result_img, &data_img_dst, cvRect(x,y,bb.width, bb.height));
        cvCopy(&data_img, &data_img_dst, NULL);
        //Result_skeleton= skeleton(result);
        //cvShowImage("Result_skeleton",Result_skeleton);
        //Scale result
        scaledResult=cvCreateImage( cvSize( new_width, new_height ), 8, img_thresh->nChannels );
        scaledResult_img= cvCreateImage( cvSize( new_width, new_height ), 8, img->nChannels );
        cvResize(result, scaledResult, CV_INTER_NN);
        cvResize(result_img, scaledResult_img, CV_INTER_NN);
        cvReleaseImage(&result);
        cvReleaseImage(&result_img);

        //Return processed data
        img_normal_thresh = scaledResult;
        img_normal = scaledResult_img;
        cvReleaseImage(&img_thresh);
        empty_img = false;
        return empty_img;
    }

}

//Centralize an image based on its threshold
IplImage* normalizeImg(IplImage* img,int new_width, int new_height){
    IplImage* result_img = 0;
    IplImage* scaledResult_img = 0;

    CvMat data_img;
    CvMat data_img_dst;

    CvRect bb;//bounding box
    int threshold= find_threshold(img);
    IplImage* img_thresh = mythreshold(img,threshold,1);
    //Find bounding box
    bb = cvBoundingRect(img_thresh);
    //Get bounding box data and no with aspect ratio, the x and y can be corrupted
    if((bb.height<1)||(bb.width<1)){
        cvReleaseImage(&img_thresh);
        return scaledResult_img;
    }else{
        cvSmooth( img_thresh, img_thresh, CV_GAUSSIAN, 21, 21 );
        IplImage* img_thresh_smooth = mythreshold(img_thresh,150,0);
        cvNot(img_thresh_smooth,img_thresh_smooth);
        cvSet(img, CV_RGB(0,0,0),img_thresh_smooth);
        cvGetSubRect(img, &data_img, cvRect(bb.x, bb.y, bb.width, bb.height));

        //Create image with this data with width and height with aspect ratio 1
        //then we get highest size betwen width and height of our bounding box
        int size=(bb.width>bb.height)?bb.width:bb.height;

        result_img=cvCreateImage( cvSize( size, size ), img->depth , img->nChannels );

        cvZero(result_img);
        //Copy the data in center of image
        int x=(int)floor((float)(size-bb.width)/2.0f);
        int y=(int)floor((float)(size-bb.height)/2.0f);
        cvGetSubRect(result_img, &data_img_dst, cvRect(x,y,bb.width, bb.height));
        cvCopy(&data_img, &data_img_dst, NULL);



        //Scale result
        scaledResult_img= cvCreateImage( cvSize( new_width, new_height ), img->depth, img->nChannels );
        cvResize(result_img, scaledResult_img, CV_INTER_NN);
        cvReleaseImage(&result_img);
        cvReleaseImage(&img_thresh);
        cvReleaseImage(&img_thresh_smooth);
        AddBorder(scaledResult_img);
        return scaledResult_img;
    }

}

IplImage* preprocessing(IplImage* img){
    IplImage* img_pp=0;
    int threshold= find_threshold(img);
    IplImage* img_thresh = mythreshold(img,threshold,1);
/*
    //Equalized mode
    cvNot(img_thresh,img_thresh);
    cvSet(img, CV_RGB(0,0,0),img_thresh);
    IplImage* img_gray = MakeGray(img);
    cvSetImageROI(img_gray,findBB(img_gray));
    cvEqualizeHist( img_gray, img_gray );
    cvResetImageROI(img_gray);
     cvReleaseImage(&img_thresh);
*/
    //skeleton mode
    //IplImage* img_src_threshl_skel = skeleton(img_src_thresh);
    //
    img_pp = img_thresh;
    //img_pp = img_gray;
    return img_pp;
}

// Rotate the image clockwise (or counter-clockwise if negative).
// Remember to free the returned image.
IplImage *rotateImage(const IplImage *img, float angleDegrees)
{
        // Create a map_matrix, where the left 2x2 matrix
        // is the transform and the right 2x1 is the dimensions.
        float m[6];
        CvMat M = cvMat(2, 3, CV_32F, m);
        int w = img->width;
        int h = img->height;
        float angleRadians = angleDegrees * ((float)CV_PI / 180.0f);
        m[0] = (float)( cos(angleRadians) );
        m[1] = (float)( sin(angleRadians) );
        m[3] = -m[1];
        m[4] = m[0];
        m[2] = w*0.5f;
        m[5] = h*0.5f;

        // Make a spare image for the result
        CvSize sizeRotated;
        sizeRotated.width = cvRound(w);
        sizeRotated.height = cvRound(h);

        // Rotate
        IplImage *imageRotated = cvCreateImage( sizeRotated, img->depth, img->nChannels );
        // Transform the image
        cvGetQuadrangleSubPix( img, imageRotated, &M);
        IplImage* imageRotated_normal=0;
        IplImage* imageRotated_threshold=0;
        normalize(imageRotated,imageRotated_normal,imageRotated_threshold,sizeRotated.width,sizeRotated.height);
        cvReleaseImage(&imageRotated);
        cvReleaseImage(&imageRotated_threshold);
        return imageRotated_normal;
}

int findObjects(const char *cascade_path,IplImage *img, CvRect** &objectArr ){
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad( cascade_path );

        CvMemStorage* storage = cvCreateMemStorage(0);
        CvSeq* objects;
        int i;
        /* use the fastest variant */
        objects = cvHaarDetectObjects( img, cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING );
        objectArr=(CvRect**)cvAlloc(sizeof(CvRect*)*objects->total);
        /* draw all the rectangles */
        for( i = 0; i < objects->total; i++ )
        {
            /* extract the rectanlges only */


            // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( objects, i );
            //CvRect* r =  objectArr[i];
            objectArr[i] = r;

        }
        cvReleaseMemStorage( &storage );
        return objects->total;
    }

void drawRect(IplImage *img, CvRect* rect,float scale){
    CvPoint pt1, pt2, center;
    pt1.x = rect->x;
    pt2.x = rect->x+rect->width;
    pt1.y = rect->y;
    pt2.y = rect->y+rect->height;
    center.x= (pt1.x+pt2.x)/2;
    center.y= (pt1.y+pt2.y)/2;
    pt1.x = center.x - (rect->width)*(scale/2);
    pt1.y = center.y - (rect->height)*(scale/2);
    pt2.x = center.x + (rect->width)*(scale/2);
    pt2.y = center.y + (rect->height)*(scale/2);

    // Draw the rectangle in the input image
    cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 2, 8, 0 );
    cvCircle(img,center,2,CV_RGB(255,0,0),3,8,0);
}

CvHistogram* hsvHistogram(IplImage* img)
{
    //Image Properties
    CvSize ImageSize = cvGetSize(img);
    // Compute HSV image and separate into colors
    IplImage* img_hsv = cvCreateImage( ImageSize, IPL_DEPTH_8U, 3 );
    cvCvtColor( img, img_hsv, CV_BGR2HSV );

    IplImage* h_plane = cvCreateImage( ImageSize, IPL_DEPTH_8U, 1 );
    IplImage* s_plane = cvCreateImage( ImageSize, IPL_DEPTH_8U, 1 );
    IplImage* v_plane = cvCreateImage( ImageSize, IPL_DEPTH_8U, 1 );
    IplImage* planes[] = { h_plane, s_plane };
    cvCvtPixToPlane( img_hsv, h_plane, s_plane, v_plane, 0 );

    // Build and fill the histogram
    int h_bins = 30, s_bins = 32;
    CvHistogram* hist;
    {
        int hist_size[] = { h_bins, s_bins };
        float h_ranges[] = { 0, 180 };
        float s_ranges[] = { 0, 255 };
        float* ranges[] = { h_ranges, s_ranges };
        hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
    }
    cvCalcHist( planes, hist, 0, 0 ); // Compute histogram

    cvReleaseImage(&img_hsv);
    cvReleaseImage(&h_plane);
    cvReleaseImage(&s_plane);
    cvReleaseImage(&v_plane);
    return hist;
}

IplImage* BackProjection(IplImage* img, CvHistogram* hist)
{
        //Image Properties
        CvSize ImageSize = cvGetSize(img);
        int ImageDepth = img->depth;

        IplImage* img_bp = cvCreateImage(ImageSize, ImageDepth, 1 );
        // Compute HSV image and separate into colors
        IplImage* img_hsv = cvCreateImage( cvGetSize(img), ImageDepth, 3 );
        cvCvtColor( img, img_hsv, CV_BGR2HSV );

        IplImage* h_plane = cvCreateImage( ImageSize, IPL_DEPTH_8U, 1 );
        IplImage* s_plane = cvCreateImage( ImageSize, IPL_DEPTH_8U, 1 );
        IplImage* v_plane = cvCreateImage( ImageSize, IPL_DEPTH_8U, 1 );
        IplImage* planes[] = { h_plane, s_plane };
        cvCvtPixToPlane( img_hsv, h_plane, s_plane, v_plane, 0 );

        cvCalcBackProject( planes, img_bp, hist );// Calculate back projection
        // Show back projection
        //cvNamedWindow( "Back Projection", 1) ;
        //cvShowImage( "Back Projection", img_bp );
        cvReleaseImage(&img_hsv);
        cvReleaseImage(&h_plane);
        cvReleaseImage(&s_plane);
        cvReleaseImage(&v_plane);
        return (img_bp);

}

CvHistogram* FindSkinHist(IplImage* img_colored,const char* cascade_file){
CvHistogram* hist_skin=0;
//char cascade_file[255];
IplImage* img_gray = MakeGray(img_colored);
//sprintf(cascade_file,"%s%s",cascades_path, "aGest.xml");
CvRect **objects;
 int nObjects= findObjects(cascade_file,img_gray,objects);
 qDebug() << "nObjects ="<< nObjects;
  if (nObjects>0){
  CvRect mybb = *objects[0];
    IplImage* skin_template = GetSubImage(img_colored,mybb, 0.6);
/*
    IplImage* skin_thresh = mythreshold(skin_template,10,1);
    cvNot(skin_thresh,skin_thresh);
    cvSet(skin_template, CV_RGB(255,255,255),skin_thresh);
    cvReleaseImage(&skin_thresh);
*/
    cvShowImage("skin_template",skin_template);
    if (mybb.width > 200 && mybb.height >200){
        hist_skin = hsvHistogram(skin_template);
        qDebug() << "Object: "<< mybb.width << mybb.height;
        }
    cvReleaseImage(&skin_template);
  }

  cvReleaseImage(&img_gray);
  return hist_skin;
}

CvRect findBB(IplImage* img){
    IplImage* img_gray = MakeGray(img);
    CvRect bb= cvBoundingRect(img_gray);
    cvReleaseImage(&img_gray);
    return bb;
}

void resetImage(IplImage* &img){
    cvReleaseImage(&img);
    img=0;
}

void trimImage(IplImage* &img,int trimValue){
    IplImage* imgTrimed = 0;
    CvRect bb;
    bb.width=0;
    bb.height=0;
    bb = findBB(img);
    if((bb.height>10)&&(bb.width>10)){
        //removing black border for some images
        bb.x+=trimValue; bb.y+=trimValue; bb.width-=2*trimValue; bb.height-=2*trimValue;
        imgTrimed = GetSubImage(img,bb,1);
        //IplImage *imgClone = cvCreateImage(cvGetSize(img),img->depth,img->nChannels);
        cvResize(imgTrimed,img,CV_INTER_NN);
        //img = imgClone;
        cvReleaseImage(&imgTrimed);
    }else{
        img = 0;
        //qDebug()<< "image is empty!";
    }
}
