#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QListWidget>
#include <QFileInfoList>
#include <QLabel>
#include "newprojectdialog.h"

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
    void updatePreview(QListWidgetItem* item);
    void addToScene();
    void sceneItemDragged();
    void selectAnother();
    void snapUp();
    void snapDown();
    void snapLeft();
    void snapRight();
    void updateStatusBarText();


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
    NewProjectDialog *dialog;

    static const QString ITEMS_TO_LEFT;// = "itemsToLeft";
    static const QString ITEMS_ABOVE;// = "itemsAbove";

    void setPreviewScene(QGraphicsScene*);
    void updatePreview(int);
    void zoomOut();
    void zoomIn();
    void createProject();
    bool correctCollisions(QGraphicsItem*, enum SnapDirection);
    void finalizeCollisions(QGraphicsItem*, enum SnapDirection);
};

#endif // MAINWINDOW_H
