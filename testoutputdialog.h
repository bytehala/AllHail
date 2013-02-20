#ifndef TESTOUTPUTDIALOG_H
#define TESTOUTPUTDIALOG_H

#include <QDialog>

namespace Ui {
class TestOutputDialog;
}

class TestOutputDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TestOutputDialog(QWidget *parent = 0);
    ~TestOutputDialog();

public slots:
    void receiveText(const QString);
    
private:
    Ui::TestOutputDialog *ui;
};

#endif // TESTOUTPUTDIALOG_H
