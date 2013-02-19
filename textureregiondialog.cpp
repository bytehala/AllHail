#include "textureregiondialog.h"
#include "ui_textureregiondialog.h"

TextureRegionDialog::TextureRegionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureRegionDialog)
{
    ui->setupUi(this);
}

TextureRegionDialog::~TextureRegionDialog()
{
    delete ui;
}
