#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QListWidget>
#include <QFileInfoList>
#include <QLabel>
#include "newprojectdialog.h"
#include "testoutputdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void MainWindow::keyPressEvent(QKeyEvent* event);
    bool MainWindow::eventFilter(QObject *object, QEvent *event);

public slots:
    void newProjectDialog();
    void showTestOutput();
    void updatePreview(QListWidgetItem* item);
    void addToScene();
    void sceneItemDragged();
    void selectAnother();
    void snapUp();
    void snapDown();
    void snapLeft();
    void snapRight();
    void updateStatusBarText();
    void drawAtlasBox(int, int);
    void updateItemProperties();
    void receiveDirAndAtlasName(QString, QString);
    QString createAndEngineCode(); // TODO: temporary??
    void closeProject();

signals:
    void dataShouldUpdate(QString, int, int, bool);
    void sendTestOutput(QString);


private:
    Ui::MainWindow *ui;
    QGraphicsView *atlasView_;
    QGraphicsView *previewView_;
    QListWidget *imagesListWidget_;
    QGraphicsItem* selectedGraphicsItem_;
    QList<QGraphicsItem*> selectedItems_;

    QString projectPath_;
    QList<QString> imageFileNames_;
    QList<QGraphicsScene*> previewList_;
    QFileInfoList fileNames_;
    QGraphicsScene* atlasScene_;
    QRectF currentAtlasRect_;
    QLabel* statusBarText_;
    bool isScaledDown_;
    float currentScale_;
    bool isProjectOpen_;
//    NewProjectDialog *dialog;
    QString andEngineCode_;

    // Parsing related
    QString atlasName_;
//    QString createAndEngineCode();

    static const QString ITEMS_TO_LEFT;// = "itemsToLeft";
    static const QString ITEMS_ABOVE;// = "itemsAbove";
    static const QString TEXREG_NROWS;
    static const QString TEXREG_NCOLUMNS;
    static const QString TEXREG_VARNAME;
    static const QString TEXREG_IS_TILED;
    static const QString TEXREG_RESOURCENAME;

    void setPreviewScene(QGraphicsScene*);
    void updatePreview(int);
    void zoomOut();
    void zoomIn();
    void createProject();
    bool correctCollisions(QGraphicsItem*, enum SnapDirection);
    void finalizeCollisions(QGraphicsItem*, enum SnapDirection);
    void updatePropertiesData(QGraphicsItem*);
    void emptyPropertiesData();
    void deleteItem();
    void enableSnapButtons(bool);
};

#endif // MAINWINDOW_H
