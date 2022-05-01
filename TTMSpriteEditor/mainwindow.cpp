/** Created by The Task Managers for CS3505 Fall 2019 at the University of Utah.
 *  Molly Clare
 *  Taylor Jacobsen
 *  Ryan Brooks
 *  André Watson
 *  Calvin Nielson
 *  Alex Bartee
 */
#include "mainwindow.h"
#include "frame.h"
#include "ui_mainwindow.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QTimer>
#include <QMessageBox>

// a detailed definition for each data member and function can be found in "MainWindow.h"
MainWindow::MainWindow(ToolBox* toolbox) : ui(new Ui::MainWindow) {
    editedWithoutSave = false;
    canEdit = false;
    isDark = false;
    toolButtons = new std::vector<QToolButton*>();
    frameButtons = new std::vector<QPushButton*>();
    ui->setupUi(this);
    QMainWindow::setWindowState(Qt::WindowMaximized);

    //set up color picker and palette
    colorDialog = new QColorDialog(this);
    ui->colorLayout->addWidget(colorDialog, 0, nullptr);
    colorDialog->setWindowFlags(Qt::SubWindow);
    colorDialog->setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::NoButtons | QColorDialog::ShowAlphaChannel);
    //setup default custom colors - bonus feature
    //blk
    colorDialog->setCustomColor(0, QColor(0, 0, 0));
    colorDialog->setCustomColor(1, QColor(85, 85, 85));
    //blue
    colorDialog->setCustomColor(2, QColor(0, 0, 170));
    colorDialog->setCustomColor(3, QColor(85, 85, 255));
    //grn
    colorDialog->setCustomColor(4, QColor(0, 170, 0));
    colorDialog->setCustomColor(5, QColor(85, 255, 85));
    //cyan
    colorDialog->setCustomColor(6, QColor(0, 170, 170));
    colorDialog->setCustomColor(7, QColor(85, 255, 255));
    //red
    colorDialog->setCustomColor(8, QColor(170, 0, 0));
    colorDialog->setCustomColor(9, QColor(255, 85, 85));
    //magenta
    colorDialog->setCustomColor(10, QColor(170, 0, 170));
    colorDialog->setCustomColor(11, QColor(255, 85, 255));
    //yellow
    colorDialog->setCustomColor(12, QColor(170, 85, 0));
    colorDialog->setCustomColor(13, QColor(255, 255, 85));
    //white
    colorDialog->setCustomColor(14, QColor(170, 170, 170));
    colorDialog->setCustomColor(15, QColor(255, 255, 255));

    //create the scroll area for the frame list
    frameList = new QScrollArea;
    frameList->setWidgetResizable(true);
    frameList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollable = new QWidget();
    scrollable->setLayout(new QHBoxLayout());
    frameList->setWidget(scrollable);
    ui->scrollLayout->addWidget(frameList);

    // CONNECTIONS SETUP
    // toolbox connections
    QObject::connect(toolbox,&ToolBox::updateFrames, this, &MainWindow::updateFrames);
    QObject::connect(toolbox,&ToolBox::updateWorkspace, this, &MainWindow::updateWorkspace);
    // resolution setting
    QObject::connect(this, &MainWindow::setFrameResolution, toolbox, &ToolBox::setFrameResolutionStandard);
    QObject::connect(this, &MainWindow::setDisplayResolution, toolbox, &ToolBox::setDisplayResolutionStandard);
    //action buttons
    QObject::connect(ui->increaseBrushButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->decreaseBrushButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->leftRotateButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->rightRotateButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->horizontalFlipButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->verticalFlipButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->xMirrorButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->yMirrorButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    QObject::connect(ui->undoButton, &QToolButton::clicked, this, &MainWindow::actionToolButtonPressed);
    //action to ToolBox return
    QObject::connect(this, &MainWindow::rotate, toolbox, &ToolBox::rotate);
    QObject::connect(this, &MainWindow::flip, toolbox, &ToolBox::flip);
    QObject::connect(this, &MainWindow::undoPressed, toolbox, &ToolBox::undo);
    QObject::connect(this, &MainWindow::toggleMirroringX, toolbox, &ToolBox::toggleMirroringX);
    QObject::connect(this, &MainWindow::toggleMirroringY, toolbox, &ToolBox::toggleMirroringY);
    //color and size changing
    QObject::connect(this, &MainWindow::penSizeChanged, toolbox, &ToolBox::changeBrushSize);
    QObject::connect(colorDialog,&QColorDialog::currentColorChanged,this, &MainWindow::changePenColor);
    QObject::connect(this, &MainWindow::penColorChanged,toolbox,&ToolBox::changePenColor);
    QObject::connect(this, &MainWindow::setGlobalFill, toolbox, &ToolBox::setGlobalFill);
    // menu bar actions
    QObject::connect(ui->newAction, &QAction::triggered, this, &MainWindow::displayNewSpriteDialog);
    QObject::connect(ui->openAction, &QAction::triggered, this, &MainWindow::openFile);
    QObject::connect(ui->saveAction, &QAction::triggered, this, &MainWindow::saveSprite);
    QObject::connect(ui->exportAction, &QAction::triggered, this, &MainWindow::exportSprite);
    QObject::connect(ui->undoAction, &QAction::triggered, toolbox, &ToolBox::undo);
    QObject::connect(ui->shortcutsAction, &QAction::triggered, this, &MainWindow::showShortcuts);
    QObject::connect(ui->toggleDarkModeAction, &QAction::triggered, this, &MainWindow::toggleDarkMode);
    QObject::connect(this, &MainWindow::writeToJSON, toolbox, &ToolBox::writeToJSON);
    QObject::connect(this, &MainWindow::readFromJSON, toolbox, &ToolBox::readFromJSON);
    QObject::connect(toolbox, &ToolBox::invalidFile, this, &MainWindow::displayError);
    QObject::connect(this, &MainWindow::writeToGIF, toolbox, &ToolBox::writeToGIF);
    QObject::connect(toolbox, &ToolBox::newSpriteSignal, this, &MainWindow::makeNewSprite);
    // mouse connections
    QObject::connect(this, &MainWindow::mouseUp, toolbox, &ToolBox::mouseUp);
    QObject::connect(this, &MainWindow::mouseDown, toolbox, &ToolBox::mouseDown);
    QObject::connect(this, &MainWindow::mouseMove, toolbox, &ToolBox::mouseMove);
    // keyboard shortcut
    QObject::connect(this, &MainWindow::keyPressed, toolbox, &ToolBox::keyPressed);
    // frame button connections
    QObject::connect(this, &MainWindow::addFrame, toolbox, &ToolBox::addFrame);
    QObject::connect(this, &MainWindow::clearFrames, toolbox, &ToolBox::clearFrames);
    QObject::connect(ui->newFrameButton, &QPushButton::clicked, toolbox, &ToolBox::addFrame);
    QObject::connect(ui->copyButton, &QPushButton::clicked, toolbox, &ToolBox::copyFrame);
    QObject::connect(ui->deleteButton, &QPushButton::clicked, toolbox, &ToolBox::deleteFrame);
    QObject::connect(ui->swapLeftButton, &QPushButton::clicked, toolbox, &ToolBox::swapFrameLeft);
    QObject::connect(ui->swapRightButton, &QPushButton::clicked, toolbox, &ToolBox::swapFrameRight);
    QObject::connect(this, &MainWindow::selectModelFrame, toolbox, &ToolBox::changeFrame);
    // toolbox button connections
    QObject::connect(ui->pencilButton, &QToolButton::clicked, this, &MainWindow::toolButtonPressed);
    QObject::connect(ui->eraserButton, &QToolButton::clicked, this, &MainWindow::toolButtonPressed);
    QObject::connect(ui->fillButton, &QToolButton::clicked, this, &MainWindow::toolButtonPressed);
    QObject::connect(ui->lineButton, &QToolButton::clicked, this, &MainWindow::toolButtonPressed);
    QObject::connect(this, &MainWindow::toolChanged, toolbox, &ToolBox::toolChanged);
    // Live Preview connections
    QObject::connect(this, &MainWindow::getPreviewFrame, toolbox, &ToolBox::getPreviewFrame);
    QObject::connect(toolbox, &ToolBox::updatePreviewFrame, this, &MainWindow::animatePreview);
    connect(ui->fpsSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::setFPS);
    ui->fpsSpinBox->setRange(1,26);

    setUpButtonVectors();
    enableAll(false);
    colorDialog->setEnabled(false);

    // Live Preview Setup
    // Assign the starting values
    fpsCount = 1;
    frameIndex = 0;
    // Call the first animate
    animate();
}

MainWindow::~MainWindow() {
    delete ui;
    delete colorDialog;
    delete toolButtons;
    delete frameList;
    delete frameButtons;
}

void MainWindow::toggleDarkMode() {
    if (!isDark) {
        this->setStyleSheet("background-color: rgb(50, 50, 50)");
        for (auto button : *toolButtons) {
            QPalette pal = button->palette();
            if (pal.color(QPalette::Button) != QColor(toolR, toolG, toolB))
                button->setStyleSheet("background-color: rgb(100,100,100)");
        }
        for (auto button : *frameButtons) {
            button->setStyleSheet("background-color: rgb(100,100,100)");
        }
        ui->menubar->setStyleSheet("background-color: rgb(100,100,100)");
        ui->frameListLabel->setStyleSheet("color: rgb(166, 255, 178)");
        ui->previewLabel->setStyleSheet("color: rgb(166, 255, 178)");
        ui->paletteLabel->setStyleSheet("color: rgb(166, 255, 178)");
        ui->fpsSpinBox->setStyleSheet("color: rgb(166, 255, 178)");
        colorDialog->setStyleSheet("color: rgb(166,255,178); background-color: rgb(100,100,100)");
        isDark = true;
    }
    else { // sending ("") to setStyleSheet basically just resets it to the default!
        this->setStyleSheet("");
        for (auto button : *toolButtons) {
            button->setStyleSheet("");
        }
        for (auto button : *frameButtons) {
            button->setStyleSheet("");
        }
        ui->menubar->setStyleSheet("");
        ui->frameListLabel->setStyleSheet("");
        ui->previewLabel->setStyleSheet("");
        ui->paletteLabel->setStyleSheet("");
        ui->fpsSpinBox->setStyleSheet("");
        colorDialog->setStyleSheet("");
        isDark = false;
    }
}

void MainWindow::showShortcuts() {
    QDialog shortcutsD(this);
    QFormLayout form(&shortcutsD);
    form.addRow(new QLabel("Ctrl+H: Open this menu.\nCtrl+N: Create a new sprite project."
                           "\nCtrl+O: Open a .ssp file."
                           "\nCtrl+S: Save the current project as a .ssp file."
                           "\nCtrl+E: Export current project to a .gif file."
                           "\nCtrl+Z: Undo last action taken on the active frame."
                           "\nCtrl+D: Toggle dark mode."
                           "\nP: Pencil tool."
                           "\nE: Eraser tool."
                           "\nH: Flip horizontal."
                           "\nV: Flip vertical."
                           "\nY: Toggle vertical mirroring."
                           "\nX: Toggle horizontal mirroring."
                           "\nF: Fill tool."
                           "\nL: Line tool."
                           "\n+: Increase brush size."
                           "\n-: Decrease brush size."
                           "\n[: Rotate left."
                           "\n]: Rotate right."));
    if (isDark) {
        shortcutsD.setStyleSheet("color: rgb(166,255,178); background-color: rgb(100,100,100)");
    }
    shortcutsD.exec();
}

void MainWindow::setUpButtonVectors() {
    // tools
    toolButtons->push_back(ui->increaseBrushButton);
    toolButtons->push_back(ui->decreaseBrushButton);
    toolButtons->push_back(ui->eraserButton);
    toolButtons->push_back(ui->fillButton);
    toolButtons->push_back(ui->horizontalFlipButton);
    toolButtons->push_back(ui->verticalFlipButton);
    toolButtons->push_back(ui->increaseBrushButton);
    toolButtons->push_back(ui->rightRotateButton);
    toolButtons->push_back(ui->leftRotateButton);
    toolButtons->push_back(ui->lineButton);
    toolButtons->push_back(ui->pencilButton);
    toolButtons->push_back(ui->undoButton);
    toolButtons->push_back(ui->xMirrorButton);
    toolButtons->push_back(ui->yMirrorButton);
    // frame buttons
    frameButtons->push_back(ui->newFrameButton);
    frameButtons->push_back(ui->copyButton);
    frameButtons->push_back(ui->deleteButton);
    frameButtons->push_back(ui->swapLeftButton);
    frameButtons->push_back(ui->swapRightButton);
}

void MainWindow::enableAll(bool shouldEnable) {
    ui->undoAction->setEnabled(shouldEnable);
    canEdit = shouldEnable;
    ui->fpsSpinBox->setEnabled(shouldEnable);
    for (auto button : *frameButtons) {
        button->setEnabled(shouldEnable);
    }
    for (auto button : *toolButtons) {
        button->setEnabled(shouldEnable);
    }
}

void MainWindow::resetPenButtons() {
    if (isDark) {
        ui->pencilButton->setStyleSheet("background-color: rgb(100,100,100)");
        ui->eraserButton->setStyleSheet("background-color: rgb(100,100,100)");
        ui->fillButton->setStyleSheet("background-color: rgb(100,100,100)");
        ui->lineButton->setStyleSheet("background-color: rgb(100,100,100)");
    }
    else {
        ui->pencilButton->setStyleSheet("");
        ui->eraserButton->setStyleSheet("");
        ui->fillButton->setStyleSheet("");
        ui->lineButton->setStyleSheet("");
    }
}

void MainWindow::toolButtonPressed() {
    colorDialog->setEnabled(true);
    QObject*senderObj = sender();
    resetPenButtons();
    toggleButtonColor(dynamic_cast<QToolButton*>(senderObj));
    emit toolChanged(senderObj);
}

void MainWindow::actionToolButtonPressed(){
    QString senderNameQ = sender()->objectName();
     std::string senderName = senderNameQ.toStdString();
     // redirectory for action buttons
     if(senderName == "leftRotateButton"){
         emit rotate(true);
     }
     else if(senderName == "rightRotateButton"){
         emit rotate(false);
     }
     else if(senderName == "horizontalFlipButton"){
         emit flip(true);
     }
     else if(senderName == "verticalFlipButton"){
         emit flip(false);
     }
     else if(senderName == "increaseBrushButton"){
         emit penSizeChanged(true);
     }
     else if(senderName == "decreaseBrushButton"){
         emit penSizeChanged(false);
     }
     else if(senderName == "undoButton"){
         emit undoPressed();
     }
     else if(senderName == "xMirrorButton"){
         toggleButtonColor(ui->xMirrorButton);
         emit toggleMirroringX();
     }
     else if(senderName == "yMirrorButton"){
         toggleButtonColor(ui->yMirrorButton);
         emit toggleMirroringY();
     }
}

void MainWindow::toggleButtonColor(QToolButton*button) {
      QPalette pal = button->palette();
      // if the button has the green in-use highlight:
      if (pal.color(QPalette::Button) != QColor(toolR, toolG, toolB)) {
          button->setStyleSheet("background-color: rgb(166, 255, 178)");
      }
      else {
          // if in dark mode, set the background to the dark default. otherwise,
          // just reset the stylesheet.
          if (isDark) {
              button->setStyleSheet("background-color: rgb(100, 100, 100)");
          } else {
              button->setStyleSheet("");
          }
      }
}

void MainWindow::exportSprite() {
    int gifScale = 1;



    QDialog exportDialog(this);
    QFormLayout form(&exportDialog);
    exportDialog.setWindowTitle("EXPORT .GIF");
    form.addRow(new QLabel("Please note that any transparent pixels will be shown as "
                           "\n0,0,0 (black) with no alpha channel in the final product.\n"));
    form.addRow(new QLabel("Enter an integer for the scale factor of your .gif:"));
    QList<QSpinBox*> fields;

    QSpinBox*getScale = new QSpinBox(&exportDialog);
    getScale->setRange(1, 100);
    getScale->setValue(1);
    form.addRow(getScale);
    fields << getScale;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &exportDialog);
    if (isDark) {
        exportDialog.setStyleSheet("color: rgb(166,255,178); background-color: rgb(100,100,100)");
    }
    form.addRow(&buttonBox);
    exportDialog.resize(300, 100);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &exportDialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &exportDialog, SLOT(reject()));

    if (exportDialog.exec() == QDialog::Accepted) {
        gifScale = fields.first()->value();
        QString filename = QFileDialog::getSaveFileName(this, "Save As", "", "Animated GIF file (*.gif)");
        if(!filename.isEmpty()) {
            emit writeToGIF(filename, gifScale, fpsCount);
        }
        else { return; }
    }
}


void MainWindow::saveSprite() {
    QString filename = QFileDialog::getSaveFileName(this, "Save As", "", "Spritesheet Project (*.ssp)");
    if(!filename.isEmpty()) {
        emit writeToJSON(filename);
        editedWithoutSave = false;
    }
}

void MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "Spritesheet Project (*.ssp)");
    if(!filename.isEmpty()) {
        QFile file(filename);
        emit readFromJSON(filename);
        editedWithoutSave = false;
    }
}

void MainWindow::closeEvent( QCloseEvent* event ) {
    if (editedWithoutSave) {
        if (QMessageBox::Yes == QMessageBox(QMessageBox::Information, "Warning!",
                                            "Are you sure you'd like to quit without saving?"
                                            "\n\nYou can navigate from the File menu above or"
                                            "\nsimply press ctrl + s on your keyboard to save!",
                                            QMessageBox::Yes|QMessageBox::No).exec())
        {
            event->accept();
        } else { // cancel closing
            event->ignore();
        }
    }
}

void MainWindow::displayError(QString errorMessage) {
    QDialog error(this);
    QFormLayout form(&error);
    form.addRow(new QLabel(errorMessage));
    error.exec();
}

void MainWindow::keyPressEvent(QKeyEvent*event) {
    // don't do anything if the canvas workspace is not editable
    if (!canEdit) {
        return;
    }
    // redirectory for keys pressed
    switch (event->key()) {
    // 'P' (pencil)
    case Qt::Key::Key_P: {
        resetPenButtons();
        toggleButtonColor(ui->pencilButton);
        emit toolChanged(ui->pencilButton);
        break;
    }
    // 'E' (eraser)
    case Qt::Key::Key_E: {
        resetPenButtons();
        toggleButtonColor(ui->eraserButton);
        emit toolChanged(ui->eraserButton);
        break;
    }
    // '+' (increase pen size) ('=')
    case Qt::Key::Key_Equal: {
        emit penSizeChanged(true);
        break;
    }
    // '-' (decrease pen size)
    case Qt::Key::Key_Minus: {
        emit penSizeChanged(false);
        break;
    }
    // 'F' (fill)
    case Qt::Key::Key_F: {
        resetPenButtons();
        toggleButtonColor(ui->fillButton);
        emit toolChanged(ui->fillButton);
        break;
    }
    // shift
    case Qt::Key::Key_Shift: {
        //TODO model->isGlobalFill = true;
        emit setGlobalFill(true);
        break;
    }
    // 'L' (line)
    case Qt::Key::Key_L: {
        resetPenButtons();
        toggleButtonColor(ui->lineButton);
        emit toolChanged(ui->lineButton);
        break;
    }
    // 'X' (mirror horizontal)
    case Qt::Key::Key_X: {
        toggleButtonColor(ui->xMirrorButton);
        emit toggleMirroringX();
        break;
    }
    // 'Y' (mirror vertical)
    case Qt::Key::Key_Y: {
        toggleButtonColor(ui->yMirrorButton);
        emit toggleMirroringY();
        break;
    }
    // '[' (rotate left)
    case Qt::Key::Key_BracketLeft: {
        emit rotate(true);
        break;
    }
    // ']' (rotate right)
    case Qt::Key::Key_BracketRight: {
        emit rotate(false);
        break;
    }
    // 'H' (flip horizontal)
    case Qt::Key::Key_H: {
        emit flip(true);
        break;
    }
    // 'V' (flip vertical)
    case Qt::Key::Key_V: {
        emit flip(false);
        break;
    }
    } // end switch
    emit keyPressed(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent*event) {
    if (event->key() == Qt::Key::Key_Shift) {
        emit setGlobalFill(false);
    }
}

void MainWindow::mousePressEvent(QMouseEvent*event) {
    QMainWindow::setWindowState(Qt::WindowMaximized);
    emit mouseDown(event, ui->workspaceLabel);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    emit mouseMove(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent*event) {
    emit mouseUp(event);
}


void MainWindow::drawWorkspace(Frame frame){
    ui->workspaceLabel->setPixmap((QPixmap::fromImage(frame.currentWorkspace).scaled(640,640,Qt::KeepAspectRatio,Qt::FastTransformation)));
    //Scaling may end up non-square, so adjust label size accordingly
    ui->workspaceLabel->setMaximumSize(ui->workspaceLabel->pixmap()->size());
    ui->workspaceLabel->setStyleSheet("border: 1px solid black");
}

void MainWindow::makeNewSprite(int x, int y) {
    x=std::min(128,x);
    y=std::min(128,y);
    emit clearFrames();
    Frame firstFrame(x, y);
    double dispX, dispY;
    if(y > x){
        dispY = 640;
        dispX = x * (dispY / y);
    }
    else{
        dispX = 640;
        dispY = y * (dispX / x);
    }
    emit setDisplayResolution(static_cast<int>(dispX), static_cast<int>(dispY));
    emit setFrameResolution(x, y);
    emit addFrame();
    drawWorkspace(firstFrame);
    enableAll(true);
    if (x != y) {
        ui->leftRotateButton->setEnabled(false);
        ui->rightRotateButton->setEnabled(false);
    }
    ui->swapRightButton->setEnabled(false);
    ui->swapLeftButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);

}

void MainWindow::displayNewSpriteDialog() {
    /* This method has been adapted from some helpful guiding code that can be found at:
    https://stackoverflow.com/questions/17512542/getting-multiple-inputs-from-qinputdialog-in-qtcreator
    written by StackOverflow user @alexisdm. */

    // BEGIN sampled code:
    QDialog newSpriteDlog(this);
    QFormLayout form(&newSpriteDlog);
    newSpriteDlog.setWindowTitle("NEW SPRITE");
    form.addRow(new QLabel("Fill out the information below, then click \"Ok\" to create a new sprite!"));
    QList<QSpinBox*> fields;

    // places for the user to type information
    QSpinBox*getUserX = new QSpinBox(&newSpriteDlog);
    getUserX->setRange(1, 128);
    getUserX->setValue(32);
    QString xLabel = QString("%1").arg("pixels wide:");
    form.addRow(xLabel, getUserX);
    fields << getUserX;

    QSpinBox*getUserY = new QSpinBox(&newSpriteDlog);
    getUserY->setRange(1, 128);
    getUserY->setValue(32);
    QString yLabel = QString("%1").arg("pixels tall:");
    form.addRow(yLabel, getUserY);
    fields << getUserY;

    form.addRow(new QLabel("WARNING: you will not be able to rotate a non-square sprite."));

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &newSpriteDlog);
    if (isDark) {
        newSpriteDlog.setStyleSheet("background-color: rgb(100,100,100)");
    }
    form.addRow(&buttonBox);
    newSpriteDlog.resize(400, 100);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &newSpriteDlog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &newSpriteDlog, SLOT(reject()));

    // only need to handle accept, pressing "cancel" just closes the dialog
    if (newSpriteDlog.exec() == QDialog::Accepted) {
        int xDimen = fields.first()->value();
        int yDimen = fields.last()->value();
        makeNewSprite(xDimen, yDimen);
    } // END of code sampled from the URL above

    delete getUserX; delete getUserY;
}

void MainWindow::changePenColor(const QColor& color) {
    emit penColorChanged(color);
}

void MainWindow::updateWorkspace(Frame* currentFrame) {
    editedWithoutSave = true;
    drawWorkspace(*currentFrame);
}

void MainWindow::updateFrames(vector<Frame>* frames, int index) {
    //state-dependent button disabling
    if (frames->size() <= 1) {
        ui->deleteButton->setEnabled(false);
    }
    else {
         ui->deleteButton->setEnabled(true);
    }
    if (index == 0) {
        ui->swapLeftButton->setEnabled(false);
    }
    else {
        ui->swapLeftButton->setEnabled(true);
    }
    if (index >= (frames->size() - 1)) {
        ui->swapRightButton->setEnabled(false);
    }
    else {
        ui->swapRightButton->setEnabled(true);
    }

    //close and remake scrollable widget so everything redraws
    scrollable->close();
    scrollable = new QFrame(frameList);
    scrollable->setLayout(new QHBoxLayout());
    frameList->setWidget(scrollable);

    for (unsigned int i = 0; i < frames->size(); i++){
        QPushButton* displayButton = new QPushButton;
        displayButton->setFlat(true);
        displayButton->setObjectName("frame" + QString::number(i));
        displayButton->setParent(scrollable);
        displayButton->setAttribute(Qt::WA_DeleteOnClose);
        connect(displayButton, &QPushButton::pressed, this, &MainWindow::selectFrame);

        QPixmap frameMap = QPixmap::fromImage(frames->at(i).getUpdatedState().scaled(320,320, Qt::KeepAspectRatio, Qt::FastTransformation));
        displayButton->setIconSize(frameMap.size());
        displayButton->setIcon(frameMap);
        displayButton->setMaximumSize(frameMap.size());
        if (index == (int)i) {
            displayButton->setStyleSheet("border: 1px solid red");
        }
        else {
            displayButton->setStyleSheet("border: 1px solid black");
        }
        scrollable->layout()->addWidget(displayButton);

    }
}

void MainWindow::selectFrame() {
    QString buttonName = sender()->objectName();
    int newIndex = buttonName.remove(0, 5).toInt(); //chop off "frame" from start of name
    emit selectModelFrame(newIndex);

}

//------------------------------------------
//     Live Preview
//------------------------------------------

void MainWindow::setFPS(int newFPS){
    if (newFPS > 60){
        newFPS = 60;
    }
    else if (newFPS < 0){
        newFPS = 0;
    }
    fpsCount = newFPS;
}

int MainWindow::getFPS(){
    return fpsCount;
}

void MainWindow::animate(){
    QTimer timer;

    if (fpsCount == 0){
        // validate the current frame
        emit getPreviewFrame(frameIndex);

        // check again in 1 second
        timer.singleShot(1000,this,SLOT(nextAnimate()));
    }
    else {
        // Increment to the next frame and update it
        frameIndex++;
        emit getPreviewFrame(frameIndex);

        timer.singleShot(1000 / fpsCount,this,SLOT(animate()));
    }
}

void MainWindow::animatePreview(Frame frame, long updatedIndex){
    frameIndex = updatedIndex;
    ui->animationPreviewLabel->setPixmap((QPixmap::fromImage(frame.currentWorkspace).scaled(320,320,Qt::KeepAspectRatio,Qt::FastTransformation)));
    //Scaling may end up non-square, so adjust label size accordingly
    ui->animationPreviewLabel->setMaximumSize(ui->animationPreviewLabel->pixmap()->size());
    ui->animationPreviewLabel->setStyleSheet("border: 1px solid black");
}
