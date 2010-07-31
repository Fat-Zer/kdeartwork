/*
  $Id$

  Gallium-IceWM themeable KWin client

  Copyright 2001
	Karol Szwed <gallium@kde.org>
	http://gallium.n3.net/

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.

  -----------------------------------------------------------------------------
  This client loads most icewm 1.0.X pixmap themes, without taking into account
  specific font settings for clients, or coloured mouse cursors. Titlebar
  fonts can be changed via the kde control center. Bi-colour mouse cursors
  may be added in future if requested by users, as well as theme font support.
  Any styles using inbuilt icewm titlebar drawing without using pixmaps (e.g.
  Warp4, win95 etc.) are not fully supported, and may cause drawing errors,
  as these themes use in-built icewm drawing mechanisms.

  When a pixmap theme is not present (or a corrupt one is present) then very
  plain title decorations are painted instead, so that users don't see
  non-painted window areas where possible ;)

  At a later date, frame shaping may be added if really requested, and an
  update to support the latest icewm 1.1.X theme format may be made.
*/

#ifndef __KDEGALLIUM_ICEWM_H
#define __KDEGALLIUM_ICEWM_H

#include <tqbutton.h>
#include <tqlayout.h>
#include <kpixmap.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <tqbutton.h>
class QLabel;
class QSpacerItem;
class QBoxLayout;
class QGridLayout;

namespace IceWM {

class IceWMClient;

// Pixmap group
enum { InActive=0, Active };
// Pixmap stretching mode
enum { Vertical=0, Horizontal=1 };


// Handles the resetClients() signal from the Options class,
// and manages the dynamic pixmaps, configuration and theme changing
class ThemeHandler: public KDecorationFactory
{
	public:
		ThemeHandler();
		~ThemeHandler();

		virtual KDecoration* createDecoration( KDecorationBridge* );
		virtual bool reset( unsigned long changed );
		virtual bool supports( Ability ability );

	private:
		bool initialized;
		TQString themeName;

		void readConfig();
		TQColor decodeColor( TQString& s );
		bool isFrameValid();
		void initTheme();
		void freePixmaps();
		void freePixmapGroup( TQPixmap* p[] );
		void setPixmap( TQPixmap* p[], TQString s1, TQString s2, bool
						stretch=false, bool stretchHoriz=true );
		void setPixmapButton( TQPixmap* p[], TQString s1, TQString s2);
		TQPixmap* stretchPixmap( TQPixmap* src, bool stretchHoriz=true,
								int stretchSize=-1);
		TQPixmap* duplicateValidPixmap( bool act, int size = -1 );
		void convertButtons( TQString& s );
		TQString reverseString( TQString s );
};


class IceWMButton : public QButton
{
	public:
		IceWMButton( IceWMClient *parent=0, const char *name=0,
					 TQPixmap* (*p)[2]=0L, bool isToggle=false,
					 const TQString& tip=NULL, const int realizeBtns = LeftButton );
		void setTipText(const TQString &tip);
		void  usePixmap( TQPixmap* (*p)[2] );
		TQSize sizeHint() const;
		void  turnOn( bool isOn );
		ButtonState   last_button;

	protected:
		void mousePressEvent( TQMouseEvent* e );
		void mouseReleaseEvent( TQMouseEvent* e );

		void drawButton( TQPainter *p );
		void drawButtonLabel( TQPainter * ) {;}

	private:
		int m_realizeButtons;
		IceWMClient* client;
		TQPixmap* (*pix)[2]; // Points to active/inactive pixmap array
};


class IceWMClient : public KDecoration
{
    Q_OBJECT
	public:
	    IceWMClient( KDecorationBridge* bridge, KDecorationFactory* factory );
	    ~IceWMClient();

		virtual void init();
		virtual void resize(const TQSize&);
		virtual bool eventFilter( TQObject* o, TQEvent* e );

	protected:
	    void resizeEvent( TQResizeEvent* );
	    void paintEvent( TQPaintEvent* );
	    void showEvent( TQShowEvent* );
	    void mouseDoubleClickEvent( TQMouseEvent * );
	    void wheelEvent( TQWheelEvent * );
	    virtual void captionChange();
	    virtual void maximizeChange();
	    virtual void shadeChange();
	    virtual void activeChange();
	//	void shadeChange(bool);     /* KWin Client class doesn't provide this yet */
	    Position mousePosition( const TQPoint& ) const;
		void renderMenuIcons();
		void iconChange();
		virtual void desktopChange( );
		virtual void borders(int&, int&, int&, int&) const;
		virtual TQSize minimumSize() const;

	protected slots:
	    void slotMaximize();
	    void menuButtonPressed();
	    void menuButtonReleased();
	    void toggleShade();

	private:
	    // These are all the icewm button types :)
	    enum Buttons{ BtnSysMenu=0, BtnClose, BtnMaximize, BtnMinimize,
					  BtnHide, BtnRollup, BtnDepth, BtnCount };

		TQString shortenCaption( const TQString* s );
		void calcHiddenButtons();
	    int  titleTextWidth( const TQString& s );
		void addClientButtons( const TQString& s );
		TQSpacerItem* addPixmapSpacer( TQPixmap* p[],
			TQSizePolicy::SizeType = TQSizePolicy::Maximum, int hsize = -1 );

	    IceWMButton* button[ IceWMClient::BtnCount ];
	    TQPixmap*     menuButtonWithIconPix[2];
	    TQSpacerItem* titleSpacerJ;
	    TQSpacerItem* titleSpacerL;
	    TQSpacerItem* titleSpacerS;
	    TQSpacerItem* titleSpacerP;
	    TQSpacerItem* titlebar;
	    TQSpacerItem* titleSpacerM;
	    TQSpacerItem* titleSpacerB;
    	TQSpacerItem* titleSpacerR;
	    TQSpacerItem* titleSpacerQ;
	    TQBoxLayout*  hb;
	    TQGridLayout* grid;
	    bool m_closing;
};

}

#endif

// vim: ts=4
