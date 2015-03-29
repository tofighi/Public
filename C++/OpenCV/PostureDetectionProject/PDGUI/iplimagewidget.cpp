#include "iplimagewidget.h"

IplImageWidget::IplImageWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QVBoxLayout;
    imagelabel = new QLabel;
    label = new QLabel;
    QImage image(0,0,QImage::Format_RGB888);
    layout->addWidget(imagelabel,0,Qt::AlignCenter);
    layout->addWidget(label,1,Qt::AlignCenter);
    layout->setAlignment(Qt::AlignCenter);
    imagelabel->setPixmap(QPixmap::fromImage(image));
    imagelabel->setStyleSheet("border-style: hidden; border-width: 3px;");
    setLayout(layout);
}

void IplImageWidget::putQtImage(QString name,QImage image)
{
    label->setText(name);
    imagelabel->setPixmap(QPixmap::fromImage(image));
}




