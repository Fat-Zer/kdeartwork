/***************************************************************************
                          glowclient.h  -  description
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

#ifndef GLOW_CLIENT_H
#define GLOW_CLIENT_H

#include <vector>
#include <map>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QPixmap;
class QBitmap;
class QTimer;
class QBoxLayout;
class QGridLayout;
class QVBoxLayout;
class QSpacerItem;

namespace Glow
{

class GlowButton;
class GlowButtonFactory;

//-----------------------------------------------------------------------------
// GlowTheme
//-----------------------------------------------------------------------------

struct GlowTheme
{
	TQSize buttonSize;
	
	TQString backgroundPixmap;
	TQString backgroundAlphaPixmap;

	TQString stickyOnPixmap;
	TQString stickyOffPixmap;
	TQString maximizeOnPixmap;
	TQString maximizeOffPixmap;
	TQString helpPixmap;
	TQString closePixmap;
	TQString iconifyPixmap;

	TQString stickyOnGlowPixmap;
	TQString stickyOffGlowPixmap;
	TQString maximizeOnGlowPixmap;
	TQString maximizeOffGlowPixmap;
	TQString helpGlowPixmap;
	TQString closeGlowPixmap;
	TQString iconifyGlowPixmap;
};

static GlowTheme default_glow_theme = {
	TQSize (17, 17),
	"background.png", "background_alpha.png",
	"stickyon.png", "stickyoff.png",
	"maximizeon.png", "maximizeoff.png",
	"help.png", "close.png", "iconify.png",
	"stickyon_glow.png", "stickyoff_glow.png",
	"maximizeon_glow.png", "maximizeoff_glow.png",
	"help_glow.png", "close_glow.png", "iconify_glow.png" };

//-----------------------------------------------------------------------------
// GlowClientConfig
//-----------------------------------------------------------------------------

class GlowClientConfig
{
public:
	GlowClientConfig();

	void load (KDecorationFactory *factory);

	TQColor stickyButtonGlowColor;
	TQColor helpButtonGlowColor;
	TQColor iconifyButtonGlowColor;
	TQColor maximizeButtonGlowColor;
	TQColor closeButtonGlowColor;
	bool showResizeHandle;
	int titlebarGradientType;
	TQString themeName;
};

//-----------------------------------------------------------------------------
// GlowClientGlobals
//-----------------------------------------------------------------------------

class GlowClientGlobals : public KDecorationFactory
{
public:
	enum PixmapType { StickyOn, StickyOff, Help, Iconify, MaximizeOn,
		MaximizeOff, Close };

	static GlowClientGlobals *instance();
	
	~GlowClientGlobals();

	virtual KDecoration* createDecoration( KDecorationBridge* b );
	virtual bool reset( unsigned long changed );
    virtual bool supports( Ability ability );
	TQValueList< GlowClientGlobals::BorderSize >  borderSizes() const;
	
	TQString getPixmapName(PixmapType type, bool isActive);

	GlowTheme * theme() const { return _theme; }
	GlowClientConfig * config() const { return _config; }
	GlowButtonFactory * buttonFactory() { return _button_factory; }

private:
	static GlowClientGlobals *m_instance;
	
	GlowTheme * _theme;
	GlowClientConfig * _config;
	GlowButtonFactory * _button_factory;

	GlowClientGlobals();
	void readConfig();
	void readTheme ();
	bool createPixmaps();
	void deletePixmaps();
	bool createPixmap(PixmapType type,bool isActive);
	const TQString getPixmapTypeName(PixmapType type);
};

//-----------------------------------------------------------------------------
// GlowClient
//-----------------------------------------------------------------------------

class GlowClient : public KDecoration
{
	Q_OBJECT
public:
	GlowClient( KDecorationBridge* b, KDecorationFactory* f );
	~GlowClient();
	
	virtual void init();
	virtual void borders( int&, int&, int&, int& ) const;
	virtual void resize( const TQSize& );
	virtual TQSize minimumSize() const;

protected:
	virtual void resizeEvent( TQResizeEvent * );
	virtual void paintEvent( TQPaintEvent * );
	virtual void showEvent( TQShowEvent * );
	virtual void mouseDoubleClickEvent( TQMouseEvent * );
	virtual void wheelEvent( TQWheelEvent * );
	virtual void maximizeChange();
	virtual void activeChange();
	virtual void iconChange();
	virtual void desktopChange();
	virtual void shadeChange();
	virtual void captionChange();
	virtual Position mousePosition(const TQPoint &) const;
	virtual bool eventFilter( TQObject* o, TQEvent* e );

private:
	std::vector<GlowButton*> m_buttonList;
	std::vector<GlowButton*> m_leftButtonList;
	std::vector<GlowButton*> m_rightButtonList;
	GlowButton *m_stickyButton;
	GlowButton *m_helpButton;
	GlowButton *m_minimizeButton;
	GlowButton *m_maximizeButton;
	GlowButton *m_closeButton;
	TQBoxLayout *m_leftButtonLayout;
	TQBoxLayout *m_rightButtonLayout;
	TQSpacerItem * _bottom_spacer;
	TQSpacerItem * _title_spacer;
	TQVBoxLayout * _main_layout;

	void createButtons();
	void updateButtonPositions();
	/**
	 * Before this method is called we have to update the button
	 * positions with updateButtonPositions() because the pixmaps
	 * depend on the position
	 */
	void updateButtonPixmaps();
	void resetLayout();
	void doShape();
	bool isLeft(GlowButton *button);
	bool isRight(GlowButton *button);

protected slots:
	void slotMaximize();
};

} // namespace

#endif
