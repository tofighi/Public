#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);
}

Options::~Options()
{
    delete ui;
}


void Options::on_btnSelectTestFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Testing Samples Directory"),ui->edtTestFolder->text(),QFileDialog::ShowDirsOnly);
    if (dir!="") ui->edtTestFolder->setText(dir);
}

void Options::on_btnSelectTrainFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Training Samples Directory"),ui->edtTrainFolder->text(),QFileDialog::ShowDirsOnly);
    if (dir!="") ui->edtTrainFolder->setText(dir);
}


