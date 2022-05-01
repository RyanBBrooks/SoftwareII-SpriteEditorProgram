
#ifndef FRAME_H
#define FRAME_H
#include <QImage>
#include <QColor>
#include <stack>

/**
 * Holds a QImage to be displayed in the view's preview pane or in the workspace.
 */
class Frame
{
public:
    /**
     * Holds the image of the form
     */
    QImage currentWorkspace;
    /**
     * Constructs a Frame of the given width and height (x and y);
     */
    Frame(int, int);
    /**
     * A copy constructor for Frame
     */
    Frame(const Frame&);
    /**
     * Overrides '=' for Frame
     */
    Frame& operator=(Frame);
    /**
    * Updates a pixel at a certain position to have a different color in currentView
    */
    void setPixelColor(int,int,QColor);
    /**
    * Retrives the color of a pixel from currentView
    */
    QColor getPixelColor(int,int);
    /**
    * Retrieves the width of currentView
    */
    int getWidth();
    /**
    * Retrieves the height of currentView
    */
     int getHeight();
     /**
      * Resets the image to the most recent action
      */
     void undo();
     /**
      * Returns the top of history, in order to always accurately reflect
      * changes made to the frame, including undos and any frame manipulation tools
      */
     QImage getUpdatedState();
     /**
      * Clears the history of changes
      */
     void clearHistory();
     /**
      * Saves the pending changes made to the current image displaying for this frame.
      */
     void saveModifications();
     /**
      * Reverts any pending changes made to the current image for this frame.
      */
     void revertModifications();

private:
    // dimensions
    int dimensionX;
    int dimensionY;
    std::stack<QImage> history;
};

#endif // FRAME_H
