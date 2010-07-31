/*****************************************************************
kwin - the KDE window manager
								
Copyright (C) 1999, 2000 Matthias Ettrich <ettrich@kde.org>
******************************************************************/
#ifndef STDCLIENT_H
#define STDCLIENT_H
#include <tqlayout.h>
#include <tqvariant.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <tqtoolbutton.h>
class QLabel;
class QSpacerItem;

namespace KDE1 {

enum ButtonType {
    ButtonMenu=0,
    ButtonSticky,
    ButtonMinimize,
    ButtonMaximize,
    ButtonClose,
    ButtonHelp,
    ButtonTypeCount
};

class StdClient : public KDecoration
{
    Q_OBJECT
public:
    StdClient( KDecorationBridge* b, KDecorationFactory* f );
    ~StdClient();
    void init();
    TQSize minimumSize() const;
    void borders( int& left, int& right, int& top, int& bottom ) const;
    void reset( unsigned long mask );
    void resize( const TQSize& s );
    void shadeChange() {};
    Position mousePosition( const TQPoint& p ) const { return KDecoration::mousePosition( p ); }
protected:
    bool eventFilter( TQObject* o, TQEvent* e );
    void resizeEvent( TQResizeEvent* );
    void paintEvent( TQPaintEvent* );

    void mouseDoubleClickEvent( TQMouseEvent * );
    void wheelEvent( TQWheelEvent * );
    void captionChange();
    void iconChange();
    void maximizeChange();
    void desktopChange();
    void activeChange();

private:
    void addButtons(TQBoxLayout* hb, const TQString& buttons);

private slots:
    void menuButtonPressed();
    void maxButtonClicked( ButtonState );

private:
    TQToolButton* button[ButtonTypeCount];
    TQSpacerItem* titlebar;
};

class StdToolClient : public KDecoration
{
    Q_OBJECT
public:
    StdToolClient( KDecorationBridge* b, KDecorationFactory* f );
    ~StdToolClient();
    void init();
    TQSize minimumSize() const;
    void borders( int& left, int& right, int& top, int& bottom ) const;
    void reset( unsigned long mask );
    void resize( const TQSize& s );
    void shadeChange() {};
    void activeChange() {};
    void iconChange() {};
    void maximizeChange() {};
    void desktopChange() {};
    Position mousePosition( const TQPoint& p ) const { return KDecoration::mousePosition( p ); }
protected:
    bool eventFilter( TQObject* o, TQEvent* e );
    void resizeEvent( TQResizeEvent* );
    void paintEvent( TQPaintEvent* );

    void mouseDoubleClickEvent( TQMouseEvent * );
    void wheelEvent( TQWheelEvent * );
    void captionChange();

private:
    TQToolButton* closeBtn;
    TQSpacerItem* titlebar;
};



/*
  Like TQToolButton, but provides a clicked(ButtonState) signals that
  has the last pressed mouse button as argument
 */
class ThreeButtonButton: public QToolButton
{
    Q_OBJECT
public:
  ThreeButtonButton ( TQWidget *parent = 0, const char* name = 0 )
      : TQToolButton( parent, name )
    {
	connect( this, TQT_SIGNAL( clicked() ), this, TQT_SLOT( handleClicked() ) );
        setCursor( arrowCursor );
    }
    ~ThreeButtonButton () {}

signals:
    void clicked( ButtonState );

protected:
    void mousePressEvent( TQMouseEvent* e )
    {
	last_button = e->button();
	TQMouseEvent me ( e->type(), e->pos(), e->globalPos(), LeftButton, e->state() );
	TQToolButton::mousePressEvent( &me );
    }

    void mouseReleaseEvent( TQMouseEvent* e )
    {
	TQMouseEvent me ( e->type(), e->pos(), e->globalPos(), LeftButton, e->state() );
	TQToolButton::mouseReleaseEvent( &me );
    }

private slots:
    void handleClicked()
    {
	emit clicked( last_button );
    }

private:
    ButtonState last_button;

};

class StdFactory : public KDecorationFactory
{
public:
    StdFactory();
    ~StdFactory();
    KDecoration* createDecoration( KDecorationBridge* b );
    bool reset( unsigned long mask );
    virtual bool supports( Ability ability );
};

}

#endif
