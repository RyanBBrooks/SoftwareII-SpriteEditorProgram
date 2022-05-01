#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include "toolbox.h"
#include <QToolButton>
#include <QScrollArea>
#include <QHBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * The main unit of our GUI. The user interacts with this object using key
 * presses and mouse clicks, and the information is passed off to the model
 * class, which is our ToolBox, for the most part.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructs the MainWindow.
     */
    MainWindow(ToolBox*);

    /**
     * Destroys resources managed by the class.
     */
    ~MainWindow();

signals:
    /**
     * emit on mouseDown, pass the event to the model to determine what
     * should be done.
     */
    void mouseDown(QMouseEvent*, QLabel*);

    /**
     * emit on mouseUp, pass the event to the model to determine what
     * should be done.
     */
    void mouseUp(QMouseEvent*);

    /**
     * emit on KeyPress, pass the event to the model.
     */
    void keyPressed(QKeyEvent*);

    /**
     * emit on toolSelection, pass to the model so that the next mouse
     * maneuvers can be dealt with according to which tool was selected.
     */
    void toolChanged(QObject*);

    /**
     * emit on colorPicked in the QColorDialog
     */
    void penColorChanged(QColor);

    /**
     * emit on either decrease or increase pen size buttons pressed,
     * update the size in the model.
     */
    void penSizeChanged(bool);

    /**
     * emit on flip buttons pressed, true for "is horizontal", false for a
     * vertical flip of the drawing canvas.
     */
    void flip(bool);

    /**
     * emit on rotate buttons pressed, true for "is to the left", false for
     * a flip to the right. all rotations are 90 degrees.
     */
    void rotate(bool);

    /**
     * emit on mouse moved, passed to the model
     */
    void mouseMove(QMouseEvent*);

    /**
     * emit on creation of sprite
     */
    void setFrameResolution(int, int);

    /**
     * emit on creation of sprite
     */
    void setDisplayResolution(int,int);

    /**
     * emit on new frame button pressed, adds a frame to the model, which in
     * turn updates the frame layout in the view.
     */
    void addFrame();

    /**
     * emit on clear frames button pressed, clears frames out
     * of the model, which in turn clears the frames from the view.
     */
    void clearFrames();

    /**
     * emits after a frame is selected in the frame list portion
     * of the GUI. updates the model's current frame, which is the on
     * which is editable.
     */
    void selectModelFrame(int);

    /**
     * emit on undo button, menu option, or shortcut 'ctrl + Z' pressed on
     * the keyboard. undos the most recent so-many actions in the model
     * and results in the workspace being reverted to what it was
     * immediately before.
     */
    void undoPressed();

    /**
     * emit on mirroring X button pressed or shortcut 'X' on the
     * keyboard. having this feature turned on allows the user
     * to draw on the top and bottom halves of the screen at the
     * same time in a mirror fashion.
     */
    void toggleMirroringX();

    /**
     * emit on mirroring Y button pressed or shortcut 'Y' on the
     * keyboard. having this feature turned on allows the user
     * to draw on the top and bottom halves of the screen at the
     * same time in a mirror fashion.
     */
    void toggleMirroringY();

    /**
     * emit to save the object to JSON
     */
    void writeToJSON(QString);

    /**
     * emit to tell the model to set its global fill variable
     */
    void setGlobalFill(bool);

    /**
     * emit to open a JSON object and open a project
     */
    void readFromJSON(QString);

    /**
     * emit to export a sprite to animated GIF format
     */
    void writeToGIF(QString, int, int);

    /**
     * emit to retrieve a given frame
     */
    void getPreviewFrame(long);

public slots:
    /**
     * shows a dialogue containing some useful shortcuts to the user.
     */
    void showShortcuts();

    /**
     * puts the UI in dark mode if it's in light mode, or puts the UI in
     * light mode if it's in dark mode. changes the state of isDark.
     */
    void toggleDarkMode();

    /**
     * slot called when something in the model is changed that requires
     * the view to also be updated.
     */
    void updateWorkspace(Frame*);

    /**
     * called to update the frames section of the GUI. Passes in the current frame's index
     */
    void updateFrames(vector<Frame>*, int);

    /**
     * Responds to clicks in the frames section. Should then update the model's
     * selected frame.
     */
    void selectFrame();

    /**
     * called on tool button press to determine functionality
     */
    void toolButtonPressed();

    /**
     * called on action tool button press to determine functionality --
     * redirects in these special "action" cases when the user is not simply
     * in "drawing mode", for example, but used in cases like flipping the canvas
     * vertically or horizontally.
     */
    void actionToolButtonPressed();

    /**
     * Slot called by view when the user selects a new color.
     */
    void changePenColor(const QColor&);

    /**
     * Slot called on new Sprite selected from the menu or the "ctrl + N" shortcut
     */
    void displayNewSpriteDialog();

    /**
     * when attempting to load an invalid file, displays an appropriate error message
     */
    void displayError(QString);

    /**
     * Slot called on save sprite selected from the menu or the "ctrl + S" shortcut
     */
    void saveSprite();

    /**
     * Slot called on open file selected from the menu or the "ctrl + O" shortcut
     */
    void openFile();

    /**
     * Slot called on export sprite selected from the menu or the "ctrl + E" shortcut
     * provides functionality for exporting to a gif!
     */
    void exportSprite();

    /**
     * called when the new sprite dialog box is closed by the user after new file info has been entered,
     * or when we need to create a new sprite from a .ssp file
     */
    void makeNewSprite(int, int);

    /**
     * Adjusts the speed at which frames are updated.
     */
    void setFPS(int);

    /**
     * Retrieves the current FPS speed used for other methods
     * @return the current FPS
     */
    int getFPS();

    /**
     * Handles the frame animation through repeated timed calls
     */
    void animate();

    /**
     * Called to update the frame shown
     */
    void animatePreview(Frame frame, long);

private:
    /**
     * the main interface through which the user interacts with the program.
     */
    Ui::MainWindow* ui;

    /**
     * a flag to aide in preventing progress loss. if the program attempts to close
     * while this is true, a dialog will show asking the user if they are sure they'd
     * like to quit without saving.
     */
    bool editedWithoutSave;

    /**
     * a flag for if the UI is in dark mode or not
     */
    bool isDark;

    /**
     * the existing Qt object that we dock in the main window. allows the color of the
     * pen to be set by the user.
     */
    QColorDialog* colorDialog;

    /**
     * true if the user can draw on the canvas at any given moment, false if not
     */
    bool canEdit;


    /**
     * a storage container for all of the relevant tool buttons so that they
     * can be iterated through. very useful for enabling/disabling or changing
     * the color of them en masse.
     */
    std::vector<QToolButton*>* toolButtons;

    /**
     * a storage container for all of the relevant frame buttons so that they
     * can be iterated through. very useful for enabling/disabling or changing
     * the color of them en masse.
     */
    std::vector<QPushButton*>* frameButtons;

    /**
     * Overloaded close event to aide in preventing loss of unsaved progress.
     */
    void closeEvent(QCloseEvent* event);

    /**
     * widget built inside the constructor, responsible for scrolling list of frames
     */
    QScrollArea* frameList;

    /**
     * interior widget of framelist, organizes the frames added in
     */
    QWidget* scrollable;

    // ** universal color channels for in-use tool buttons (166,255,178):
    /**
     * the amount of red in the color of highlighted buttons
     */
    const int toolR = 166;

    /**
     * the amount of green in the color of highlighted buttons
     */
    const int toolG = 255;

    /**
     * the amount of blue in the color of highlighted buttons
     */
    const int toolB = 178;

    /**
     * sets the color of all relevant tool buttons to the universal default.
     * the relevant buttons are: pencil, eraser, fill, and line.
     */
    void resetPenButtons();

    /**
     * adds all of the relevant buttons to the vector
     */
    void setUpButtonVectors();

    /**
     * Makes all applicable buttons enabled or disabled, depending on the value
     * of the incoming boolean.
     */
    void enableAll(bool);

    /**
     * Draws A frame scaling it in the workspace and sends resolution to model
     */
    void drawWorkspace(Frame);

    /**
     * Sets the color of the incoming button in the ui.
     */
    void toggleButtonColor(QToolButton*);


    /**
     * Overloaded events inherited from QMainWindow
     */
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);

    // Live Preview Items
    /**
     * Stores the speed at which frames should be shown
     * Constricted from 0 to 60 frames per second. If 0, the frame will not change.
     */
    int fpsCount;

    /**
     * The current frame being shown
     */
    long frameIndex;
};

#endif // MAINWINDOW_H
