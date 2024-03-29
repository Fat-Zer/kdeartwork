//-----------------------------------------------------------------------------
//
// kpolygon - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//

#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <tqtimer.h>
#include <tqptrlist.h>

#include <kdialogbase.h>
#include <kscreensaver.h>
#include <krandomsequence.h>

class kPolygonSaver : public KScreenSaver
{
	Q_OBJECT
  TQ_OBJECT
public:
	kPolygonSaver( WId id );
	virtual ~kPolygonSaver();

	void setPolygon( int len, int ver );
	void setSpeed( int spd );

private:
	void readSettings();
	void blank();
	void initialisePolygons();
	void moveVertices();
	void initialiseColor();
	void nextColor();

protected slots:
	void slotTimeout();

protected:
	TQTimer		timer;
	unsigned	numLines;
	int			numVertices;
	int			colorContext;
	int			speed;
	TQColor		colors[64];
    int         currentColor;
	TQPtrList<TQPointArray> polygons;
	TQMemArray<TQPoint> directions;
	KRandomSequence rnd;
};

class kPolygonSetup : public KDialogBase
{
	Q_OBJECT
  TQ_OBJECT
public:
	kPolygonSetup( TQWidget *parent = 0, const char *name = 0 );
    ~kPolygonSetup();

protected:
	void readSettings();

private slots:
	void slotLength( int );
	void slotVertices( int );
	void slotSpeed( int );
	void slotOk();
	void slotHelp();

private:
	TQWidget *preview;
	kPolygonSaver *saver;

	int length;
	int vertices;
	int speed;
};

#endif

