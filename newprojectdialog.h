#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

public slots:
    void launchFileDialog();
    void accept();
    void reject();

signals:
    void widthAndHeightValidated(int, int);
    void sendAtlasName(QString);
    
private:
    Ui::NewProjectDialog *ui;
};

#endif // NEWPROJECTDIALOG_H
