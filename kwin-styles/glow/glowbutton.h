/***************************************************************************
                          glowbutton.h  -  description
                             -------------------
    begin                : Thu Sep 14 2001
    copyright            : (C) 2001 by Henning Burchardt
    email                : h_burchardt@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GLOW_BUTTON_H
#define GLOW_BUTTON_H

#include <vector>
#include <tqmap.h>
#include <tqbutton.h>

class TQPixmap;
class TQBitmap;
class TQTimer;
class TQString;

namespace Glow
{

class PixmapCache
{
public:
	static const TQPixmap* find(const TQString& key);
	static void insert(const TQString& key, const TQPixmap *pixmap);
	static void erase(const TQString& key);
	static void clear();
private:
	static TQMap<TQString, const TQPixmap*> m_pixmapMap;
};


//-----------------------------------------------------------------------------

class GlowButton : public QButton
{
	Q_OBJECT

public:
	GlowButton(TQWidget *parent, const char* name, const TQString& tip, const int realizeBtns);
	~GlowButton();

	void setTipText( const TQString& tip );

	TQString getPixmapName() const;
	ButtonState lastButton() const;

	/** Sets the name of the pixmap in the pixmap cache.
	 * If no background pixmap is wanted use TQString::null as name. */
	void setPixmapName(const TQString& pixmapName);

protected:
	virtual void paintEvent( TQPaintEvent * );
	virtual void enterEvent( TQEvent * );
	virtual void leaveEvent( TQEvent * );
	virtual void mousePressEvent( TQMouseEvent * );
	virtual void mouseReleaseEvent( TQMouseEvent * );

protected slots:
	void slotTimeout();

private:
	enum TimerStatus { Run, Stop };

	int m_updateTime;
	int _steps;
	TQString m_pixmapName;

	TQTimer *m_timer;
	int m_pos;
	TimerStatus m_timerStatus;

	int m_realizeButtons;
	ButtonState _last_button;
};

//-----------------------------------------------------------------------------

class GlowButtonFactory
{
public:
	GlowButtonFactory();

	int getSteps();

	/**
	 * Sets the number of pixmaps used to create the glow effect of the
	 * glow buttons.
	 */
	void setSteps(int steps);

	/**
	 * Creates a background pixmap for a glow button.
	 * The pixmap will consist of sub pixmaps of the size of the button which
	 * are placed one below the other. Each sub pixmap is copied on the button
	 * in succession to create the glow effect. The last sub pixmap is used
	 * when the button is pressed.
	 */
	TQPixmap * createGlowButtonPixmap(
				const TQImage & bg_image,
//				const TQImage & bg_alpha_image,
				const TQImage & fg_image,
				const TQImage & glow_image,
				const TQColor & color,
				const TQColor & glow_color);

	GlowButton* createGlowButton(
		TQWidget *parent, const char* name, const TQString& tip, const int realizeBtns = Qt::LeftButton);

private:
	int _steps;
};

} // namespace

#endif
