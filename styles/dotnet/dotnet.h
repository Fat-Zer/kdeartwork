/*
 * $Id$
 *
 *	Copyright 2001, Chris Lee <lee@azsites.com>
 *	Originally copied from the KDE3 HighColor style, modified to fit mine.
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *	License version 2 as published by the Free Software Foundation.
 *
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Library General Public License for more details.
 *
 *	You should have received a copy of the GNU Library General Public License
 *	along with this library; see the file COPYING.LIB.  If not, write to
 *	the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *	Boston, MA 02110-1301, USA.
 */

#ifndef __DOTNET_H
#define __DOTNET_H

#include <kstyle.h>
#include <tqbitmap.h>

#define u_arrow -4,1, 2,1, -3,0, 1,0, -2,-1, 0,-1, -1,-2
#define d_arrow -4,-2, 2,-2, -3,-1, 1,-1, -2,0, 0,0, -1,1
#define l_arrow 0,-3, 0,3,-1,-2,-1,2,-2,-1,-2,1,-3,0
#define r_arrow -2,-3,-2,3,-1,-2, -1,2,0,-1,0,1,1,0

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

class dotNETstyle : public KStyle
{
	Q_OBJECT

public:
	dotNETstyle();
	virtual ~dotNETstyle();

	bool inheritsKHTML( const TQWidget* widget ) const;
	void polish( TQWidget* widget );
	void unPolish( TQWidget* widget );

	void renderMenuBlendPixmap( KPixmap&, const TQColorGroup&, const TQPopupMenu * ) const;

	void drawKStylePrimitive( KStylePrimitive kpe,
		TQPainter* p,
		const TQWidget* widget,
		const TQRect &r,
		const TQColorGroup &cg,
		SFlags flags = Style_Default,
		const TQStyleOption& = TQStyleOption::Default ) const;

	void drawPrimitive( PrimitiveElement pe,
		TQPainter *p,
		const TQRect &r,
		const TQColorGroup &cg,
		SFlags flags = Style_Default,
		const TQStyleOption &opt = TQStyleOption::Default ) const;

	void drawControl( ControlElement element,
		TQPainter *p,
		const TQWidget *widget,
		const TQRect &r,
		const TQColorGroup &cg,
		SFlags flags = Style_Default,
		const TQStyleOption& = TQStyleOption::Default ) const;

	void drawControlMask( ControlElement, TQPainter *, const TQWidget *, const TQRect &, const TQStyleOption &) const;

	void drawComplexControl( ComplexControl control,
		TQPainter *p,
		const TQWidget *widget,
		const TQRect &r,
		const TQColorGroup &cg,
		SFlags flags = Style_Default,
		SCFlags controls = SC_All,
		SCFlags active = SC_None,
		const TQStyleOption& = TQStyleOption::Default ) const;

	int pixelMetric( PixelMetric m,
		const TQWidget *widget = 0 ) const;

	TQRect subRect( SubRect r,
		const TQWidget *widget ) const;

	TQRect querySubControlMetrics( ComplexControl control,
		const TQWidget *widget,
		SubControl subcontrol,
		const TQStyleOption &opt = TQStyleOption::Default ) const;

	void drawComplexControlMask(TQStyle::ComplexControl c,
	                            TQPainter *p,
	                            const TQWidget *w,
	                            const TQRect &r,
	                            const TQStyleOption &o=TQStyleOption::Default) const;

	TQSize sizeFromContents(TQStyle::ContentsType t,
	                       const TQWidget *w,
	                       const TQSize &s,
	                       const TQStyleOption &o) const;

protected:
	void renderButton(TQPainter *p,
	                  const TQRect &r,
	                  const TQColorGroup &g,
	                  bool sunken = false,
	                  bool corners = false) const;

	void renderPanel(TQPainter *p,
	                 const TQRect &r,
	                 const TQColorGroup &g,
	                 bool sunken = true,
	                 bool thick = true) const;

	void renderSlider(TQPainter *p,
	                  const TQRect &r,
	                  const TQColorGroup &g) const;
	bool eventFilter(TQObject *, TQEvent *);

	void updatePalette( TQComboBox * );
	void updatePalette( TQToolBar * );
	void updatePalette( TQMenuBar * );

protected slots:
	void slotDestroyed();
	void paletteChanged();

private:
// Disable copy constructor and = operator
	dotNETstyle( const dotNETstyle & );
	dotNETstyle& operator=( const dotNETstyle & );
	TQStyle *winstyle;

	bool pseudo3D, useTextShadows, roundedCorners, reverseLayout, kickerMode;

	TQValueList<TQWidget*> m_widgets;
};

#endif

// vim: set noet ts=4 sw=4:
