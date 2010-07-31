/*
 *	$Id$
 *
 *	This file contains the IceWM configuration widget
 *
 *	Copyright (c) 2001
 *		Karol Szwed <gallium@kde.org>
 *		http://gallium.n3.net/
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef _ICEWMCONFIG_H
#define _ICEWMCONFIG_H

#include <tqwidget.h>
#include <tqcheckbox.h>
#include <tqgroupbox.h>
#include <tqlistbox.h>
#include <tqlabel.h>
#include <kurllabel.h>
#include <kconfig.h>

class QVBox;

class IceWMConfig: public QObject
{
	Q_OBJECT

	public:
		IceWMConfig( KConfig* conf, TQWidget* parent );
		~IceWMConfig();

	// These public signals/slots work similar to KCM modules
	signals:
		void changed();

	public slots:
		void load( KConfig* conf );
		void save( KConfig* conf );
		void defaults();

	protected slots:
		void slotSelectionChanged();	// Internal use
		void callURL( const TQString& s );
		void findIceWMThemes();

	private:
		KConfig*   icewmConfig;
		TQCheckBox* cbThemeTitleTextColors;
		TQCheckBox* cbTitleBarOnTop;
		TQCheckBox* cbShowMenuButtonIcon;
		TQListBox*  themeListBox;
		TQLabel*	   themeLabel;
		KURLLabel* urlLabel;
		TQString    localThemeString;
		TQVBox*     mainWidget;
};


#endif
// vim: ts=4
