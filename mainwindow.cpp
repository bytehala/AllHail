#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <Qimage>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QFileDialog>
#include <QJsonObject>

enum SnapDirection
{
    SNAP_UP,
    SNAP_DOWN,
    SNAP_LEFT,
    SNAP_RIGHT
};

const QString MainWindow::ITEMS_TO_LEFT("itemsToLeft");
const QString MainWindow::ITEMS_ABOVE("itemsAbove");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isScaledDown_ = false;

    ui->setupUi(this);
    showMaximized();

    imagesListWidget_ = ui->listWidget;
    previewView_ = ui->previewView;

    QImage image;
    image.load("C:/Users/lemuel.dulfo/workspace/RuleThemAll/assets/gfx/face_triangle_tiled.png");
    atlasScene_ = new QGraphicsScene();
    atlasScene_->setSceneRect(0, 0, 100, 100);
    atlasView_ = ui->atlasView;
    atlasView_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    atlasView_->setScene(atlasScene_);
    atlasScene_->addPixmap(QPixmap::fromImage(image))->setFlags((QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable));
    currentAtlasRect_ = QRectF(0, 0, 512, 512);
    atlasScene_->addLine(513, 0, 513, 513);
    atlasScene_->addLine(0, 513, 513, 513);

    statusBarText_ = new QLabel();
    statusBarText_->setText("blah");
    statusBarText_->setAlignment(Qt::AlignLeft);
    statusBar()->addWidget(statusBarText_);
//    atlasScene_->installEventFilter(this);

    // DEBUG CODE
    projectPath_ = "C:/Users/lemuel.dulfo/workspace/RuleThemAll" ;
    createProject();

    connect(atlasScene_, SIGNAL(selectionChanged()), this, SLOT(selectAnother()));
    connect(atlasScene_, SIGNAL(selectionChanged()), this, SLOT(updateStatusBarText()));
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
 {
    if (object == atlasScene_) {
        switch(event->type())
        {
        case 43:
        {
            QMouseEvent* mouseEvent = (QMouseEvent*) event;
            qDebug() << "Dragging: (" << mouseEvent->x() << "," << mouseEvent->y() << ")";
            break;
        }
        case 157:
        {
            qDebug() << "MouseUp";//event->type();

            //         == QEvent::MouseButtonRelease
            //         qDebug() << event->type();
            return false;
        }
        }
    }
    return false;
 }

void MainWindow::newProjectDialog()
{
    NewProjectDialog dialog(this);
    dialog.exec();
}


void MainWindow::createProject()
{
    if ( projectPath_.isNull() == false )
    {
        QFile myFile("proj1.ahp");
        myFile.open(QIODevice::WriteOnly);
        QString message = "<allhailproject>";
        message += "\n";
        message += "<rootdir value=\"";
        message += projectPath_;
        message += "\" />";
        message += "\n";
        message += "</allhailproject>";
        myFile.write(message.toUtf8().constData());
        myFile.close();

        QDir gfxDir(projectPath_ + "\\assets\\gfx\\");
        fileNames_ = gfxDir.entryInfoList(QStringList("*.png"));
        int index = -1;
        foreach(QFileInfo info , fileNames_)
        {
            imageFileNames_.append(info.absoluteFilePath()); // Maybe this is not necessary anymore

            // Populate images list
            QListWidgetItem* item = new QListWidgetItem(info.baseName());
            item->setData(Qt::UserRole, ++index);
            imagesListWidget_->addItem(item);

            // Populate preview list
            QImage image;
            image.load(info.absoluteFilePath());
            QGraphicsScene* scene = new QGraphicsScene();
            scene->addPixmap(QPixmap::fromImage(image));
            previewList_.append(scene);
        }

        updatePreview(0);
    }
}

// Slots

void MainWindow::updateStatusBarText()
{
    QList<QGraphicsItem*> selectedItems = atlasScene_->selectedItems();
    foreach(QGraphicsItem* item, selectedItems)
    {
        QString textDisplayed = "";
        QPointF point = item->scenePos();
        textDisplayed += "(";

        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
        int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();

        textDisplayed += QString::number(point.x() - leftIndex*0.5);
        textDisplayed += ",";
        textDisplayed += QString::number(point.y() - topIndex*0.5);
        textDisplayed += ")";

//        statusBar()->removeWidget(statusBarText_);
        statusBarText_->setText(textDisplayed);
//        statusBar()->addWidget(statusBarText_);
    }
}

void MainWindow::snapUp()
{
    QList<QGraphicsItem*> selectedItems = atlasScene_->selectedItems();
    foreach(QGraphicsItem* item, selectedItems)
    {
        item->setY(0);
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
        int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
        userData.insert(ITEMS_ABOVE, topIndex);
        userData.insert(ITEMS_TO_LEFT, leftIndex);
        item->setData(Qt::UserRole, userData);
        correctCollisions(item, SNAP_UP);
//        finalizeCollisions(item, SNAP_UP);
    }
}

void MainWindow::snapDown()
{
    QList<QGraphicsItem*> selectedItems = atlasScene_->selectedItems();
    foreach(QGraphicsItem* item, selectedItems)
    {
        item->setY(currentAtlasRect_.height() - item->boundingRect().height());
        item->setData(Qt::UserRole, 0);
        correctCollisions(item, SNAP_DOWN);
        //finalizeCollisions(item, SNAP_DOWN);
    }
}

void MainWindow::snapLeft()
{
    QList<QGraphicsItem*> selectedItems = atlasScene_->selectedItems();
    foreach(QGraphicsItem* item, selectedItems)
    {
        item->setX(0);
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
        int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
        userData.insert(ITEMS_ABOVE, topIndex);
        userData.insert(ITEMS_TO_LEFT, leftIndex);
        correctCollisions(item, SNAP_LEFT);
        //finalizeCollisions(item, SNAP_LEFT);
    }
}

void MainWindow::snapRight()
{
    QList<QGraphicsItem*> selectedItems = atlasScene_->selectedItems();
    foreach(QGraphicsItem* item, selectedItems)
    {
        item->setX(currentAtlasRect_.width() - item->boundingRect().width());
        item->setData(Qt::UserRole, 0);
        correctCollisions(item, SNAP_RIGHT);
        //finalizeCollisions(item, SNAP_RIGHT);
    }
}

void MainWindow::selectAnother()
{
    selectedItems_ = atlasScene_->selectedItems();
    if(selectedGraphicsItem_ == NULL)
    {
        selectedGraphicsItem_ = selectedItems_.at(0);
    }
    else // There is exactly one selected item
    {
        // This is good enough.
        atlasScene_->clearSelection();
        selectedGraphicsItem_ = NULL;
    }
}

void MainWindow::sceneItemDragged()
{
    qDebug() << "dragging...";
}

void MainWindow::addToScene()
{
    QList<QListWidgetItem*> selectedItems = imagesListWidget_->selectedItems();
    foreach(QListWidgetItem* item, selectedItems)
    {
        QImage image;
        image.load(imageFileNames_.at(item->data(Qt::UserRole).toInt()));
        // TODO: isMovable should be removed on release
        QPixmap pixMap = QPixmap::fromImage(image);
        if(pixMap.width() > currentAtlasRect_.width() || pixMap.height() > currentAtlasRect_.height())
            qDebug() << "Cannot add larger image.";
        else
        {

            atlasScene_->addPixmap(pixMap)->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        }
    }
}

void MainWindow::updatePreview(int itemIndex)
{
    setPreviewScene(previewList_.at(itemIndex));
}

void MainWindow::updatePreview(QListWidgetItem *item){
    setPreviewScene(previewList_.at(item->data(Qt::UserRole).toInt()));
}

void MainWindow::setPreviewScene(QGraphicsScene* scene)
{
    previewView_->setScene(scene);
    if(isScaledDown_)
    {
        previewView_->scale(1/currentScale_, 1/currentScale_);
        isScaledDown_ = false;
    }
    if(scene->height() > previewView_->height() || scene->width() > previewView_->width())
    {
        float lower = std::min(previewView_->height()/scene->height(), previewView_->width()/scene->width());
        lower *= 0.9f;
        currentScale_ = lower;
        previewView_->scale(lower, lower);
        isScaledDown_ = true;
    }
}

// Key Events because I don't know how to handle MouseWheel yet
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case '-':
        zoomOut();
        break;
    case '+':
        zoomIn();
        break;
    case 16777216: // Esc key
        this->close();
        //qDebug() << event->key();
    }
}

void MainWindow::zoomOut()
{
    zoomIn();
    atlasView_->scale(.8, .8);
}

void MainWindow::zoomIn()
{
    atlasView_->scale(1.1, 1.1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Helper methods
bool MainWindow::correctCollisions(QGraphicsItem* selectedItem, SnapDirection snapDirection)
{
    // TODO: Set QVariant(Object) userData to know how many 0.5 to deduct
    QList<QGraphicsItem*> collidingItems = atlasScene_->collidingItems(selectedItem);
    qreal min_max = 0;
    if(snapDirection == SNAP_DOWN)
    {
        min_max = currentAtlasRect_.height();
    }
    else if(snapDirection == SNAP_RIGHT)
    {
        min_max = currentAtlasRect_.width();
    }

    foreach(QGraphicsItem* collidingItem, collidingItems)
    {
        switch(snapDirection)
        {
        case SNAP_UP:
        {
            qreal lowest = collidingItem->sceneBoundingRect().bottom();
            if(lowest > min_max)
            {
                QJsonObject userData = selectedItem->data(Qt::UserRole).toJsonObject();
                int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
                int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
                userData.insert(ITEMS_ABOVE, ++topIndex);
                userData.insert(ITEMS_TO_LEFT, leftIndex);
                selectedItem->setData(Qt::UserRole, userData);
                min_max = lowest;
                selectedItem->setY(min_max);
            }
            break;
        }
        case SNAP_DOWN:
        {
            qreal highest = collidingItem->sceneBoundingRect().top();
            if(highest < min_max)
            {
                min_max = highest;
                selectedItem->setY(min_max - selectedItem->boundingRect().height());
            }
            break;
        }
        case SNAP_LEFT:
        {
            qreal left = collidingItem->sceneBoundingRect().right();
            if(left > min_max)
            {
                QJsonObject userData = selectedItem->data(Qt::UserRole).toJsonObject();
                int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
                int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
                userData.insert(ITEMS_ABOVE, topIndex);
                userData.insert(ITEMS_TO_LEFT, ++leftIndex);
                selectedItem->setData(Qt::UserRole, userData);
                min_max = left;
                selectedItem->setX(min_max);
            }
            break;
        }
        case SNAP_RIGHT:
        {
            qreal right = collidingItem->sceneBoundingRect().left();
            if(right < min_max)
            {
                min_max = right;
                selectedItem->setX(min_max - selectedItem->boundingRect().width());
            }
            break;
        }
        }
    }
    if(collidingItems.length() > 0)
    {
        correctCollisions(selectedItem, snapDirection);
    }

    return false;
}

// Adjust 0.5 boundingRect allowance
void MainWindow::finalizeCollisions(QGraphicsItem* selectedItem, SnapDirection snapDirection)
{
    return;
    switch(snapDirection)
    {
    case SNAP_UP:
    {
        qreal posY = selectedItem->scenePos().y();
        qDebug() << posY;
        selectedItem->setY(selectedItem->scenePos().y() - 0.5);
        break;
    }
    case SNAP_DOWN:
    {
        selectedItem->setY(selectedItem->sceneBoundingRect().top() + 0.5);
        break;
    }
    case SNAP_LEFT:
    {
        selectedItem->setX(selectedItem->scenePos().x() - 0.5);
        break;
    }
    case SNAP_RIGHT:
    {
//        selectedItem->setX(selectedItem->sceneBoundingRect().left() + 0.5);
        break;
    }
    }

}
