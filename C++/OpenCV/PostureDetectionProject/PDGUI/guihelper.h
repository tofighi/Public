// This GUI helper contains common snippets which is used in GUI forms
#ifndef GUIHELPER_H
#define GUIHELPER_H
#include <QtGui/QApplication>
#include <QDebug>
#include <QDialog>
#include <QtGui>
#include "iplimagewidget.h"
#include "helper.h"

void centerWidgetOnScreen (QWidget * widget);
void qimageWidget(QString name,QImage qimg,IplImageWidget* widget);
void imageWidget(QString name,IplImage* img,IplImageWidget* widget);

#endif // GUIHELPER_H
