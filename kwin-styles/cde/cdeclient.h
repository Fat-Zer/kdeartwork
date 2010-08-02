/*
 *	CDE KWin client - emulates the look and feel
 *	of dtwm, the CDE window manager.
 *
 *	Copyright (c) 2000-2001, 2002
 *		Chris Lee       <lee@azsites.com>
 *		Lennart Kudling <kudling@kde.org>
 *      	Fredrik Höglund <fredrik@kde.org>
 *
 *	Copyright (c) 2003
 *		Luciano Montanaro <mikelima@cirulla.net>
 *
 *	Originally based on the KStep client.
 *
 *	Distributed under the terms of the BSD license.
 */

#ifndef __CDECLIENT_H
#define __CDECLIENT_H

#include <tqbutton.h>
#include <tqbitmap.h>
#include <kpixmap.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class TQLabel;
class TQBoxLayout;
class TQVBoxLayout;
class TQSpacerItem;

namespace CDE {

class CdeClient;

enum Buttons { BtnMenu=0, BtnHelp, BtnIconify, BtnMax, BtnClose, BtnCount };

class CdeButton : public QButton
{
public:
    CdeButton( CdeClient* parent=0, const char* name=0, int btnType=0,
               const TQString& tip=NULL, int realize_btns = LeftButton );
    void reset();
    ButtonState lastButton() { return last_button; }

protected:
    void mousePressEvent(TQMouseEvent *e);
    void mouseReleaseEvent(TQMouseEvent *e);
    virtual void drawButton(TQPainter *p);

private:
    CdeClient *m_parent;
    int m_btnType;
    int m_realize_buttons;
    ButtonState last_button;
};

class CdeClient : public KDecoration
{
    Q_OBJECT
public:
    CdeClient(KDecorationBridge *b, KDecorationFactory *f);
    ~CdeClient() {};
    void init();

protected:
    bool eventFilter(TQObject *o, TQEvent *e);
    void resizeEvent( TQResizeEvent* );
    void paintEvent( TQPaintEvent* );

    void showEvent(TQShowEvent *);
    void addClientButtons( const TQString& );
    void mouseDoubleClickEvent( TQMouseEvent* );
    void wheelEvent( TQWheelEvent * );
    void captionChange();
    void desktopChange();
    void activeChange();
    void shadeChange();
    void iconChange();
    TQSize minimumSize() const;
    void resize(const TQSize &size);
    void borders(int &left, int &right, int &top, int &bottom) const;
    void mousePressEvent( TQMouseEvent* );
    void mouseReleaseEvent( TQMouseEvent* );
    void maximizeChange();
    Position mousePosition( const TQPoint& p ) const;

protected slots:
    void menuButtonPressed();
    void menuButtonReleased();
    void maximizeButtonClicked();

private:
    CdeButton* button[BtnCount];
    TQVBoxLayout* mainLayout;
    TQBoxLayout*  titleLayout;
    TQSpacerItem* titlebar;
    bool titlebarPressed;
    bool closing;
};

class CdeClientFactory: public TQObject, public KDecorationFactory
{
public:
    CdeClientFactory();
    virtual ~CdeClientFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *);
    virtual bool supports( Ability ability );
    virtual bool reset(unsigned long changed);

    TQValueList< CdeClientFactory::BorderSize > borderSizes() const;

};

}

#endif

