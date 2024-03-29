#ifndef __EUPHORIASS_H__
#define __EUPHORIASS_H__
//============================================================================
//
// Terence Welsh Screensaver - Euphoria
// http://www.reallyslick.com/
//
// KDE port by Karl Robillard
//
//============================================================================


#include <tqgl.h>

#include <kdialogbase.h>

class wisp;
class TQTimer;

class EuphoriaWidget : public TQGLWidget
{
    Q_OBJECT
  TQ_OBJECT

public:

    enum eDefault
    {
        Regular,
        Grid,
        Cubism,
        BadMath,
        MTheory,
        UHFTEM,   // ultra high frequency tunneling electron microscope
        Nowhere,
        Echo,
        Kaleidoscope,
        DefaultModes
    };

    EuphoriaWidget( TQWidget* parent=0, const char* name=0 );
    ~EuphoriaWidget();

    void updateParameters();
    void setDefaults( int which );

protected:

    void paintGL();
    void resizeGL( int w, int h );
    void initializeGL();
#ifdef UNIT_TEST
    void keyPressEvent( TQKeyEvent* );
#endif

private slots:

    void nextFrame();

private:

    GLuint texName;
    wisp* _wisps;
    wisp* _backwisps;
    unsigned char* feedbackmap;
    float aspectRatio;
    int viewport[4];
    double elapsedTime;

    // feedback texture object
    unsigned int feedbacktex;
    int feedbacktexsize;
    // feedback variables
    float fr[4];
    float fv[4];
    float f[4];
    // feedback limiters
    float lr[3];
    float lv[3];
    float l[3];



    int dWisps;
    int dBackground;
    int dDensity;
    int dVisibility;
    float dSpeed;
    int dFeedback;
    int dFeedbackspeed;
    int dFeedbacksize;
    int dWireframe;
    int dTexture;
    int dPriority;


    // Using TQTimer rather than timerEvent() to avoid getting locked out of
    // the TQEvent loop on lower-end systems.  Ian Geiser <geiseri@kde.org>
    // says this is the way to go.
    TQTimer* _timer;
    int _frameTime;

    friend class wisp;
};


#ifndef UNIT_TEST
#include <tqdialog.h>
#include <kscreensaver.h>


class KEuphoriaScreenSaver : public KScreenSaver
{
    Q_OBJECT
  TQ_OBJECT

public:

    KEuphoriaScreenSaver( WId id );
    virtual ~KEuphoriaScreenSaver();

    int mode() const { return _mode; }

public slots:

    void setMode( int );

private:

    void readSettings();

    EuphoriaWidget* _effect;
    int _mode;
};


class TQComboBox;

class KEuphoriaSetup : public KDialogBase
{
    Q_OBJECT
  TQ_OBJECT

public:
    KEuphoriaSetup( TQWidget* parent = 0, const char* name = 0 );
    ~KEuphoriaSetup();

private slots:
    void slotHelp();
    void slotOk();

private:
    TQComboBox* modeW;
    KEuphoriaScreenSaver* _saver;
};
#endif

#endif //__EUPHORIASS_H__
