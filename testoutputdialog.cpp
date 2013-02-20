#include "testoutputdialog.h"
#include "ui_testoutputdialog.h"

TestOutputDialog::TestOutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestOutputDialog)
{
    ui->setupUi(this);
}

TestOutputDialog::~TestOutputDialog()
{
    delete ui;
}

void TestOutputDialog::receiveText(const QString text)
{
    ui->plainTextEdit->setPlainText(text);
}
