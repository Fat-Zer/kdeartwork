// ----------------------------------------------------------------
//
// kscience - screen saver for KDE
//
// copyright (c)  Rene Beutler 1998
//

#ifndef __SCIENCE_H__
#define __SCIENCE_H__

#include <tqrect.h>
#include <tqtimer.h>
#include <kdialogbase.h>
#include <kscreensaver.h>

class TQSlider;
class TQCheckBox;

#define MAX_MODES  6

typedef signed int T32bit;

class KScienceSaver;

class KPreviewWidget : public TQWidget
{
	Q_OBJECT
  TQ_OBJECT
public:
	KPreviewWidget( TQWidget *parent );
	void paintEvent( TQPaintEvent *event );
	void notifySaver( KScienceSaver *s = 0 );
private:
	KScienceSaver *saver;
};

struct KScienceData;

class KScienceSaver : public KScreenSaver
{
	Q_OBJECT
  TQ_OBJECT
public:
	KScienceSaver( WId id, bool setup=false, bool gP=false);
	virtual ~KScienceSaver();

	void do_refresh( const TQRect & rect );
	void setMode        ( int mode );
	void setMoveX       ( signed int s );
	void setMoveY       ( signed int s );
	void setMove        ( bool s );	
	void setSize        ( signed int s );
	void setIntensity   ( signed int s );
	void setSpeed       ( signed int s );
	void setInverse     ( bool b );
	void setGravity     ( bool b );
	void setHideBG      ( bool b );

	void myAssert( bool term, const char *sMsg );

private:
	void readSettings();
	void initLens();
	void initialize();
	void releaseLens();
	void (KScienceSaver::*applyLens)(int xs, int ys, int xd, int yd, int w, int h);

protected slots:
	void slotTimeout();

protected:
	void       grabRootWindow();
	void       grabPreviewWidget();
	void       initWhirlLens();
	void       initSphereLens();
	void       initExponentialLens();
	void       initWaveLens();
	void       initCurvatureLens();
	void       blackPixel( int x, int y );
	void       blackPixelUndo( int x, int y);
	void       applyLens8bpp( int xs, int ys, int xd, int yd, int w, int h);
	void       applyLens16bpp(int xs, int ys, int xd, int yd, int w, int h);
	void       applyLens24bpp(int xs, int ys, int xd, int yd, int w, int h);
	void       applyLens32bpp(int xs, int ys, int xd, int yd, int w, int h);
	TQTimer     timer;
	bool       moveOn;
	bool       setup;
	bool       grabPixmap;
	int        mode;
	bool       inverse[MAX_MODES];
	bool       gravity[MAX_MODES];
	bool       hideBG[MAX_MODES];
	signed int size[MAX_MODES];
	signed int moveX[MAX_MODES];
	signed int moveY[MAX_MODES];
	signed int speed[MAX_MODES];
	signed int intensity[MAX_MODES];
	int        xcoord, ycoord;
	double     x, y, vx, vy;
	signed int bpp, side;
	int        border, radius, diam, origin;
	int        imgnext;
	char       blackRestore[4];
    KScienceData *d;
};


class KScienceSetup : public KDialogBase
{
	Q_OBJECT
  TQ_OBJECT
public:
	KScienceSetup(TQWidget *parent=0, const char *name=0);
	~KScienceSetup();
protected:
	void updateSettings();
	void readSettings();

private slots:
	void slotMode( int );
	void slotInverse();
	void slotGravity();
	void slotHideBG();
	void slotMoveX( int );
	void slotMoveY( int );
	void slotSize( int );
	void slotIntensity( int );
	void slotSliderPressed();
	void slotSliderReleased();
	void slotSpeed( int );
	void slotOk();
	void slotHelp();

private:
	KPreviewWidget *preview;
	KScienceSaver *saver;
	TQSlider *slideSize, *slideSpeed, *slideIntensity;
	TQSlider *slideMoveX, *slideMoveY;
	TQCheckBox *checkInverse, *checkGravity, *checkHideBG;	

	int  mode;
	bool inverse  [MAX_MODES];
	bool gravity  [MAX_MODES];
	bool hideBG   [MAX_MODES];
	int  moveX    [MAX_MODES];
	int  moveY    [MAX_MODES];	
	int  size     [MAX_MODES]; 
	int  intensity[MAX_MODES];
	int  speed    [MAX_MODES];
};
#endif
