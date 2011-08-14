// $Id$
#include "config.h"
#include <kapplication.h>
#include <kglobal.h>
#include <tqwhatsthis.h>
#include <tqvbox.h>
#include <klocale.h>

extern "C" KDE_EXPORT TQObject* allocate_config( KConfig* conf, TQWidget* parent )
{
	return new CdeConfig(conf, parent);
}


/* NOTE: 
 * 'conf' 	is a pointer to the kwindecoration modules open kwin config,
 *			and is by default set to the "Style" group.
 *
 * 'parent'	is the parent of the TQObject, which is a VBox inside the
 *			Configure tab in kwindecoration
 */

CdeConfig::CdeConfig( KConfig* conf, TQWidget* parent )
	: TQObject( parent )
{
	cdeConfig = new KConfig("kwincderc");
	KGlobal::locale()->insertCatalogue("kwin_art_clients");
	
	groupBox = new TQVBox( parent );
	
	bgAlign = new TQButtonGroup( 3, Qt::Horizontal, i18n("Text &Alignment"), groupBox );
	bgAlign->setExclusive( true );
	TQWhatsThis::add( bgAlign, i18n("Use these buttons to set the tqalignment of the titlebar caption text.") );
	new TQRadioButton( i18n("Left"), bgAlign, "AlignLeft" );
	TQRadioButton *radio2 = new TQRadioButton( i18n("Centered"), bgAlign, "AlignHCenter" );
	radio2->setChecked( true );
	new TQRadioButton( i18n("Right"), bgAlign, "AlignRight" );
	
	cbColorBorder = new TQCheckBox( i18n("Draw window frames using &titlebar colors"), groupBox );
	TQWhatsThis::add( cbColorBorder, i18n("When selected, the window decoration borders "
					     "are drawn using the titlebar colors. Otherwise, they are "
					     "drawn using normal border colors instead.") );
	
//	cbTitlebarButton = new TQCheckBox( i18n("Titlebar acts like a &pushbutton when clicked"), groupBox );
//	TQWhatsThis::add( cbTitlebarButton, i18n("When selected, this option causes the window titlebar to behave "
//						"as if it was a pushbutton when you click it to move the window.") );
	
	(void) new TQLabel( i18n("Tip: If you want the look of the original Motif(tm) Window Manager,\n"
				"click the \"Buttons\" tab above and remove the help\n"
				"and close buttons from the titlebar."), groupBox );
	
	// Load configuration options
	load( conf );

	// Ensure we track user changes properly
	connect( cbColorBorder, TQT_SIGNAL(clicked()), TQT_SLOT(slotSelectionChanged()) );
//	connect( cbTitlebarButton, TQT_SIGNAL(clicked()), TQT_SLOT(slotSelectionChanged()) );
	connect( bgAlign, TQT_SIGNAL(clicked(int)), TQT_SLOT(slotSelectionChanged(int)) );

	// Make the widgets visible in kwindecoration
	groupBox->show();
}


CdeConfig::~CdeConfig()
{
	delete bgAlign;
	delete groupBox;
	delete cdeConfig;
}


void CdeConfig::slotSelectionChanged()
{
	emit changed();
}

void CdeConfig::slotSelectionChanged( int )
{
	emit changed();
}

// Loads the configurable options from the kwinrc config file
// It is passed the open config from kwindecoration to improve efficiency
void CdeConfig::load( KConfig* /*conf*/ )
{
	cdeConfig->setGroup("General");

	TQString value = cdeConfig->readEntry( "TextAlignment", "AlignHCenter" );
	TQRadioButton *button = (TQRadioButton*)bgAlign->child( (const char *)value.latin1() );
	if ( button )
	    button->setChecked( true );

	bool coloredFrame = cdeConfig->readBoolEntry( "UseTitleBarBorderColors", true );
	cbColorBorder->setChecked( coloredFrame );

//	bool titlebarButton = cdeConfig->readBoolEntry( "TitlebarButtonMode", true );
//	cbTitlebarButton->setChecked( titlebarButton );
}


// Saves the configurable options to the kwinrc config file
void CdeConfig::save( KConfig* /*conf*/ )
{
	cdeConfig->setGroup("General");

	TQRadioButton *button = (TQRadioButton*)bgAlign->selected();
	if ( button )
	    cdeConfig->writeEntry( "TextAlignment", TQString(button->name()) );

	cdeConfig->writeEntry( "UseTitleBarBorderColors", cbColorBorder->isChecked() );
//	cdeConfig->writeEntry( "TitlebarButtonMode", cbTitlebarButton->isChecked() );
	
	// Ensure others trying to read this config get updated
	cdeConfig->sync();
}


// Sets UI widget defaults which must correspond to style defaults
void CdeConfig::defaults()
{
	TQRadioButton *button = (TQRadioButton*)bgAlign->child( "AlignHCenter" );
	if ( button )
	    button->setChecked( true );

	cbColorBorder->setChecked( true );
//	cbTitlebarButton->setChecked( true );
}

#include "config.moc"
// vim: ts=4
