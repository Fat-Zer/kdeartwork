// $Id$
//
// kclock - Clock screen saver for KDE
// Copyright (c) 2003   Melchior FRANZ
//
// License:		GPL v2
// Author:		Melchior FRANZ  <mfranz@kde.org>
// Dependencies:	libart_lgpl_2   http://www.levien.com/libart/
//
#ifndef __KCLOCK_H__
#define __KCLOCK_H__

#include <tqtimer.h>
#include <kdialogbase.h>
#include <kscreensaver.h>


class KClockPainter
{
	int m_width;
	int m_height;
	TQ_UINT8 *m_buf;
	double m_matrix[6];
	TQ_UINT32 m_color;
	TQ_UINT32 m_shadow;

    public:
	KClockPainter(int width, int height);
	~KClockPainter();
	void copy(KClockPainter *p);
	void drawToImage(TQImage *q, int x, int y);
	inline int width() { return m_width; }
	inline int height() { return m_height; }
	inline void *image() { return (void *)m_buf; }
	void setColor(const TQColor &color);
	void setShadowColor(const TQColor &color);
	void fill(const TQColor &color);
	void drawRadial(double alpha, double r0, double r1, double width);
	void drawDisc(double radius);
	void drawHand(const TQColor &color, double angle, double length,
			double width, bool disc);
};


class KClockSaver : public KScreenSaver
{
    Q_OBJECT
  TQ_OBJECT
    public:
	KClockSaver(WId id);
	virtual ~KClockSaver();
	inline void setBgndColor(const TQColor &c) { m_bgndColor = c; drawScale(); setBackgroundColor(c); };
	inline void setScaleColor(const TQColor &c) { m_scaleColor = c; drawScale(); };
	inline void setHourColor(const TQColor &c) { m_hourColor = c; forceRedraw(); };
	inline void setMinColor(const TQColor &c) { m_minColor = c; forceRedraw(); };
	inline void setSecColor(const TQColor &c) { m_secColor = c; forceRedraw(); };
	void setKeepCentered(bool b);
	void restart(int siz);
	inline void forceRedraw() { m_second = -1; }

    private:
	void readSettings();
	void drawScale();
	void drawClock();
	void start(int size);
	void stop();

    protected slots:
	void slotTimeout();

    protected:
	TQTimer m_timer;
	TQImage *m_image;
	KClockPainter *m_scale;
	KClockPainter *m_clock;

	int m_x;
	int m_y;
	int m_diameter;
	int m_size;
	bool m_showSecond;
	bool m_keepCentered;
	int m_hour;
	int m_minute;
	int m_second;

	TQColor m_bgndColor;
	TQColor m_scaleColor;
	TQColor m_hourColor;
	TQColor m_minColor;
	TQColor m_secColor;
};


class KClockSetup : public KDialogBase
{
    Q_OBJECT
  TQ_OBJECT
    public:
	 KClockSetup(TQWidget *parent = 0, const char *name = 0);
    ~KClockSetup();
    protected:
	void readSettings();

    private slots:
	void slotOk();
	void slotHelp();

	void slotBgndColor(const TQColor &);
	void slotScaleColor(const TQColor &);
	void slotHourColor(const TQColor &);
	void slotMinColor(const TQColor &);
	void slotSecColor(const TQColor &);
	void slotSliderMoved(int);
	void slotKeepCenteredChanged(int);

    private:
	KClockSaver *m_saver;

	TQColor m_bgndColor;
	TQColor m_scaleColor;
	TQColor m_hourColor;
	TQColor m_minColor;
	TQColor m_secColor;

	int m_size;
	bool m_keepCentered;
};

#endif


