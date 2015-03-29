#ifndef IPLIMAGEWIDGET_H
#define IPLIMAGEWIDGET_H

#include <QWidget>
#include <cv.h>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>

class IplImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IplImageWidget(QWidget *parent = 0);
    void putQtImage(QString name,QImage image);
    QLabel *imagelabel;
    QLabel *label;
    QImage image;

signals:

public slots:

private:
    QVBoxLayout *layout;


};

#endif // IPLIMAGEWIDGET_H

