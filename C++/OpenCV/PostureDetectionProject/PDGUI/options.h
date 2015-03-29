#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
    class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

private:
    Ui::Options *ui;

private slots:
    void on_btnSelectTrainFolder_clicked();
    void on_btnSelectTestFolder_clicked();

};

#endif // OPTIONS_H
