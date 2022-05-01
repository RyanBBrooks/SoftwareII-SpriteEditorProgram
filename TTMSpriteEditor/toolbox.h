#ifndef TOOLBOX_H
#define TOOLBOX_H
#include <vector>
#include <frame.h>
#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPushButton>
#include <QLabel>

using std::vector;
using std::swap;

class ToolBox : public QObject
{
    Q_OBJECT
public:
    /**
     * This stores the frames of the sprite.
     */
    vector<Frame>* frames;

    /**
     * bool reffering to if global fill is on (I.E. if shift is held.)
     */
    bool isGlobalFill;
    /**
    * Stores whether the mirror tool is mirroring over X
    */
    bool isMirroredX;
    /**
    * Stores whether the mirror tool is mirroring over X
    */
    bool isMirroredY;
    ToolBox();
    ~ToolBox();

public slots:
    /**
     * on mouse click down
     */
    void mouseDown(QMouseEvent*, QLabel*);
    /**
     * on mouse click release
     */
    void mouseUp(QMouseEvent*);
    /**
     * on mouse move
     */
    void mouseMove(QMouseEvent*);
    /**
     * on keypress
     */
    void keyPressed(QKeyEvent*);
    /**
     * on tool Changed
     */
    void toolChanged(QObject*);
    /**
     * on color selection
     */
    void changePenColor(QColor);

    /**
     * sets the toolbox's isGlobalFill to the provided state
     */
    void setGlobalFill(bool);

    /**
     * setFrameResolutionStandard on sprite creation
     */
    void setFrameResolutionStandard(int, int);
    /**
     * setFrameResolutionStandard on sprite creation
     */
    void setDisplayResolutionStandard(int, int);

    /**
     * resets the last action
     */
    void undo();

    /**
     *  Changes the size of the pencil and eraser brushes. isIncreasing=true if the brush size should increase, false otherwise.
     */
    void changeBrushSize(bool);

    /**
     * Adds a blank frame to the frame following the currently active frame.
     * This new frame is the new active frame.
     */
    void addFrame();

    /**
     * empties all frames from frames
     */
    void clearFrames();

    /**
     * Deletes the current frame.  If this is the first frame, the active frame becomes the next frame, otherwise it becomes the previous one.
     * Only deletes a frame if there are at least two frames.
     */
    void deleteFrame();

    /**
     * Swaps the current frame with the frame to the left.
     * If the current frame is the furthest left frame, nothing happens.
     */
    void swapFrameLeft();

    /**
     * Swaps the current frame with the frame to the right.
     *If the current frame is the furthest right frame, nothing happens.
     */
    void swapFrameRight();

    /**
     * Copies the currently active frame, placing a newly created duplicate to the
     * right of the active frame, and then setting the duplicate as the active frame.
     */
    void copyFrame();

    /**
     * returns the current frame index
     */
    unsigned int getCurrentFrameIndex();

    void changeFrame(int);
    /**
     * Rotates the current frame by 90 degrees if true then to the right, left otherwise.
     */
    void rotate(bool);

    /**
     * If bool is true flips pixels over vertical axis otherwise over horizontal axis.
     */
    void flip(bool);

    /**
     * Inverts the state of Xaxis mirroring
     */
    void toggleMirroringX();

    /**
     * Inverts the state of Yaxis mirroring
     */
    void toggleMirroringY();

    /**
     * Takes the sprite and turns it into a JSON document
     */
    void writeToJSON(QString);

    /**
     * Takes a JSON document and opens a sprite editor.
     */
    void readFromJSON(QString);

    /**
     * Writes the sprite to an animated gif with the provided filename
     */
    void writeToGIF(QString, int, int);
    /**
     * Returns the frame for use in the preview animation
     */
    void getPreviewFrame(long);

signals:
    /**
     * Updates the view to reflect this model.
     */
    void updateWorkspace(Frame*);

    /**
     * tells the view to update its frame list and the images
     * dependent on the list of frames.
     */
    void updateFrames(vector<Frame>*, int);

    /**
     * Emits when unable to load file.
     */
    void invalidFile(QString);

    /**
     * For creating a new sprite based on an opened .ssp file
     */
    void newSpriteSignal(int, int);

    /**
     * for updating the state of the preview
     */
    void updatePreviewFrame(Frame, long);

private:
    // pointer to the model's workspace
    QLabel*workspacePtr = nullptr;

    QColor transparent = QColor(0, 0, 0, 0);
    /**
     * The Resoluton Of current Sprite - X
     */
    int canvasWidth;

    /**
     * The Resoluton Of current Sprite - Y
     */
    int canvasHeight;

    /**
     * Wrapper that draws it in currentColor
     */
    void drawBrush(double, double);

    /**
     * Draws on the page based on x and y and the brush size as well as the current mirrors.
     */
    void drawBrush(double, double, QColor color);

    /**
     * Draws a single instance of brush onto the page in the specified color. the size of brush is based on brushSize.
     */
    void drawSingleBrush(double, double, QColor color);

    /**
     * Erases on the page based on x and y and the brush size as well as the current mirrors.
     */
    void drawEraser(double, double);

    /**
     * Wrapper that draws it in currentColor
     */
    void drawLine(double, double, double, double);

    /**
     * Draws a line from point xI yI to xF yF.
     */
    void drawLine(double, double, double, double, QColor);

    /**
     * Draws the center point between two points recursivley until the points form a line.
     */
    void drawCenterPoints(double, double, double, double, QColor);

    /**
     * the color currently being drawn in
     */
    QColor currentColor;

    /**
     * The current position of the mouse in X based on the grid
     */
    double mouseX;

    /**
     * The current position of the mouse in Y based on the grid
     */
    double mouseY;

    /**
     * The current position of the mouse in X based on the grid
     */
    double mouseClickedX;

    /**
     * The current position of the mouse in Y based on the grid
     */
    double mouseClickedY;
    int labelXPos;
    int labelYPos;

   /**
    * The last position of the mouse in X - grid
    */
    double lastMouseX;

   /**
     * The last position of the mouse in Y - grid
     */
    double lastMouseY;

    /**
     * The current display Height - set when the user zooms or when a frame is switched to or created
     */
    double displayHeight;

    /**
     * The current display Width - set when the user zoom  or when a frame is switched to or created
     */
    double displayWidth;

    /**
     * boolean referring to if the mouse is being held and not canceled
     */
    bool mouseIsActive;

    /**
     * the lastButtonClicked on screen
     */
    QObject*lastButtonClicked;



    /**
     * This states what the currently active frame is.
     */
    unsigned int currentFrameIndex;

    /**
     * Stores a pointer to the currently active frame.
     */
    Frame* currentFrame;

    /**
    * Stores the current brush size. the number means the brush for pencil and eraser has an area of BrushSize by BrushSize pixels.
    */
    int brushSize;
    /**
     * Updates X and Y based on current location of the mouse, zoom, and image size,
     * sets the old X, Y to last X, Y for smoothing out choppy brush strokes.
     */
    void updateXY(double, double);

    /**
     * Converts a given x, y by reference to the grid system from pixels
     * grid system is a double from 0 to MaxX.9999 or MaxY.9999
     * dividing displayWidth by CanvasWidth etc. gives us the pixel size to
     * divide the location of our current pixel by.
     * i.e. if the canvas size is 10x10 and the display resolution is 100x100
     * the current mouse position in screen pixels is 11x11, the pixel size will
     * be 10 and we will divide 11 by 10 meaning it is 1.1x1.1 pixels on the grid system
     */
    void convertXY(double&, double&);

    /**
     * clears the history for the current frame
     */
    void clearHistory();

    /**
     * Replaces all pixels of 'oldColor' with 'newColor' on the current frame.
     */
    void globalFill(int,int);

    /**
     * Replaces all pixels from x,y that are connected and oldColor with newColor on the current frame.
     */
    void fill(int, int);

    /**
     * helper method to fill checking all adjacent squares.
     */
    void fillRecursive(int x,int y, QColor oldColor);

    /**
     * Puts each frame's pixel data into a QJsonObject, each frame being a QJsonArray
     */
    void getFrameTuples(QJsonObject&);


};

#endif // TOOLBOX_H
