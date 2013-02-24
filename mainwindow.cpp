#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <Qimage>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QFileDialog>
#include <QJsonObject>
#include <QXmlStreamWriter>

enum SnapDirection
{
    SNAP_UP,
    SNAP_DOWN,
    SNAP_LEFT,
    SNAP_RIGHT
};

const QString MainWindow::ITEMS_TO_LEFT("itemsToLeft");
const QString MainWindow::ITEMS_ABOVE("itemsAbove");

const QString MainWindow::TEXREG_NROWS("nRows");
const QString MainWindow::TEXREG_NCOLUMNS("nColumns");
const QString MainWindow::TEXREG_VARNAME("varName");
const QString MainWindow::TEXREG_IS_TILED("bIsTiled");
const QString MainWindow::TEXREG_RESOURCENAME("resName");

const QString MainWindow::PROJECT_ELEMENT("AllHailProject");
const QString MainWindow::PROJECT_ATTR_LOCATION("projectLocation");
const QString MainWindow::PROJECT_ATTR_MAINCLASS("mainClass");
const QString MainWindow::ATLAS_ELEMENT("Atlas");
const QString MainWindow::ATLAS_ATTR_NAME("name");
const QString MainWindow::ATLAS_ATTR_WIDTH("width");
const QString MainWindow::ATLAS_ATTR_HEIGHT("height");
const QString MainWindow::TEXTUREREGION_ELEMENT("TextureRegion");
const QString MainWindow::TEXTUREREGION_ATTR_TILED("isTiled");
const QString MainWindow::TEXTUREREGION_ATTR_ROWS("rows");
const QString MainWindow::TEXTUREREGION_ATTR_COLUMNS("columns");
const QString MainWindow::TEXTUREREGION_ATTR_RESNAME("resName");
const QString MainWindow::TEXTUREREGION_ATTR_ITEMNAME("itemName");
const QString MainWindow::TEXTUREREGION_ATTR_SCENEX("sceneX");
const QString MainWindow::TEXTUREREGION_ATTR_SCENEY("sceneY");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isProjectOpen_ = false;
    isScaledDown_ = false;

    ui->setupUi(this);
    showMaximized();

    QIntValidator* validator = new QIntValidator(1, 100);
    ui->nRowsText->setValidator(validator);
    ui->nColumnsText->setValidator(validator);

    imagesListWidget_ = ui->listWidget;
    previewView_ = ui->previewView;

    QImage image;
    image.load("C:/Users/lemuel.dulfo/workspace/RuleThemAll/assets/gfx/face_triangle_tiled.png");
    atlasScene_ = new QGraphicsScene();
    atlasScene_->setBackgroundBrush(Qt::gray);
    atlasScene_->setSceneRect(0, 0, 100, 100);
    atlasView_ = ui->atlasView;
    atlasView_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    atlasView_->setScene(atlasScene_);
    atlasScene_->addPixmap(QPixmap::fromImage(image))->setFlags((QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable));

    statusBarText_ = new QLabel();
    statusBarText_->setText("blah");
    statusBarText_->setAlignment(Qt::AlignLeft);
    statusBar()->addWidget(statusBarText_);
//    atlasScene_->installEventFilter(this);

    // DEBUG CODE
//    atlasName_ = "mBitmapTextureAtlas";
//    drawAtlasBox(800, 512);
//    projectPath_ = "C:/Users/lemuel.dulfo/workspace/FishDish" ;
//    createProject();

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
            return false;
        }
        }
    }
    return false;
 }

void MainWindow::showTestOutput()
{
    TestOutputDialog dialog(this);
    connect(this, SIGNAL(sendTestOutput(QString)), &dialog, SLOT(receiveText(QString)));
    emit sendTestOutput(andEngineCode_);
    //connect(&dialog, SIGNAL())
    dialog.exec();
}

void MainWindow::newProjectDialog()
{
    NewProjectDialog dialog(this);
    connect(&dialog, SIGNAL(widthAndHeightValidated(int, int)), this, SLOT(drawAtlasBox(int,int)));
    connect(&dialog, SIGNAL(sendDirAndAtlasName(QString, QString)), this, SLOT(receiveDirAndAtlasName(QString, QString)));
    dialog.exec();
}

void MainWindow::closeProject()
{
    if(isProjectOpen_)
    {
        atlasScene_->clear();
        atlasName_ = "";
        projectPath_ = "";
        imageFileNames_.clear();
        fileNames_.clear();
        previewView_->scene()->clear();
        previewList_.clear();
        imagesListWidget_->clear();
        currentAtlasRect_ = QRectF();

        isProjectOpen_ = false;
        ui->actionOpen_Project->setEnabled(true);
        ui->actionClose_Project->setEnabled(false);
        ui->menuProject->setEnabled(false);
        ui->actionNew_Project->setEnabled(true);
        ui->actionSave_Project->setEnabled(false);
    }
}

void MainWindow::createProject()
{
    if ( projectPath_.isEmpty() == false )
    {
        ui->actionTest_Output->setEnabled(true);
        isProjectOpen_ = true;
        ui->actionOpen_Project->setEnabled(false);
        ui->actionClose_Project->setEnabled(true);
        ui->menuProject->setEnabled(true);
        ui->actionNew_Project->setEnabled(false);
        ui->actionSave_Project->setEnabled(true);

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
//            QString dimensions = QString::number(image.width()) + "x" + QString::number(image.height());
            previewList_.append(scene);
        }

        //updatePreview(0);
    }
}

void MainWindow::saveProject()
{
    // TODO: make user select filename... not proj1.ahp
    QFile outputFile("proj1.ahp");
    outputFile.open(QIODevice::WriteOnly);

    QXmlStreamWriter stream(&outputFile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement(PROJECT_ELEMENT);
    stream.writeAttribute(PROJECT_ATTR_LOCATION, projectPath_);
    stream.writeAttribute(PROJECT_ATTR_MAINCLASS, "Test.java");

    stream.writeStartElement(ATLAS_ELEMENT);
    stream.writeAttribute(ATLAS_ATTR_NAME, atlasName_);
    stream.writeAttribute(ATLAS_ATTR_WIDTH, QString::number(currentAtlasRect_.width()));
    stream.writeAttribute(ATLAS_ATTR_HEIGHT, QString::number(currentAtlasRect_.height()));

    QList<QGraphicsItem*> allItems = atlasScene_->items(currentAtlasRect_);
    foreach(QGraphicsItem* item, allItems)
    {
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        int nRows = (int) userData.find(TEXREG_NROWS).value().toDouble();
        int nColumns = (int) userData.find(TEXREG_NCOLUMNS).value().toDouble();
        bool isTiled = userData.find(TEXREG_IS_TILED).value().toBool();
        QString resName = QFileInfo(userData.find(TEXREG_RESOURCENAME).value().toString()).fileName();
        QString itemName = userData.find(TEXREG_VARNAME).value().toString();
        QString tiledString("false");
        if(isTiled)
            tiledString = "true";

        // get x and y values
        QPointF point = item->scenePos();
        int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
        int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
        int sceneX = (point.x() - 0.5*leftIndex);
        int sceneY = (point.y() - 0.5*topIndex);

        stream.writeStartElement(TEXTUREREGION_ELEMENT);
        stream.writeAttribute(TEXTUREREGION_ATTR_TILED, tiledString);
        stream.writeAttribute(TEXTUREREGION_ATTR_ROWS, QString::number(nRows));
        stream.writeAttribute(TEXTUREREGION_ATTR_COLUMNS, QString::number(nColumns));
        stream.writeAttribute(TEXTUREREGION_ATTR_RESNAME, resName);
        stream.writeAttribute(TEXTUREREGION_ATTR_ITEMNAME, itemName);
        stream.writeAttribute(TEXTUREREGION_ATTR_SCENEX, QString::number(sceneX));
        stream.writeAttribute(TEXTUREREGION_ATTR_SCENEY, QString::number(sceneY));
        stream.writeAttribute(ITEMS_ABOVE, QString::number(topIndex));
        stream.writeAttribute(ITEMS_TO_LEFT, QString::number(leftIndex));
        stream.writeEndElement(); //TextureRegion
    }

    stream.writeEndElement(); //Atlas

    stream.writeEndElement(); // AllHailProject
    stream.writeEndDocument();

    outputFile.close();
}

void MainWindow::loadProject()
{
    qDebug() << "load";
    QFile inputFile("proj1.ahp");
    inputFile.open(QIODevice::ReadOnly);

     QXmlStreamReader xml(&inputFile);
     while (!xml.atEnd())
     {
         xml.readNext();
         // do processing

         if(xml.tokenString() != "StartElement")
             continue;
         if(xml.name().toString() == PROJECT_ELEMENT)
         {
             QXmlStreamAttributes attribs = xml.attributes();
             projectPath_ = attribs.value(PROJECT_ATTR_LOCATION).toString();
         }
         else if(xml.name().toString() == ATLAS_ELEMENT)
         {
             QXmlStreamAttributes attribs = xml.attributes();
             int atlasWidth = attribs.value(ATLAS_ATTR_WIDTH).toString().toInt();
             int atlasHeight = attribs.value(ATLAS_ATTR_HEIGHT).toString().toInt();
             drawAtlasBox(atlasWidth, atlasHeight);
             atlasName_ = attribs.value(ATLAS_ATTR_NAME).toString();
         }
         else if(xml.name().toString() == TEXTUREREGION_ELEMENT)
         {
             QXmlStreamAttributes attribs = xml.attributes();

             // Image properties
             QString resName = attribs.value(TEXTUREREGION_ATTR_RESNAME).toString();
             int sceneX = attribs.value(TEXTUREREGION_ATTR_SCENEX).toString().toInt();
             int sceneY = attribs.value(TEXTUREREGION_ATTR_SCENEY).toString().toInt();

             // TextureRegion properties
             QString itemName = attribs.value(TEXTUREREGION_ATTR_ITEMNAME).toString();
             QString tiledString = attribs.value(TEXTUREREGION_ATTR_TILED).toString();
             bool isTiled = false;
             if(tiledString == "true")
                 isTiled = true;
             int rows = attribs.value(TEXTUREREGION_ATTR_ROWS).toString().toInt();
             int columns = attribs.value(TEXTUREREGION_ATTR_COLUMNS).toString().toInt();

             qreal leftOffset = attribs.value(ITEMS_TO_LEFT).toString().toFloat();
             qreal topOffset = attribs.value(ITEMS_ABOVE).toString().toFloat();

             // Create Pixmap from
             QImage image;
             bool loaded = image.load(projectPath_ + "/assets/gfx/" + resName);
             if(loaded)
             {
                QPixmap pixmap = QPixmap::fromImage(image);
                QGraphicsItem* pixmapItem = atlasScene_->addPixmap(pixmap);
                pixmapItem->setFlags((QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable));
                pixmapItem->setPos(sceneX + 0.5*leftOffset, sceneY + 0.5*topOffset);

                QJsonObject userData;
                userData.insert(TEXREG_RESOURCENAME, resName);
                userData.insert(ITEMS_ABOVE, topOffset);
                userData.insert(ITEMS_TO_LEFT, leftOffset);
                userData.insert(TEXREG_NCOLUMNS, columns);
                userData.insert(TEXREG_NROWS, rows);
                userData.insert(TEXREG_VARNAME, itemName);
                userData.insert(TEXREG_IS_TILED, isTiled);
                pixmapItem->setData(Qt::UserRole, userData);
             }
         }
     }
     if (xml.hasError()) {
         // do error handling
     }

    inputFile.close();
    isProjectOpen_ = true;
    createProject();
}

// When selection has changed, call this so that the toolbar data updates
void MainWindow::updatePropertiesData(QGraphicsItem * item)
{
    QString itemName("");
    bool isTiled = false;
    int nRows = 0;
    int nColumns = 0;
    if(item) // Item is not null
    {
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        nRows = (int) userData.find(TEXREG_NROWS).value().toDouble();
        nColumns = (int) userData.find(TEXREG_NCOLUMNS).value().toDouble();
        isTiled = userData.find(TEXREG_IS_TILED).value().toBool();
        itemName = userData.find(TEXREG_VARNAME).value().toString();

        ui->varNameText->setText(itemName);
        ui->varNameText->setEnabled(true);
        ui->isTiledCheckBox->setChecked(isTiled);
        ui->isTiledCheckBox->setEnabled(true);
        QString rowString = "", colString = "";
        if(nRows > 0)
            rowString = QString::number(nRows);
        if(nColumns > 0)
            colString = QString::number(nColumns);
        ui->nRowsText->setText(rowString);
        ui->nColumnsText->setText(colString);
    }
}

// Slots

void MainWindow::receiveDirAndAtlasName(QString dirPath, QString atlasName)
{
    atlasName_ = atlasName;
    projectPath_ = dirPath;
    createProject();
}

// When toolbar data updates, call this so that item data updates
// TODO: Because of this code, the nColumns and nRows don't persist through clicks
void MainWindow::updateItemProperties()
{
    QString itemName = ui->varNameText->text();
    bool isTiled = ui->isTiledCheckBox->isChecked();
    int nRows = ui->nRowsText->text().toInt();
    int nColumns = ui->nColumnsText->text().toInt();
    QList<QGraphicsItem*> selectedItems = atlasScene_->selectedItems();
    foreach(QGraphicsItem* item, selectedItems)
    {
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        userData.insert(TEXREG_VARNAME, itemName);
        userData.insert(TEXREG_IS_TILED, isTiled);
        userData.insert(TEXREG_NCOLUMNS, nColumns);
        userData.insert(TEXREG_NROWS, nRows);
        item->setData(Qt::UserRole, userData);
    }
}

void MainWindow::drawAtlasBox(int width, int height)
{
    currentAtlasRect_ = QRectF(0, 0, width, height);
    atlasScene_->addLine(width + 1, 0, width + 1, height + 1);
    atlasScene_->addLine(0, height + 1, width + 1, height + 1);
}

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
        int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
        userData.insert(ITEMS_ABOVE, 0);
        userData.insert(ITEMS_TO_LEFT, leftIndex);
        item->setData(Qt::UserRole, userData);
        correctCollisions(item, SNAP_UP);
//        finalizeCollisions(item, SNAP_UP);
    }
    updateStatusBarText();
}

// deprecated
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
    updateStatusBarText();
}

void MainWindow::snapLeft()
{
    QList<QGraphicsItem*> selectedItems = atlasScene_->selectedItems();
    foreach(QGraphicsItem* item, selectedItems)
    {
        item->setX(0);
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
        userData.insert(ITEMS_ABOVE, topIndex);
        userData.insert(ITEMS_TO_LEFT, 0);
        item->setData(Qt::UserRole, userData);
        correctCollisions(item, SNAP_LEFT);
        //finalizeCollisions(item, SNAP_LEFT);
    }
    updateStatusBarText();
}

// deprecated
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
    updateStatusBarText();
}

void MainWindow::selectAnother()
{
    selectedItems_ = atlasScene_->selectedItems();
    if(selectedGraphicsItem_ == NULL)
    {
        selectedGraphicsItem_ = selectedItems_.at(0);
        updatePropertiesData(selectedGraphicsItem_);
        enableSnapButtons(true);
    }
    else // There is exactly one selected item
    {
        // This is good enough.
        atlasScene_->clearSelection();
        emptyPropertiesData();
        enableSnapButtons(false);
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
//            QGraphicsPixmapItem pxItem(pixMap);
//            pxItem.setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
//            QJsonObject userData;
//            userData.insert(TEXREG_RESOURCENAME, imageFileNames_.at(item->data(Qt::UserRole).toInt()));
//            pxItem.setData(Qt::UserRole, userData);
//            atlasScene_->addItem(&pxItem);
            QGraphicsPixmapItem* returnedItem = atlasScene_->addPixmap(pixMap);
            returnedItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            QJsonObject userData;
            userData.insert(TEXREG_RESOURCENAME, imageFileNames_.at(item->data(Qt::UserRole).toInt()));
            returnedItem->setData(Qt::UserRole, userData);

        }
    }
}

void MainWindow::updatePreview(int itemIndex)
{
    setPreviewScene(previewList_.at(itemIndex));
}

void MainWindow::updatePreview(QListWidgetItem *item){
    if(item != NULL)
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
        break;
    case 16777223: // Del key
        deleteItem();
        break;
    case 87: // W
    case 44:
        snapUp();
        break;
    case 65: // A
        snapLeft();
        break;
    default:
        qDebug() << event->key();
    }
}

void MainWindow::deleteItem()
{
    QList<QGraphicsItem*> selectedList = atlasScene_->selectedItems();
    foreach(QGraphicsItem* selectedItem, selectedList)
    {
        atlasScene_->removeItem(selectedItem);
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
    bool shouldMove = false;

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
                shouldMove = true;
                min_max = lowest;
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
                shouldMove = true;
                min_max = left;
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

    // Only move after deciding which is the largest
    if(shouldMove)
        switch(snapDirection)
        {
        case SNAP_UP:
        {
            if(min_max > currentAtlasRect_.bottom())
                return false;
            QJsonObject userData = selectedItem->data(Qt::UserRole).toJsonObject();
            int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
            int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
            userData.insert(ITEMS_ABOVE, ++topIndex);
            userData.insert(ITEMS_TO_LEFT, leftIndex);
            selectedItem->setData(Qt::UserRole, userData);

            selectedItem->setY(min_max);
            break;
        }
        case SNAP_LEFT:
        {
            if(min_max > currentAtlasRect_.right())
                return false;
            QJsonObject userData = selectedItem->data(Qt::UserRole).toJsonObject();
            int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
            int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();
            userData.insert(ITEMS_ABOVE, topIndex);
            userData.insert(ITEMS_TO_LEFT, ++leftIndex);
            selectedItem->setData(Qt::UserRole, userData);

            selectedItem->setX(min_max);
        }
        }

    collidingItems = atlasScene_->collidingItems(selectedItem);
    if(collidingItems.length() > 0)
    {
        correctCollisions(selectedItem, snapDirection);
    }

    return false;
}

void MainWindow::enableSnapButtons(bool enabled = true)
{
    ui->leftButton->setEnabled(enabled);
    ui->upButton->setEnabled(enabled);
}

void MainWindow::emptyPropertiesData()
{
    ui->varNameText->setText("");
    ui->varNameText->setEnabled(false);
    ui->isTiledCheckBox->setChecked(false);
    ui->isTiledCheckBox->setEnabled(false);
    ui->nRowsText->setText("");
    ui->nColumnsText->setText("");
}

QString MainWindow::createAndEngineCode()
{
    QString code("");
    QString itemName;
    bool isTiled;
    ////////////////////////////////////////////////////////////////////////
    code += "// Field Declarations \nprivate BitmapTextureAtlas " + atlasName_ + ";\n";
    // Loop here
    QList<QGraphicsItem*> allItems = atlasScene_->items(currentAtlasRect_);
    foreach(QGraphicsItem* item, allItems)
    {
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        isTiled = userData.find(TEXREG_IS_TILED).value().toBool();
        itemName = userData.find(TEXREG_VARNAME).value().toString();
        if(itemName.isEmpty())
            itemName = "NAMELESS";

        code += "private ";
        if(isTiled)
            code += "Tiled";
        code += "TextureRegion " + itemName + ";\n";
    }
    ////////////////////////////////////////////////////////////////////////


    code += "\n\n";
    code += "// Put these in onCreateResources()\n";
    code += "BitmapTextureAtlasTextureRegionFactory.setAssetBasePath(\"gfx/\")";
    code += "\n\n";

    code += atlasName_ + " = new BitmapTextureAtlas(getTextureManager(), " + \
        QString::number(currentAtlasRect_.width()) + ", " + \
        QString::number(currentAtlasRect_.height()) + ", TextureOptions.BILINEAR);\n";

    foreach(QGraphicsItem* item, allItems)
    {
        QJsonObject userData = item->data(Qt::UserRole).toJsonObject();
        int nRows = (int) userData.find(TEXREG_NROWS).value().toDouble();
        int nColumns = (int) userData.find(TEXREG_NCOLUMNS).value().toDouble();
        isTiled = userData.find(TEXREG_IS_TILED).value().toBool();
        QString resName = QFileInfo(userData.find(TEXREG_RESOURCENAME).value().toString()).fileName();
        itemName = userData.find(TEXREG_VARNAME).value().toString();
        if(itemName.isEmpty())
            itemName = "NAMELESS";

        // get x and y values
        QPointF point = item->scenePos();
        int topIndex = (int) userData.find(ITEMS_ABOVE).value().toDouble();
        int leftIndex = (int) userData.find(ITEMS_TO_LEFT).value().toDouble();

        int xPos = (point.x() - leftIndex*0.5);
        int yPos = (point.y() - topIndex*0.5);

        int width = item->boundingRect().width() - 1; // Default bounding rect stroke is 1
        int height = item->boundingRect().height() - 1; // Default bounding rect stroke is 1


        code += itemName + " = BitmapTextureAtlasTextureRegionFactory.create";
        if(isTiled)
        {
            code += "Tiled";
        }
        code += "FromAsset(" + atlasName_ + ", this, \"" \
                + resName + "\", " \
                + QString::number(xPos) + ", " + QString::number(yPos);
        if(isTiled)
        {
            code += ", " + QString::number(nColumns) + ", " + QString::number(nRows);
        }
        code += "); //" + QString::number(width) + "x" + QString::number(height) + "\n";

        //qDebug() << debug;
    }

    code += atlasName_ + ".load();";

    andEngineCode_ = code;
    showTestOutput();

    return code;
}



