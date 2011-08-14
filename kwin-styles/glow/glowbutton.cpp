/***************************************************************************
                          glowbutton.cpp  -  description
                             -------------------
    begin                : Thu Sep 6 2001
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

#include <math.h>
#include <iostream>
#include <vector>
#include <tqmap.h>
#include <tqpixmap.h>
#include <tqpixmapcache.h>
#include <tqbitmap.h>
#include <tqpainter.h>
#include <tqimage.h>
#include <tqtimer.h>
#include <tqtooltip.h>
#include <kdecoration.h>
#include <kiconeffect.h>
#include "glowbutton.h"

namespace Glow
{

//-----------------------------------------------------------------------------
// PixmapCache
//-----------------------------------------------------------------------------

TQMap<TQString, const TQPixmap*> PixmapCache::m_pixmapMap;

const TQPixmap* PixmapCache::find(const TQString& key)
{
	TQMap<TQString, const TQPixmap*>::const_iterator it =
		m_pixmapMap.find(key);
	if( it != m_pixmapMap.end() )
		return *it;
	else
		return 0;
}

void PixmapCache::insert(const TQString& key, const TQPixmap *pixmap)
{
	m_pixmapMap[key] = pixmap;
}

void PixmapCache::erase(const TQString& key)
{
	TQMap<TQString, const TQPixmap*>::iterator it =
		m_pixmapMap.find(key);
	if (it != m_pixmapMap.end())
	{
		delete *it;
		m_pixmapMap.erase(it);
	}
}

void PixmapCache::clear()
{
	// delete all pixmaps in the cache
	TQMap<TQString, const TQPixmap*>::const_iterator it
		= m_pixmapMap.begin();
	for(; it != m_pixmapMap.end(); ++it)
		delete *it;
	m_pixmapMap.clear();
}

//-----------------------------------------------------------------------------
// GlowButton
//-----------------------------------------------------------------------------

GlowButton::GlowButton(TQWidget *parent, const char *name,
	const TQString& tip, const int realizeBtns)
	: TQButton(parent, name)
{
	m_realizeButtons = realizeBtns;

	_steps = 0;
	m_updateTime = 50;
	m_pixmapName = TQString();

	m_timer = new TQTimer(this);
	connect(m_timer, TQT_SIGNAL(timeout()), this, TQT_SLOT(slotTimeout()));
	m_pos = 0;
	m_timertqStatus = Stop;

	setTipText (tip);
	setCursor(arrowCursor);
}

GlowButton::~GlowButton()
{
}

void GlowButton::setTipText( const TQString& tip )
{
	if (KDecoration::options()->showTooltips())
	{
	    TQToolTip::remove( this );
	    TQToolTip::add( this, tip );
	}
}

TQString GlowButton::getPixmapName() const
{
	return m_pixmapName;
}

TQt::ButtonState GlowButton::lastButton() const
{
	return _last_button;
}

void GlowButton::setPixmapName(const TQString& pixmapName)
{
	m_pixmapName = pixmapName;

	const TQPixmap *pixmap = PixmapCache::find(pixmapName);
	if( ! pixmap )
		return;

	// set steps
	_steps = pixmap->height()/pixmap->width() - 1;

	tqrepaint(false);
}

void GlowButton::paintEvent( TQPaintEvent *e )
{
 	TQWidget::paintEvent(e);
	const TQPixmap *pixmap = PixmapCache::find(m_pixmapName);
	if( pixmap != 0 )
	{
		int pos = m_pos>=0?m_pos:-m_pos;
		TQPainter p;
		TQPixmap pm (pixmap->size());
		p.begin(&pm);
		const TQPixmap * bg_pixmap = PixmapCache::find(
				TQString::number(parentWidget()->winId()));
		p.drawPixmap (0, 0, *bg_pixmap, x(), y(), width(), height());
		p.drawPixmap (0, 0, *pixmap, 0, pos*height(), width(), height());
		p.end();
		p.begin(this);
		p.drawPixmap (0, 0, pm);
		p.end();
	}
}

void GlowButton::enterEvent( TQEvent *e )
{
	if( m_pos<0 )
		m_pos=-m_pos;
	m_timertqStatus = Run;
	if( ! m_timer->isActive() )
		m_timer->start(m_updateTime);
	TQButton::enterEvent(e);
}

void GlowButton::leaveEvent( TQEvent *e )
{
	m_timertqStatus = Stop;
	if( ! m_timer->isActive() )
		m_timer->start(m_updateTime);
	TQButton::leaveEvent(e);
}

void GlowButton::mousePressEvent( TQMouseEvent *e )
{
	_last_button = e->button();
	if( m_timer->isActive() )
		m_timer->stop();
	m_pos = _steps;
	tqrepaint(false);
	// without pretending LeftButton, clicking on the button with MidButton
	// or RightButton would cause unwanted titlebar action
	TQMouseEvent me (e->type(), e->pos(), e->globalPos(),
			(e->button()&m_realizeButtons)?Qt::LeftButton:Qt::NoButton, e->state());
	TQButton::mousePressEvent(&me);
}

void GlowButton::mouseReleaseEvent( TQMouseEvent *e )
{
	_last_button = e->button();
	TQPoint p = mapToParent(mapFromGlobal(e->globalPos()));
	if( ! m_timer->isActive() ) {
		m_timer->start(m_updateTime);
	}
	if( ! tqgeometry().contains(p) ) {
		m_timertqStatus = Stop;
	}
	TQMouseEvent me (e->type(), e->pos(), e->globalPos(),
			(e->button()&m_realizeButtons)?Qt::LeftButton:Qt::NoButton, e->state());
	TQButton::mouseReleaseEvent(&me);
}

void GlowButton::slotTimeout()
{
	tqrepaint(false);

	if( m_pos>=_steps-1 ) {
		m_pos = -m_pos;
	}
	if( m_timertqStatus==Stop ) {
		if( m_pos==0 ) {
			m_timer->stop();
			return;
		} else if( m_pos>0 ) {
			m_pos = -m_pos;
		}
	}

	m_pos++;
}

//-----------------------------------------------------------------------------
// GlowButtonFactory
//-----------------------------------------------------------------------------

GlowButtonFactory::GlowButtonFactory()
{
	_steps = 20;
}

int GlowButtonFactory::getSteps()
{
	return _steps;
}

void GlowButtonFactory::setSteps(int steps)
{
	_steps = steps;
}

TQPixmap * GlowButtonFactory::createGlowButtonPixmap(
				const TQImage & bg_image,
				const TQImage & fg_image,
				const TQImage & glow_image,
				const TQColor & color,
				const TQColor & glow_color)
{
		if (bg_image.size() != fg_image.size()
			|| fg_image.size() != glow_image.size()) {
				std::cerr << "Image size error" << std::endl;
				return new TQPixmap();
		}

		TQImage colorized_bg_image = bg_image.copy();
		KIconEffect::colorize (colorized_bg_image, color, 1.0);

		int w = colorized_bg_image.width();
		int h = colorized_bg_image.height();

		TQImage image (w, (_steps+1)*h, 32);
		image.setAlphaBuffer (true);
		for (int i=0; i<_steps+1; ++i) {
			for (int y=0; y<h; ++y) {
				uint * src1_line = (uint*) colorized_bg_image.scanLine (y);
				uint * src2_line = (uint*) fg_image.scanLine (y);
				uint * dst_line = (uint*) image.scanLine (i*h+y);
				for (int x=0; x<w; ++x) {
					int r = tqRed (*(src1_line+x));
					int g = tqGreen (*(src1_line+x));
					int b = tqBlue (*(src1_line+x));
					int a = TQMAX (tqAlpha(*(src1_line+x)),tqGray(*(src2_line+x)));
					*(dst_line+x) = tqRgba (r, g, b, a);
				}
			}
		}
		TQPixmap * pixmap = new TQPixmap (image);
		TQPainter painter (pixmap);

		bool dark = (tqGray(color.rgb()) <= 127);
		TQImage fg_img (w, h, 32);
		fg_img.setAlphaBuffer (true);
		for (int y=0; y<h; ++y) {
			uint * src_line = (uint*) fg_image.scanLine (y);
			uint * dst_line = (uint*) fg_img.scanLine (y);
			for (int x=0; x<w; ++x) {
				int alpha = tqGray (*(src_line+x));
				if (dark)
						*(dst_line+x) = tqRgba (255, 255, 255, alpha);
				else
						*(dst_line+x) = tqRgba (0, 0, 0, alpha);
			}
		}

		int r = glow_color.red();
		int g = glow_color.green();
		int b = glow_color.blue();
		TQImage glow_img (w, h, 32);
		glow_img.setAlphaBuffer (true);
		for (int i=0; i<_steps; ++i) {
			painter.drawImage (0, i*h, fg_img);
			for (int y=0; y<h; ++y) {
				uint * src_line = (uint*) glow_image.scanLine(y);
				uint * dst_line = (uint*) glow_img.scanLine(y);
				for (int x=0; x<w; ++x) {
					int alpha =
							(int) (tqGray (*(src_line+x)) * ((double) i/_steps));
					*(dst_line+x) = tqRgba (r, g, b, alpha);
				}
			}
			painter.drawImage (0, i*h, glow_img);
		}
		painter.drawImage (0, _steps*h, fg_img);
		for (int y=0; y<h; ++y) {
				uint * src_line = (uint*) glow_image.scanLine (y);
				uint * dst_line = (uint*) glow_img.scanLine (y);
				for (int x=0; x<w; ++x) {
					int alpha = tqGray (*(src_line+x));
					*(dst_line+x) = tqRgba (r, g, b, alpha);
				}
		}
		painter.drawImage (0, _steps*h, glow_img);

		return pixmap;
}

GlowButton* GlowButtonFactory::createGlowButton(
	TQWidget *parent, const char* name, const TQString& tip, const int realizeBtns)
{
	GlowButton *glowButton = new GlowButton(parent, name, tip, realizeBtns);
	return glowButton;
}

}

#include "glowbutton.moc"

