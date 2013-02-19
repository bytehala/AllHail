#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
    QIntValidator* validator = new QIntValidator(1, 1024);
    ui->widthText->setValidator(validator);
    ui->heightText->setValidator(validator);
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}


void NewProjectDialog::launchFileDialog()
{
    QDir directory;
    QString path = QFileDialog::getExistingDirectory (this, tr("Directory"), directory.path());
//    projectPath_ = path;
    ui->projectPathText->setText(path);
//    createProject();
}

void NewProjectDialog::accept()
{
    // Checkers

    if(!(ui->widthText->text().isEmpty() ||
       ui->heightText->text().isEmpty() ||
       ui->atlasNameText->text().isEmpty() ||
       ui->projectPathText->text().isEmpty()))
    {
        int width = ui->widthText->text().toInt();
        int height = ui->heightText->text().toInt();
        if( width > 0 && height > 0)
        {
            emit widthAndHeightValidated(width, height);
            emit sendAtlasName(ui->atlasNameText->text());
            QDialog::accept();
        }
        else
        {
        }
    }
}


void NewProjectDialog::reject()
{
    qDebug() << "Rejected";
    QDialog::reject();
}
