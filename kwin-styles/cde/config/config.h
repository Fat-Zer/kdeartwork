#ifndef __KDE_CDECONFIG_H
#define __KDE_CDECONFIG_H

#include <tqcheckbox.h>
#include <tqgroupbox.h>
#include <tqbuttongroup.h>
#include <tqlabel.h>
#include <tqradiobutton.h>
#include <tqhbox.h>
#include <kconfig.h>

class QCheckBox;
class QGroupBox;
class QVBox;
class QLabel;
class QRadioButton;

class CdeConfig: public QObject
{
	Q_OBJECT

	public:
		CdeConfig( KConfig* conf, TQWidget* parent );
		~CdeConfig();

	// These public signals/slots work similar to KCM modules
	signals:
		void changed();

	public slots:
		void load( KConfig* conf );	
		void save( KConfig* conf );
		void defaults();

	protected slots:
		void slotSelectionChanged();	// Internal use
		void slotSelectionChanged( int );
		
	private:
		KConfig*   	cdeConfig;
		TQCheckBox* 	cbColorBorder;
//		TQCheckBox* 	cbTitlebarButton;
		TQHBox* 	        groupBox;
		TQGroupBox* 	gbSlider;
		TQButtonGroup*	bgAlign;
};


#endif

// vim: ts=4
