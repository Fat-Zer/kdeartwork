//-----------------------------------------------------------------------------
//
// kbanner - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//

#ifndef __BANNER_H__
#define __BANNER_H__

#include <tqtimer.h>

#include <kscreensaver.h>
#include <kdialogbase.h>

#define SATURATION 150
#define VALUE 255

class TQLineEdit;
class KColorButton;
class KRandomSequence;

class KBannerSaver : public KScreenSaver
{
    Q_OBJECT
  TQ_OBJECT
public:
    KBannerSaver( WId id );
    virtual ~KBannerSaver();

    void setSpeed( int spd );
    void setFont( const TQString &family, int size, const TQColor &color,
		    bool b, bool i );
    void setMessage( const TQString &msg );
    void setTimeDisplay();
    void setCyclingColor(bool on);
    void setColor( TQColor &color);

private:
    void readSettings();
    void initialize();

protected slots:
    void slotTimeout();

protected:
    TQFont   font;
    TQTimer	timer;
    TQString	fontFamily;
    int		fontSize;
    bool	bold;
    bool	italic;
    TQColor	fontColor;
    bool	cyclingColor;
    int		currentHue;
    bool	needUpdate;
    bool	needBlank;
    TQString	message;
    bool	showTime;
    int		xpos, ypos, step, fsize;
    KRandomSequence *krnd;
    int		speed;
    int		colorContext;
    TQPixmap	pixmap;
    TQSize	pixmapSize;
};


class KBannerSetup : public KDialogBase
{
    Q_OBJECT
  TQ_OBJECT
public:
    KBannerSetup( TQWidget *parent = NULL, const char *name = NULL );

protected:
    void readSettings();
    void fillFontSizes();

private slots:
    void slotFamily( const TQString & );
    void slotSize( int );
    void slotSizeEdit(const TQString &);
    void slotColor(const TQColor &);
    void slotCyclingColor(bool on);
    void slotBold( bool );
    void slotItalic( bool );
    void slotSpeed( int );
    void slotMessage( const TQString & );
    void slotOk();
    void slotHelp();
    void slotTimeToggled(bool on);

private:
    TQWidget *preview;
    KColorButton *colorPush;
    KBannerSaver *saver;
    TQLineEdit *ed;
    TQComboBox* comboSizes;

    TQString message;
    bool    showTime;
    TQString fontFamily;
    int	    fontSize;
    TQColor  fontColor;
    bool    cyclingColor;
    bool    bold;
    bool    italic;
    int	    speed;
    TQValueList<int> sizes;
};

#endif

