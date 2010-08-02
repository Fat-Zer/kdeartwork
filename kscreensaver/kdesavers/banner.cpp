//-----------------------------------------------------------------------------
//
// kbanner - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//
// layout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>
// clock function and color cycling added 2000/01/09 by Alexander Neundorf <alexander.neundorf@rz.tu-ilmenau.de>
// 2001/03/04 Converted to use libkscreensaver by Martin R. Jones
// 2002/04/07 Added random vertical position of text,
//		changed horizontal step size to reduce jerkyness,
//		text will return to right margin immediately (and not be drawn half a screen width off-screen)
//		Harald H.-J. Bongartz <harald@bongartz.org>
// 2003/09/06 Converted to use KDialogBase - Nadeem Hasan <nhasan@kde.org>
#include <stdlib.h>

#include <tqlabel.h>
#include <tqlineedit.h>
#include <tqcombobox.h>
#include <tqcheckbox.h>
#include <tqgroupbox.h>
#include <tqslider.h>
#include <tqlayout.h>
#include <tqdatetime.h>
#include <tqfontdatabase.h>
#include <tqpainter.h>

#include <kapplication.h>
#include <krandomsequence.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kcolorbutton.h>
#include <kfontcombo.h>

#include "banner.h"
#include "banner.moc"

// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "kbanner.kss";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "KBanner" );
    KDE_EXPORT const char *kss_version = "2.2.0";

    KDE_EXPORT KScreenSaver *kss_create( WId id )
    {
        return new KBannerSaver( id );
    }

    KDE_EXPORT TQDialog *kss_setup()
    {
        return new KBannerSetup();
    }
}

//-----------------------------------------------------------------------------

KBannerSetup::KBannerSetup( TQWidget *parent, const char *name )
	: KDialogBase( parent, name, true, i18n( "Setup Banner Screen Saver" ),
	  Ok|Cancel|Help, Ok, true ), saver( 0 ), ed(0), speed( 50 )
{
	setButtonText( Help, i18n( "A&bout" ) );
	readSettings();

    TQWidget *main = makeMainWidget();

	TQLabel *label;

	TQVBoxLayout *tl = new TQVBoxLayout(main, 0, spacingHint());
	TQHBoxLayout *tl1 = new TQHBoxLayout( 0, 0, spacingHint() );
	tl->addLayout(tl1);
	TQVBoxLayout *tl11 = new TQVBoxLayout( 0, 0, spacingHint() );
	tl1->addLayout(tl11);

	TQGroupBox *group = new TQGroupBox( 0, Vertical, i18n("Font"), main );
	TQGridLayout *gl = new TQGridLayout(group->layout(), 6, 2, spacingHint() );

	label = new TQLabel( i18n("Family:"), group );
	gl->addWidget(label, 1, 0);

	KFontCombo* comboFonts = new KFontCombo( TQFontDatabase().families(), group );
	comboFonts->setCurrentFont( fontFamily );
	gl->addWidget(comboFonts, 1, 1);
	connect( comboFonts, TQT_SIGNAL( activated( const TQString& ) ),
			TQT_SLOT( slotFamily( const TQString& ) ) );

	label = new TQLabel( i18n("Size:"), group );
	gl->addWidget(label, 2, 0);

	comboSizes = new TQComboBox( TRUE, group );
        fillFontSizes();
	gl->addWidget(comboSizes, 2, 1);
	connect( comboSizes, TQT_SIGNAL( activated( int ) ), TQT_SLOT( slotSize( int ) ) );
	connect( comboSizes, TQT_SIGNAL( textChanged( const TQString & ) ),
			TQT_SLOT( slotSizeEdit( const TQString &  ) ) );

	TQCheckBox *cb = new TQCheckBox( i18n("Bold"),
				       group );
	cb->setChecked( bold );
	connect( cb, TQT_SIGNAL( toggled( bool ) ), TQT_SLOT( slotBold( bool ) ) );
	gl->addWidget(cb, 3, 0);

	cb = new TQCheckBox( i18n("Italic"), group );
	cb->setChecked( italic );
	gl->addWidget(cb, 3, 1);
	connect( cb, TQT_SIGNAL( toggled( bool ) ), TQT_SLOT( slotItalic( bool ) ) );

	label = new TQLabel( i18n("Color:"), group );
	gl->addWidget(label, 4, 0);

	colorPush = new KColorButton( fontColor, group );
	gl->addWidget(colorPush, 4, 1);
	connect( colorPush, TQT_SIGNAL( changed(const TQColor &) ),
		 TQT_SLOT( slotColor(const TQColor &) ) );

   TQCheckBox *cyclingColorCb=new TQCheckBox(i18n("Cycling color"),group);
   cyclingColorCb->setMinimumSize(cyclingColorCb->sizeHint());
   gl->addMultiCellWidget(cyclingColorCb,5,5,0,1);
   connect(cyclingColorCb,TQT_SIGNAL(toggled(bool)),this,TQT_SLOT(slotCyclingColor(bool)));
   cyclingColorCb->setChecked(cyclingColor);

	preview = new TQWidget( main );
	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
	preview->show();    // otherwise saver does not get correct size
	saver = new KBannerSaver( preview->winId() );
	tl1->addWidget(preview);

	tl11->addWidget(group);

	label = new TQLabel( i18n("Speed:"), main );
	tl11->addStretch(1);
	tl11->addWidget(label);

	TQSlider *sb = new TQSlider(0, 100, 10, speed, TQSlider::Horizontal, main );
	sb->setMinimumWidth( 180);
	sb->setFixedHeight(20);
    sb->setTickmarks(TQSlider::Below);
    sb->setTickInterval(10);
	tl11->addWidget(sb);
	connect( sb, TQT_SIGNAL( valueChanged( int ) ), TQT_SLOT( slotSpeed( int ) ) );

	TQHBoxLayout *tl2 = new TQHBoxLayout;
	tl->addLayout(tl2);

	label = new TQLabel( i18n("Message:"), main );
	tl2->addWidget(label);

	ed = new TQLineEdit( main );
	tl2->addWidget(ed);
	ed->setText( message );
	connect( ed, TQT_SIGNAL( textChanged( const TQString & ) ),
			TQT_SLOT( slotMessage( const TQString &  ) ) );

   TQCheckBox *timeCb=new TQCheckBox( i18n("Show current time"), main);
   timeCb->setFixedSize(timeCb->sizeHint());
   tl->addWidget(timeCb,0,Qt::AlignLeft);
   connect(timeCb,TQT_SIGNAL(toggled(bool)),this,TQT_SLOT(slotTimeToggled(bool)));
   timeCb->setChecked(showTime);

   tl->addStretch();
}

// read settings from config file
void KBannerSetup::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

   speed=config->readNumEntry("Speed",50);
/*	if ( speed > 100 )
		speed = 100;
	else if ( speed < 50 )
		speed = 50;*/

   message=config->readEntry("Message","KDE");
   showTime=config->readBoolEntry("ShowTime",FALSE);
   fontFamily=config->readEntry("FontFamily",(TQApplication::font()).family());
   fontSize=config->readNumEntry("FontSize",48);
   fontColor.setNamedColor(config->readEntry("FontColor","red"));
   cyclingColor=config->readBoolEntry("CyclingColor",FALSE);
   bold=config->readBoolEntry("FontBold",FALSE);
   italic=config->readBoolEntry("FontItalic",FALSE);
}

void KBannerSetup::fillFontSizes()
{
    bool block = comboSizes->signalsBlocked();
    comboSizes->blockSignals( true );
    comboSizes->clear();
    int i = 0;
    sizes = TQFontDatabase().pointSizes( fontFamily );
    sizes << 96 << 128 << 156 << 0;
    int current = 0;
    while ( sizes[i] )
	{
	TQString num;
	num.setNum( sizes[i] );
	comboSizes->insertItem( num, i );
	if ( fontSize == sizes[i] ) // fontsize equals one of the defined ones
	    {
            current = i;
	    comboSizes->setCurrentItem( current );
	    slotSize( current );
	    }
	i++;
	}
    if ( current == 0 ) // fontsize seems to be entered by hand
        {
	TQString fsize;
	fsize.setNum( fontSize );
	comboSizes->setEditText(fsize);
	slotSizeEdit( fsize );
	}
    comboSizes->blockSignals( block );
}

void KBannerSetup::slotFamily( const TQString& fam )
{
	fontFamily = fam;
        fillFontSizes(); // different font, different sizes
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSize( int indx )
{
	fontSize = sizes[indx];
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSizeEdit( const TQString& fs )
{
	bool ok;
        fontSize = fs.toInt( &ok, 10 );
	if ( ok )
		if ( saver )
			saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotColor( const TQColor &col )
{
    fontColor = col;
    if ( saver )
	saver->setColor(fontColor);
}

void KBannerSetup::slotCyclingColor(bool on)
{
   colorPush->setEnabled(!on);
   cyclingColor=on;

   if ( saver )
   {
      saver->setCyclingColor( on );
      if ( !on )
         saver->setColor( fontColor );
   }
}

void KBannerSetup::slotBold( bool state )
{
	bold = state;
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotItalic( bool state )
{
	italic = state;
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSpeed( int num )
{
	speed = num;
	if ( saver )
		saver->setSpeed( speed );
}

void KBannerSetup::slotMessage( const TQString &msg )
{
	message = msg;
	if ( saver )
		saver->setMessage( message );
}

void KBannerSetup::slotTimeToggled( bool on )
{
   ed->setEnabled(!on);
   showTime=on;
   if (saver)
   {
      if (showTime)
         saver->setTimeDisplay();
      else
      {
         message=ed->text();
         saver->setMessage(message);
      };
   };
}

// Ok pressed - save settings and exit
void KBannerSetup::slotOk()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	config->writeEntry( "Speed", speed );
	config->writeEntry( "Message", message );
	config->writeEntry( "ShowTime", showTime );
	config->writeEntry( "FontFamily", fontFamily );

	TQString fsize;
	if (fontSize == 0) // an non-number was entered in the font size combo
	{
		fontSize = 48;
	}
	fsize.setNum( fontSize );
	config->writeEntry( "FontSize", fsize );

	TQString colName;
	colName.sprintf( "#%02x%02x%02x", fontColor.red(), fontColor.green(),
		fontColor.blue() );
	config->writeEntry( "FontColor", colName );
	config->writeEntry( "CyclingColor", cyclingColor );
	config->writeEntry( "FontBold", bold );
	config->writeEntry( "FontItalic", italic );

	config->sync();

	accept();
}

void KBannerSetup::slotHelp()
{
	KMessageBox::about(this,
			     i18n("Banner Version 2.2.1\n\nWritten by Martin R. Jones 1996\nmjones@kde.org\nExtended by Alexander Neundorf 2000\nalexander.neundorf@rz.tu-ilmenau.de\n"));
}

//-----------------------------------------------------------------------------

KBannerSaver::KBannerSaver( WId id ) : KScreenSaver( id )
{
	krnd = new KRandomSequence();
	readSettings();
	initialize();
	colorContext = TQColor::enterAllocContext();
	needBlank = TRUE;
	timer.start( speed );
	connect( &timer, TQT_SIGNAL( timeout() ), TQT_SLOT( slotTimeout() ) );
}

KBannerSaver::~KBannerSaver()
{
	timer.stop();
	TQColor::leaveAllocContext();
	TQColor::destroyAllocContext( colorContext );
	delete krnd;
}

void KBannerSaver::setSpeed( int spd )
{
	timer.stop();
	int inv = 100 - spd;
	speed = 1 + ((inv * inv) / 100);
	timer.start( speed );
}

void KBannerSaver::setFont( const TQString& family, int size, const TQColor &color,
		bool b, bool i )
{
	fontFamily = family;
	fontSize = size;
	fontColor = color;
	bold = b;
	italic = i;

	initialize();
}

void KBannerSaver::setColor(TQColor &color)
{
    fontColor = color;
    cyclingColor = FALSE;
    needUpdate = TRUE;
}

void KBannerSaver::setCyclingColor( bool on )
{
    cyclingColor = on;
    needUpdate = TRUE;
}

void KBannerSaver::setMessage( const TQString &msg )
{
    showTime = FALSE;
    message = msg;
    pixmapSize = TQSize();
    needBlank = TRUE;
}

void KBannerSaver::setTimeDisplay()
{
    showTime = TRUE;
    pixmapSize = TQSize();
    needBlank = TRUE;
}

// read settings from config file
void KBannerSaver::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

   setSpeed( config->readNumEntry("Speed",50) );

   message=config->readEntry("Message","KDE");

   showTime=config->readBoolEntry("ShowTime",FALSE);

   fontFamily=config->readEntry("FontFamily",(TQApplication::font()).family());

   fontSize=config->readNumEntry("FontSize",48);

   fontColor.setNamedColor(config->readEntry("FontColor","red"));

   cyclingColor=config->readBoolEntry("CyclingColor",FALSE);

   bold=config->readBoolEntry("FontBold",FALSE);
   italic=config->readBoolEntry("FontItalic",FALSE);

    if ( cyclingColor )
    {
        currentHue=0;
        fontColor.setHsv(0,SATURATION,VALUE);
    }
}

// initialize font
void KBannerSaver::initialize()
{
	fsize = fontSize * height() / TQApplication::desktop()->height();

	font = TQFont( fontFamily, fsize, bold ? TQFont::Bold : TQFont::Normal, italic );

	pixmapSize = TQSize();
	needBlank = TRUE;

	xpos = width();
	ypos = fsize + (int) ((double)(height()-fsize)*krnd->getDouble());
	step = 2 * width() / TQApplication::desktop()->width(); // 6 -> 2 -hhjb-
	if ( step == 0 )
		step = 1;
}

// erase old text and draw in new position
void KBannerSaver::slotTimeout()
{
    if (cyclingColor)
    {
        int hueStep = speed/10;
        currentHue=(currentHue+hueStep)%360;
        fontColor.setHsv(currentHue,SATURATION,VALUE);
    }
    if (showTime)
    {
        TQString new_message = KGlobal::locale()->formatTime(TQTime::currentTime(), true);
	if( new_message != message )
	    needUpdate = TRUE;
	message = new_message;
    }
    if ( !pixmapSize.isValid() || cyclingColor || needUpdate || needBlank )
    {
        TQRect rect = TQFontMetrics( font ).boundingRect( message );
	rect.setWidth( rect.width() + step );
	if ( rect.width() > pixmapSize.width() )
	    pixmapSize.setWidth( rect.width() );
	if ( rect.height() > pixmapSize.height() )
	    pixmapSize.setHeight( rect.height() );
	pixmap = TQPixmap( pixmapSize );
	pixmap.fill( black );
	TQPainter p( &pixmap );
	p.setFont( font );
	p.setPen( fontColor );
	p.drawText( -rect.x(), -rect.y(), message );
	needUpdate = FALSE;
    }
    xpos -= step;
    if ( xpos < -pixmapSize.width() ) {
	TQPainter p( this );
	p.fillRect( xpos + step, ypos, pixmapSize.width(), pixmapSize.height(), black );
	xpos = width();
	ypos = fsize + (int) ((double)(height()-2.0*fsize)*krnd->getDouble());
    }

    if ( needBlank )
    {
	setBackgroundColor( black );
	erase();
	needBlank = FALSE;
    }
    bitBlt( this, xpos, ypos, &pixmap, 0, 0, pixmapSize.width(), pixmapSize.height());
}
