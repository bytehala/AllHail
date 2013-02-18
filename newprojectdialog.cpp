#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include <QDir>
#include <QFileDialog>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
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
