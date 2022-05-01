#include "frame.h"
#include <stack>

Frame::Frame(int x, int y):dimensionX(x), dimensionY(y) {
    currentWorkspace = QImage(x, y, QImage::Format_ARGB32);
    currentWorkspace.fill(QColor(0, 0, 0, 0));
    // push a copy of currentworkspace for reverting to empty
    history.push(QImage(currentWorkspace));
}

Frame::Frame(const Frame& other) {
    //no need for a deep copy
    //used for the frame copy method
    dimensionX = other.dimensionX;
    dimensionY = other.dimensionY;
    history = other.history;
    currentWorkspace = other.currentWorkspace;
}

Frame& Frame::operator=(Frame other) {
    std::swap(dimensionX, other.dimensionX);
    std::swap(dimensionY, other.dimensionY);
    std::swap(currentWorkspace, other.currentWorkspace);
    std::swap(history, other.history);
    return*this;
}

void Frame::setPixelColor(int x, int y, QColor newColor){
    if(x < dimensionX && y < dimensionY && x > -1 && y > -1){
        currentWorkspace.setPixelColor(x, y, newColor);
    }
}

QColor Frame::getPixelColor(int x, int y){
    if(x < dimensionX && y < dimensionY && x > -1 && y > -1){
        return currentWorkspace.pixelColor(x, y);
    }
    else{
        return nullptr;
    }
}

void Frame::saveModifications(){
    //push a copy of the currentWorkspace
    history.push(QImage(currentWorkspace));
}

void Frame::revertModifications(){
    //set currentWorkspace to a copy of the most recent saved Qimage
    currentWorkspace = QImage(history.top());
}

void Frame::undo(){
    if(history.size() > 1){
        history.pop();
        currentWorkspace = history.top();
    }
}

void Frame::clearHistory(){
    QImage newestImage;
    newestImage = history.top();
    while(!history.empty()){
        history.pop();
    }
    history.push(newestImage);
}

QImage Frame::getUpdatedState() {
    return history.top();
}

int Frame::getHeight(){
    return dimensionY;
}

int Frame::getWidth(){
    return dimensionX;
}
