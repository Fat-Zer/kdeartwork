/***************************************************************************
 *   Copyright (C) 2011 by Timothy Pearson <kb9vqf@pearsoncomputing.net>   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <tqobject.h>
#include <tqtimer.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>

#include "krootbacking.h"

bool argb_visual = false;

static const char description[] =
    I18N_NOOP("A program to grab the current TDE desktop backrounds for xscreensaver");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
	KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KAboutData about("krootbacking", I18N_NOOP("krootbacking"), version, description,
			KAboutData::License_GPL, "(C) 2011 Timothy Pearson", 0, 0, "kb9vqf@pearsoncomputing.net");
	about.addAuthor( "Timothy Pearson", 0, "kb9vqf@pearsoncomputing.net" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );
	
	KApplication app;

	// no session.. just start up normally
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	
	/// @todo do something with the command line args here
	args->clear();

	TQObject* mainWin = new KRootBacking();
	TQTimer *timer = new QTimer( mainWin );
        TQObject::connect( timer, SIGNAL(timeout()), mainWin, SLOT(start()) );
        timer->start( 100, TRUE ); // 100ms single shot timer

	app.exec();

	delete timer;
	delete mainWin;
}

