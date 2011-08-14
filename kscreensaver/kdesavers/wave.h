//-----------------------------------------------------------------------------
//
// kwave - Partical Wave Screen Saver for KDE 2
//
// Copyright (c)  Ian Reinhart Geiser 2001
//
/////
//NOTE:
// The base particle engine did not come from me, it was designed by Jeff Molofee <nehe@connect.ab.ca>
// I did some extensive modifications to make it work with QT's OpenGL but the base principal is about
// the same.
////

#ifndef __WAVE_H__
#define __WAVE_H__

#include <tqdialog.h>
#include <tqgl.h>
#ifdef TQ_WS_MACX
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <kscreensaver.h>
#include <tqtimer.h>
#include <tqimage.h>
#include "wavecfg.h"


class Wave : public TQGLWidget
{
Q_OBJECT
  TQ_OBJECT

public:
	Wave( TQWidget * parent=0, const char * name=0 );
	~Wave();

protected:
	/** paint the GL view */
	void paintGL ();
	/** resize the gl view */
	void resizeGL ( int w, int h );
	/** setup the GL enviroment */
	void initializeGL ();


private:
//
	GLUnurbsObj *pNurb;
	GLint nNumPoints;
//	float ctrlPoints[4][4][3];
//	float knots[8];
	int index;
	bool LoadGLTextures();
	GLuint  texture[1];
	TQImage  tex;

};

class KWaveSaver : public KScreenSaver
{
Q_OBJECT
  TQ_OBJECT
public:
	KWaveSaver( WId drawable );
	virtual ~KWaveSaver();
	void readSettings();
public slots:
	void blank();

private:
	Wave *wave;
	TQTimer  *timer;
};

class KWaveSetup : public SetupUi
{
	Q_OBJECT
  TQ_OBJECT
public:
	KWaveSetup( TQWidget *parent = NULL, const char *name = NULL );
    ~KWaveSetup( );
protected:
	void readSettings();

private slots:
	void slotOkPressed();
	void aboutPressed();
private:
	KWaveSaver *saver;
	float	size;
	float	stars;
};

#endif


