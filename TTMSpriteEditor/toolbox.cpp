#include "toolbox.h"
#include <algorithm>
#include "mainwindow.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <gif.h>

ToolBox::ToolBox() : currentFrameIndex(0), isGlobalFill(false), brushSize(1), isMirroredX(false), mouseIsActive(false), isMirroredY(false), lastButtonClicked(nullptr) {
    frames = new vector<Frame>;
    currentColor = QColor(0, 0, 0, 255); // black, max alpha
    //constant
    displayWidth=640;
    displayHeight=640;
}

ToolBox::~ToolBox() {
    delete workspacePtr;
    delete frames;
}

void ToolBox::toolChanged(QObject* sender) {
    lastButtonClicked = sender;
}

void ToolBox::changePenColor(QColor color){
    currentColor = color;
}

void ToolBox::updateXY(double x, double y){
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    convertXY(x, y);
    mouseX = x;
    mouseY = y;
}

void ToolBox::convertXY(double& x, double& y){
    x -= workspacePtr->x(); // x
    y -= workspacePtr->y() + 25; // y offset
    //conversion
    x = x / (displayWidth / canvasWidth);
    y = (y / (displayHeight / canvasHeight));
}

// basically a redirectory for the tools
void ToolBox::mouseDown(QMouseEvent*event, QLabel*labelPtr) {
    workspacePtr = labelPtr;
    displayWidth = workspacePtr->width();
    displayHeight = workspacePtr->height();
    updateXY(event->x(), event->y());
    //manually set lastX,Y to pevent carryover from last mouse acttion
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseIsActive = true;

    //store and convert click location to the grid system
    double tempX = event->x();
    double tempY = event->y();
    convertXY(tempX, tempY);
    mouseClickedX = tempX;
    mouseClickedY = tempY;

    if (lastButtonClicked) {
        std::string senderName = lastButtonClicked->objectName().toStdString();
        //check which tool is selected
        if (senderName == "pencilButton") {
            drawBrush(mouseX, mouseY);

        }
        else if (senderName == "eraserButton") {
            drawEraser(mouseX, mouseY);
        }
    }
}

void ToolBox::mouseMove(QMouseEvent*event){
    if(mouseIsActive){
        updateXY(event->x(), event->y());
        //check which tool is selected
        if (lastButtonClicked) {
            std::string senderName = lastButtonClicked->objectName().toStdString();
            if(senderName == "pencilButton") {
               drawBrush(mouseX, mouseY);
               if(std::abs(mouseX - lastMouseX) > 1 || std::abs(mouseY - lastMouseY)> 1){
                   drawLine(lastMouseX, lastMouseY, mouseX, mouseY);
               }
            }
            else if(senderName == "eraserButton") {
               drawEraser(mouseX, mouseY);
               if(std::abs(mouseX - lastMouseX) > 1 || std::abs(mouseY - lastMouseY) > 1){
                   drawLine(lastMouseX, lastMouseY, mouseX, mouseY, transparent);
               }
            }
            //this should be all the tools for this method
        }
    }
}

void ToolBox::mouseUp(QMouseEvent*event) {
    if(mouseIsActive){
        updateXY(event->x(), event->y());
        if (lastButtonClicked) {
            std::string senderName = lastButtonClicked->objectName().toStdString();
            bool modified = false;
            if(senderName == "pencilButton") {
               drawBrush(mouseX, mouseY);
               if(std::abs(mouseX - lastMouseX) > 1 || std::abs(mouseY - lastMouseY) > 1){
                   drawLine(lastMouseX, lastMouseY, mouseX, mouseY);
               }
               modified = true;
            }
            else if(senderName == "eraserButton") {
               drawEraser(mouseX, mouseY);
               if(std::abs(mouseX - lastMouseX) > 1 || std::abs(mouseY - lastMouseY) > 1){
                   drawLine(lastMouseX, lastMouseY, mouseX, mouseY, transparent);
               }
               modified = true;
            }
            else if(senderName == "lineButton"){
                drawLine(mouseClickedX, mouseClickedY, mouseX, mouseY);
                modified = true;
            }
            else if(senderName == "fillButton"){
                if(isGlobalFill){
                    globalFill(mouseX, mouseY);
                }
                else{
                    fill(mouseX, mouseY);
                }
                modified = true;
            }
            //this should be all the tools for this method
            if(modified){
                currentFrame->saveModifications();
                emit updateFrames(frames, currentFrameIndex);
            }
        }
    }
    mouseIsActive = false;
}

void ToolBox::keyPressed(QKeyEvent*event) {
    //if hotkey is attempted stop drawing
    if(mouseIsActive){
        mouseIsActive = false;
        if(currentFrame){
            currentFrame->revertModifications();
            emit updateWorkspace(currentFrame);
        }
    }
}

void ToolBox::setFrameResolutionStandard(int x, int y){
    canvasWidth = x;
    canvasHeight = y;
}

void ToolBox::setDisplayResolutionStandard(int x, int y){
    canvasWidth = x;
    canvasHeight = y;
}

void ToolBox::addFrame(){
    Frame frameToInsert(canvasWidth,canvasHeight);
    //the first frame
    if(frames->size() == 0) {
        currentFrameIndex = 0;
        frames->insert(frames->begin(), frameToInsert);
        currentFrame = &(frames->at(0));
    }
    else {
        currentFrameIndex++;
        frames->insert(frames->begin() + currentFrameIndex, frameToInsert);
        currentFrame = &(frames->at(currentFrameIndex));
    }
    emit updateFrames(frames, currentFrameIndex);
    emit updateWorkspace(currentFrame);
}

void ToolBox::clearFrames(){
    frames->clear();
    emit updateFrames(frames, currentFrameIndex);
}

void ToolBox::deleteFrame(){
    if(frames->size() > 1){
        frames->erase(frames->begin()+currentFrameIndex);
        if(currentFrameIndex != 0){
            currentFrameIndex--;
            currentFrame = &(frames->at(currentFrameIndex));
        }
        else {
            currentFrame = &(frames->at(0));
        }
    }
    emit updateFrames(frames, currentFrameIndex);
    emit updateWorkspace(currentFrame);
}

void ToolBox::swapFrameLeft() {
    if(currentFrameIndex > 0) {
        swap((frames->at(currentFrameIndex)), (frames->at(currentFrameIndex - 1)));
    }
    currentFrameIndex--;
    currentFrame = &(frames->at(currentFrameIndex));
    emit updateFrames(frames, currentFrameIndex);
}

void ToolBox::swapFrameRight() {
    if(currentFrameIndex < frames->size() - 1) {
       swap((frames->at(currentFrameIndex)), (frames->at(currentFrameIndex +1)));
    }
    currentFrameIndex++;
    currentFrame = &(frames->at(currentFrameIndex));
    emit updateFrames(frames, currentFrameIndex);
}

void ToolBox::undo(){
    currentFrame->undo();
    emit updateWorkspace(currentFrame);
    emit updateFrames(frames, currentFrameIndex);
}


void ToolBox::copyFrame(){
    Frame frameToInsert(*currentFrame);
    currentFrameIndex++;
    frames->insert(frames->begin() + currentFrameIndex, frameToInsert);
    currentFrame = &(frames->at(currentFrameIndex));
    emit updateFrames(frames, currentFrameIndex);
    emit updateWorkspace(currentFrame);
}

unsigned int ToolBox::getCurrentFrameIndex() {
    return currentFrameIndex;
}

void ToolBox::changeFrame(int newIndex) {
    currentFrameIndex = newIndex;
    currentFrame = &(frames->at(currentFrameIndex));
    emit updateFrames(frames, currentFrameIndex);
    emit updateWorkspace(currentFrame);
}

void ToolBox::globalFill(int clickX, int clickY){
    QColor oldColor = currentFrame->getPixelColor(clickX, clickY);
    for(int x = 0; x < canvasWidth; x++){
        for(int y = 0; y < canvasHeight; y++){
            if(currentFrame->getPixelColor(x, y) == oldColor){
                currentFrame->setPixelColor(x, y, currentColor);
            }
        }
    }
    emit updateWorkspace(currentFrame);
}

void ToolBox::rotate(bool isRight){
    if(canvasWidth == canvasHeight){
        //Max is one less than canvas size.
        //Swap every 4 items on the canvas in the desired direction
        int Max = canvasWidth - 1;
        for (int x = 0; x < canvasWidth / 2; x++){
            for (int y = x; y < Max - x; y++){
                if(isRight){
                    QColor tempColor = currentFrame->getPixelColor(x,y);
                    currentFrame->setPixelColor(x, y, currentFrame->getPixelColor(Max - y, x));
                    currentFrame->setPixelColor(Max - y, x, currentFrame->getPixelColor(Max - x, Max - y));
                    currentFrame->setPixelColor(Max - x, Max - y, currentFrame->getPixelColor(y, Max - x));
                    currentFrame->setPixelColor(y, Max - x, tempColor);
                }
                else{
                    QColor tempColor = currentFrame->getPixelColor(x,y);
                    currentFrame->setPixelColor(x, y, currentFrame->getPixelColor(y , Max - x));
                    currentFrame->setPixelColor(y, Max - x, currentFrame->getPixelColor(Max - x, Max - y));
                    currentFrame->setPixelColor(Max - x, Max - y, currentFrame->getPixelColor(Max - y, x));
                    currentFrame->setPixelColor(Max - y, x, tempColor);
                }
            }
        }
    }
    currentFrame->saveModifications();
    emit updateFrames(frames, currentFrameIndex);
    emit updateWorkspace(currentFrame);

}

void ToolBox::changeBrushSize(bool isIncreasing){
    if(!isIncreasing && brushSize>1){
        brushSize--;
        return;
    }
    else if(isIncreasing){
        brushSize++;
    }
}
//fix oldcolor - make it come from a wrapper method
void ToolBox::fill(int x,int y){
    fillRecursive(x, y, currentFrame->getPixelColor(x,y));
    emit updateWorkspace(currentFrame);
}

void ToolBox::fillRecursive(int x,int y, QColor oldColor){
    if(x >= 0 && x < canvasWidth && y >= 0 && y < canvasHeight){
        if(currentFrame->getPixelColor(x, y) == oldColor && oldColor!=currentColor){
            //update color
            currentFrame->setPixelColor(x, y, currentColor);
            //recurse
            fillRecursive(x - 1, y, oldColor);
            fillRecursive(x + 1, y, oldColor);
            fillRecursive(x, y - 1, oldColor);
            fillRecursive(x, y + 1, oldColor);
        }
    }
}

void ToolBox::drawBrush(double x, double y){
    drawBrush(x, y, currentColor);
}

void ToolBox::drawBrush(double x, double y, QColor color){
    drawSingleBrush(x, y, color);
    double xMirrored;
    double yMirrored;
    if(isMirroredX){
        xMirrored = canvasWidth - x;
        drawSingleBrush(xMirrored, y, color);
    }
    if(isMirroredY){
        yMirrored = canvasHeight - y;
        drawSingleBrush(x, yMirrored, color);
    }
    if(isMirroredX && isMirroredY){
        //gaurenteed initialization of both xMirrored and yMirrored.
        drawSingleBrush(xMirrored, yMirrored, color);
    }
}

void ToolBox::drawSingleBrush(double x, double y, QColor color){
    //the brushSize is odd
    if(brushSize % 2 != 0){
        //from half of the total (rounded down) to the left to the right of the pixel
        for(int xOffset = (-brushSize / 2); xOffset <= brushSize / 2; xOffset++){
            //from half of the total (rounded down) above the pixel to below the pixel
            for(int yOffset = (-brushSize / 2); yOffset <= brushSize / 2; yOffset++){
                currentFrame->setPixelColor(x + xOffset, y + yOffset, color);
            }
        }
    }
    //the brushSize is even
    else{
        //regardless of specific mouse location the pixel it is in will be colored
        int xPixel = x;
        int yPixel = y;
        double xSubPixel = xPixel - x;
        double ySubPixel = yPixel - x;
        //locate the upper left pixel in the square
        int scaleFactor = (brushSize / 2) - 1;
        //change x,y to reference this upper left pixel
        xPixel -= scaleFactor;
        yPixel -= scaleFactor;
        //the subpixel coordinates will be between .00000...1 and .999999... this will refer to the point within the pixel x, y where the mouse is
        //use this to translate the upper left pixel based on subpixel
        if(xSubPixel<.5){
            xPixel--;
        }
        if(ySubPixel<.5){
            yPixel--;
        }
        for(int xCurrent = xPixel; xCurrent < xPixel + brushSize; xCurrent++){
            for(int yCurrent = yPixel; yCurrent < yPixel + brushSize; yCurrent++){
                currentFrame->setPixelColor(xCurrent, yCurrent, color);
            }
        }

    }
    emit updateWorkspace(currentFrame);
}

void ToolBox::toggleMirroringX(){
    isMirroredX = !isMirroredX;
}

void ToolBox::toggleMirroringY(){
    isMirroredY = !isMirroredY;
}

void ToolBox::drawLine(double xI, double yI, double xF, double yF){
    drawLine(xI, yI, xF, yF, currentColor);
}

void ToolBox::drawLine(double xI, double yI, double xF, double yF, QColor color){
    drawBrush(xI, yI, color);
    drawBrush(xF, yF, color);
    //if the end points are smaller than the start points, swap them so that no negatives
    //occur in later calculations.
    drawCenterPoints(xI, yI, xF, yF, color);
}

void ToolBox::drawCenterPoints(double xI, double yI, double xF, double yF, QColor color){
    double dX = xF - xI;
    double dY = yF - yI;
    //if the points are close enough together, stop recursion.
    if(std::abs(dX) <= .85 * brushSize && std::abs(dY) <= .85 * brushSize){
        return;
    }
    //calculate midpoints for drawing
    double xMid = xI + (dX / 2);
    double yMid = yI + (dY / 2);
    drawBrush(xMid, yMid, color);
    //recurse
    drawCenterPoints(xI, yI, xMid, yMid, color);
    drawCenterPoints(xMid, yMid, xF, yF, color);
}

void ToolBox::flip(bool isVerticalAxis){
    QColor tempColors[canvasWidth][canvasHeight];
    //first get all colors so none are changed before switching
    for(int x = 0; x < canvasWidth; x++){
        for(int y = 0; y < canvasHeight; y++){
            tempColors[x][y] = currentFrame->getPixelColor(x,y);
        }
    }
    //then set all colors according to the flip
    for(int x = 0; x < canvasWidth; x++){
        for(int y = 0; y < canvasHeight; y++){
            if(isVerticalAxis){
                //V-axis flip
                currentFrame->setPixelColor((canvasWidth - 1) - x, y, tempColors[x][y]);
                continue;
            }
            //H-axis flip
            currentFrame->setPixelColor(x, (canvasHeight - 1) - y, tempColors[x][y]);
        }
    }
    currentFrame->saveModifications();
    emit updateFrames(frames, currentFrameIndex);
    emit updateWorkspace(currentFrame);
}

void ToolBox::drawEraser(double x, double y){
    drawBrush(x, y, transparent);
}

void ToolBox::setGlobalFill(bool fill) {
    isGlobalFill = fill;
}

void ToolBox::clearHistory(){
    currentFrame->clearHistory();
}

void ToolBox::writeToJSON(QString filename) {
    QJsonObject spriteObject;
    spriteObject.insert("height", canvasHeight);
    spriteObject.insert("width", canvasWidth);
    QJsonValue frameNumber((int)frames->size());
    spriteObject.insert("numberOfFrames", frameNumber);
    QJsonObject framesObject;
    getFrameTuples(framesObject);

    spriteObject.insert("frames", framesObject);
    QJsonDocument spriteDoc(spriteObject);
    QFile spriteFile(filename);

    spriteFile.open(QFile::WriteOnly);
    spriteFile.write(spriteDoc.toJson());


}

void ToolBox::getFrameTuples(QJsonObject& framesObject) {

    for (int i = 0; i < (int)frames->size(); i++) {
        QJsonArray rows;
        for(int y = 0; y < canvasHeight; y++) {
            QJsonArray row;
            for(int x = 0; x < canvasWidth; x++) {
                QJsonArray colorTuple;
                QImage* workingFrame = &(frames->at(i).currentWorkspace);
                QColor color = workingFrame->pixelColor(x,y);
                colorTuple.append(color.red());
                colorTuple.append(color.green());
                colorTuple.append(color.blue());
                colorTuple.append(color.alpha());
                row.append(colorTuple);

            }
            rows.append(row);
        }
        framesObject.insert("frame" + QString::number(i), rows);
    }
}

void ToolBox::readFromJSON(QString filename) {
    QFile loadFile(filename);

    if(!loadFile.open(QIODevice::ReadOnly)) {
        emit invalidFile("Error opening file to be read. Check file format.");
        return;
    }
    QByteArray fileData = loadFile.readAll();
    QJsonDocument loadedFile(QJsonDocument::fromJson(fileData));

    if(loadedFile.isEmpty() || !loadedFile.isObject()) {
        emit invalidFile("Basic structure invalid. Please ensure appropriate .ssp file.");
        return;
    }
    QStringList keyList = loadedFile.object().keys();
    if(!keyList.contains("width") || !keyList.contains("height") || !keyList.contains("numberOfFrames") || !keyList.contains("frames")) {
        emit invalidFile("Selected .ssp file is missing required fields.");
        return;
    }

    canvasWidth = loadedFile.object().value("width").toInt();
    canvasHeight = loadedFile.object().value("height").toInt();
    emit newSpriteSignal(canvasWidth, canvasHeight);
    frames->clear();
    for(int i = 0; i < loadedFile.object().value("numberOfFrames").toInt(); i++) {
        Frame newFrame(canvasWidth, canvasHeight);
        for(int y = 0; y < canvasHeight; y++) {
            for(int x = 0; x < canvasWidth; x++) {
                QColor pixelColor;
                int r, b, g, a;
                r = loadedFile.object().value("frames").toObject().value("frame" + QString::number(i)).toArray().at(y).toArray().at(x).toArray().at(0).toInt();
                g = loadedFile.object().value("frames").toObject().value("frame" + QString::number(i)).toArray().at(y).toArray().at(x).toArray().at(1).toInt();
                b = loadedFile.object().value("frames").toObject().value("frame" + QString::number(i)).toArray().at(y).toArray().at(x).toArray().at(2).toInt();
                a = loadedFile.object().value("frames").toObject().value("frame" + QString::number(i)).toArray().at(y).toArray().at(x).toArray().at(3).toInt();
                pixelColor.setRgb(r, g, b, a);
                newFrame.currentWorkspace.setPixelColor(x,y, pixelColor);
            }
        }
        newFrame.saveModifications();
        frames->push_back(newFrame);
    }
    currentFrameIndex = 0;
    if(!frames->empty()) {
        currentFrame = &(frames->at(currentFrameIndex));
        emit updateFrames(frames, currentFrameIndex);
        emit updateWorkspace(currentFrame);
    }
    else {
        emit invalidFile("The provided .ssp file did not have any frame data. Please create a new file.");
    }
}


void ToolBox::writeToGIF(QString filename, int scale, int FPS) {

    GifWriter writer;
    int delay = 100 / FPS;
    GifBegin(&writer, filename.toStdString().c_str() , canvasWidth*scale, canvasHeight*scale, delay);
    for(Frame f: *frames) {
        QSize scaledSize(canvasWidth*scale, canvasHeight*scale);
        QImage largerF = f.currentWorkspace.scaled(scaledSize);
        GifWriteFrame(&writer, largerF.bits(), canvasWidth*scale, canvasHeight*scale, delay);
    }
    GifEnd(&writer);
}

void ToolBox::getPreviewFrame(long frameNumber){
    long size = frames->size();
    if (frameNumber >= size){
        frameNumber = 0;
    }
    if (size != 0){
        emit updatePreviewFrame(frames->at(frameNumber), frameNumber);
    }
}

