// This GUI helper contains common snippets which is used in GUI forms
#include "guihelper.h"

void centerWidgetOnScreen (QWidget * widget) {
     QRect rect = QApplication::desktop()->availableGeometry();
     widget->move(rect.center() - widget->rect().center());
}


void qimageWidget(QString name,QImage qimg,IplImageWidget* widget){
    double hFactor = double(widget->height())/double(qimg.height());
    double wFactor = double(widget->width())/double(qimg.width());
    double resizeFactor = (wFactor<hFactor)? wFactor:hFactor;
    QImage qimgScaled =qimg.scaled(qimg.width()*resizeFactor*0.85,qimg.height()*resizeFactor*0.85);
    //qDebug() << "new size" << qimgScaled.width() << qimgScaled.height() ;
    widget->putQtImage(name,qimgScaled);
}

void imageWidget(QString name,IplImage* img,IplImageWidget* widget){
    if (img){
        QImage qtImage;
        double hFactor = double(widget->height())/double(img->height);
        double wFactor = double(widget->width())/double(img->width);
        double resizeFactor = (wFactor<hFactor)? wFactor:hFactor;
        IplImage* img_resized =imgResize(img,resizeFactor*0.85);
        IplImage* img_rgb = MakeQtImage(img_resized,qtImage);
        widget->putQtImage(name,qtImage);
        cvReleaseImage(&img_resized);
        cvReleaseImage(&img_rgb);
    }else{
        QImage qimgColorBar;
        qimgColorBar.load(":/images/resources/images/colorbar.png");
        qimageWidget(name,qimgColorBar,widget);
    }
}
