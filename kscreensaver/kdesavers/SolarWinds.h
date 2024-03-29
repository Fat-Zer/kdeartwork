#ifndef __SOLARWINDS_H__
#define __SOLARWINDS_H__
//============================================================================
//
// Terence Welsh Screensaver - Solar Winds
// http://www.reallyslick.com/
//
// Ported to KDE by Karl Robillard
//
//============================================================================


#include <tqgl.h>


#define LIGHTSIZE   64


class wind;
class TQTimer;

class SWindsWidget : public TQGLWidget
{
    Q_OBJECT
  TQ_OBJECT

public:

    enum eDefault
    {
        Regular,
        CosmicStrings,
        ColdPricklies,
        SpaceFur,
        Jiggly,
        Undertow,

        DefaultModes
    };

    SWindsWidget( TQWidget* parent=0, const char* name=0 );
    ~SWindsWidget();

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

    wind* _winds;
    unsigned char lightTexture[LIGHTSIZE][LIGHTSIZE];

    int dWinds;
    int dEmitters;
    int dParticles;
    int dGeometry;
    float dSize;
    int dParticlespeed;
    int dEmitterspeed;
    int dWindspeed;
    int dBlur;


    // Using TQTimer rather than timerEvent() to avoid getting locked out of
    // the TQEvent loop on lower-end systems.  Ian Geiser <geiseri@kde.org>
    // says this is the way to go.
    TQTimer* _timer;
    int _frameTime;

    friend class wind;
};


#ifndef UNIT_TEST
#include <kdialogbase.h>
#include <kscreensaver.h>


class KSWindsScreenSaver : public KScreenSaver
{
    Q_OBJECT
  TQ_OBJECT

public:

    KSWindsScreenSaver( WId id );
    virtual ~KSWindsScreenSaver();

    int mode() const { return _mode; }

public slots:

    void setMode( int );

private:

    void readSettings();

    SWindsWidget* _flux;
    int _mode;
};


class TQComboBox;

class KSWindsSetup : public KDialogBase
{
    Q_OBJECT
  TQ_OBJECT

public:

    KSWindsSetup( TQWidget* parent = 0, const char* name = 0 );
    ~KSWindsSetup();

private slots:

    void slotHelp();
    void slotOk();

private:

    TQComboBox* modeW;
    KSWindsScreenSaver* _saver;
};
#endif


#endif //__SOLARWINDS_H__
