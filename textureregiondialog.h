#ifndef TEXTUREREGIONDIALOG_H
#define TEXTUREREGIONDIALOG_H

#include <QDialog>

namespace Ui {
class TextureRegionDialog;
}

class TextureRegionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TextureRegionDialog(QWidget *parent = 0);
    ~TextureRegionDialog();
    
private:
    Ui::TextureRegionDialog *ui;
};

#endif // TEXTUREREGIONDIALOG_H
