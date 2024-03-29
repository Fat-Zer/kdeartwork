/*
 * Copyright (C) 2001 Stefan Schimanski <1Stein@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <tqwidget.h>
#include <tqdialog.h>
#include <tqtimer.h>
#include <tqstring.h>
#include <tqvaluelist.h>
#include <tqpushbutton.h>
#include <tqradiobutton.h>
#include <tqspinbox.h>
#include <tqcheckbox.h>
#include <tqlabel.h>

#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kparts/part.h>
#include <ktrader.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <kmimetype.h>

#include <kscreensaver.h>

#include "configwidget.h"
#include "kpartsaver.h"
#include <kiconloader.h>


TQPtrList<KPartSaver> g_savers;
bool g_inited = false;


extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "kpartsaver";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "KPart Screen Saver" );
    KDE_EXPORT const char *kss_version = "1.0";

    KDE_EXPORT KScreenSaver *kss_create( WId d )
    {
        KGlobal::locale()->insertCatalogue("kpartsaver");
        return new KPartSaver( d );
    }

    KDE_EXPORT TQDialog *kss_setup()
    {
        kdDebug() << "kss_setup" << endl;
        KGlobal::locale()->insertCatalogue("kpartsaver");
        return new SaverConfig;
    }
}


void exitHandler( int )
{
    kdDebug() << "exitHandler" << endl;
    g_savers.clear();
    KLibLoader::self()->cleanUp();
    exit(0);
}


KPartSaver::KPartSaver( WId id )
    : KScreenSaver( id ), m_timer(), m_part(0), m_current(-1), m_back(0)
{
    // install signal handlers to make sure that nspluginviewer is shutdown correctly
    // move this into the nspluginviewer kpart code
    if( !g_inited ) {
        g_inited = true;
        g_savers.setAutoDelete( true );

        srand( time(0) );

        // install signal handler
        signal( SIGINT, exitHandler );    // Ctrl-C will cause a clean exit...
        signal( SIGTERM, exitHandler );   // "kill"...
        signal( SIGHUP, exitHandler );    // "kill -HUP" (hangup)...
        signal( SIGKILL, exitHandler );    // "kill -KILL"
        //atexit( ( void (*)(void) ) exitHandler );
    }

    g_savers.append( this );

    closeURL();

    // load config
    KConfig *cfg = kapp->config();
    cfg->setGroup( "Misc" );

    m_single = cfg->readBoolEntry( "Single", true );
    m_delay = cfg->readNumEntry( "Delay", 60 );
    m_random = cfg->readBoolEntry( "Random", false );
    m_files = cfg->readListEntry( "Files" );

    if( m_files.count()==0 ) {

        // create background widget
        m_back = new TQLabel( i18n("The screen saver is not configured yet."), this );

        m_back->tqsetAlignment( AlignCenter );
        embed( m_back );
        m_back->show();

    } else {

        // queue files
        for( unsigned int n=0; n<m_files.count(); n++ )
            queue( KURL::fromPathOrURL( m_files[n] ) );

        // play files
        if( m_single )
            next( m_random );
        else {
            next( m_random );
            m_timer = new TQTimer( this );
            m_timer->start( m_delay*1000, true );
            connect( m_timer, TQT_SIGNAL(timeout()), TQT_SLOT(timeout()) );
        }
    }
}


KPartSaver::~KPartSaver()
{
    g_savers.take( g_savers.findRef( this ));
    closeURL();
}


void KPartSaver::closeURL()
{
    if( m_part ) {
        m_part->closeURL();
        delete m_part;
        m_part = 0;
    }
}


bool KPartSaver::openURL( KURL url )
{
    closeURL();

    // find mime type
    TQString mime = KMimeType::findByURL( url )->name();

    // find fitting kparts
    KTrader::OfferList offers;
    offers = KTrader::self()->query( mime, "'KParts/ReadOnlyPart' in ServiceTypes" );
    if( offers.count()==0 ) {
        kdDebug() << "Can't find proper kpart for " << mime << endl;
        return false;
    }

    // load kpart library
    TQString lib = offers.first()->library();
    KLibFactory *factory = KLibLoader::self()->factory( lib.latin1() );
    if( !factory ) {
        kdDebug() << "Library " << lib << " not found." << endl;
        return false;
    }

    // create kpart
    m_part = (KParts::ReadOnlyPart *)factory->create( TQT_TQOBJECT(this), "kpart", "KParts::ReadOnlyPart" );
    if( !m_part ) {
        kdDebug() << "Part for " << url.url() << " can't be constructed" << endl;
        return false;
    } else
        embed( m_part->widget() );

    // show kpart
    delete m_back;
    m_back = 0;

    show();
    m_part->widget()->show();

    // load url
    if( !m_part->openURL( url ) ) {
        kdDebug() << "Can't load " << url.url() << endl;
        closeURL();
        return false;
    }



    return true;
}


void KPartSaver::queue( KURL url )
{
    Medium medium;
    medium.url = url;
    medium.failed = false;
    m_media.append( medium );
}


void KPartSaver::timeout()
{
    next( m_random );
    m_timer->start( m_delay*1000, true );
}


void KPartSaver::next( bool random )
{
    // try to find working media
    while( m_media.count()>0 ) {

        if( random )
            m_current = rand() % m_media.count();
        else
            m_current++;

        if( m_current>=(int)m_media.count() )
            m_current = 0;

        kdDebug() << "Trying medium " << m_media[m_current].url.url() << endl;

        // either start immediately or start mimejob first
        if( !openURL( m_media[m_current].url ) ) {
            m_media.remove( m_media.at(m_current) );
            m_current--;
        } else
            return;

    }

    // create background widget
    m_back = new TQLabel( i18n("All of your files are unsupported"), this );

    m_back->tqsetAlignment( AlignCenter );
    embed( m_back );
    m_back->show();

    // nothing found, set to invalid
    m_current = -1;
}


/*******************************************************************************/


SaverConfig::SaverConfig( TQWidget* parent, const char* name )
    : ConfigWidget( parent, name, true )
{
    connect( m_ok, TQT_SIGNAL(clicked()), TQT_SLOT(apply()) );
    connect( m_ok, TQT_SIGNAL(clicked()), TQT_SLOT(accept()) );
    connect( m_cancel, TQT_SIGNAL(clicked()), TQT_SLOT(reject()) );

    connect( m_multiple, TQT_SIGNAL(toggled(bool)), m_delayLabel, TQT_SLOT(setEnabled(bool)) );
    connect( m_multiple, TQT_SIGNAL(toggled(bool)), m_delay, TQT_SLOT(setEnabled(bool)) );
    connect( m_multiple, TQT_SIGNAL(toggled(bool)), m_secondsLabel, TQT_SLOT(setEnabled(bool)) );
    connect( m_multiple, TQT_SIGNAL(toggled(bool)), m_random, TQT_SLOT(setEnabled(bool)) );

    connect( m_files, TQT_SIGNAL(selectionChanged()), TQT_SLOT(select()) );
    connect( m_add, TQT_SIGNAL(clicked()), TQT_SLOT(add()) );
    connect( m_remove, TQT_SIGNAL(clicked()), TQT_SLOT(remove()) );
    connect( m_up, TQT_SIGNAL(clicked()), TQT_SLOT(up()) );
    connect( m_down, TQT_SIGNAL(clicked()), TQT_SLOT(down()) );

    m_up->setIconSet( SmallIconSet("up") );
    m_down->setIconSet( SmallIconSet("down") );

    // load config
    KConfig *cfg = kapp->config();
    cfg->setGroup( "Misc" );

    bool single = cfg->readBoolEntry( "Single", true );
    m_single->setChecked( single );
    m_multiple->setChecked( !single );
    m_delay->setMinValue( 1 );
    m_delay->setMaxValue( 10000 );
    m_delay->setValue( cfg->readNumEntry( "Delay", 60 ) );
    m_random->setChecked( cfg->readBoolEntry( "Random", false ) );
    m_files->insertStringList( cfg->readListEntry( "Files" ) );

    // update buttons
    select();
}


SaverConfig::~SaverConfig()
{
}


void SaverConfig::apply()
{
    kdDebug() << "apply" << endl;

    KConfig *cfg = kapp->config();
    cfg->setGroup( "Misc" );

    cfg->writeEntry( "Single", m_single->isChecked() );
    cfg->writeEntry( "Delay", m_delay->value() );
    cfg->writeEntry( "Random", m_random->isChecked() );

    int num = m_files->count();
    TQStringList files;
    for( int n=0; n<num; n++ )
        files << m_files->text(n);

    cfg->writeEntry( "Files", files );

    cfg->sync();
}


void SaverConfig::add()
{
    KURL::List files = KFileDialog::getOpenURLs( TQString(), TQString(),
                                                 this, i18n("Select Media Files") );
    for( unsigned int n=0; n<files.count(); n++ )
        m_files->insertItem( files[n].prettyURL(), -1 );
}


void SaverConfig::remove()
{
    int current = m_files->currentItem();
    if( current!=-1 )
        m_files->removeItem( current );
}


void SaverConfig::select()
{
    bool enabled = m_files->currentItem()!=-1;
    m_remove->setEnabled( enabled );
    m_up->setEnabled( enabled && m_files->currentItem()!=0 );
    m_down->setEnabled( enabled && m_files->currentItem()!=(int)m_files->count()-1 );
}


void SaverConfig::up()
{
    int current = m_files->currentItem();
    if ( current>0 ) {
        TQString txt = m_files->currentText();
        m_files->removeItem( current );
        m_files->insertItem( txt, current-1 );
        m_files->setCurrentItem( current-1 );
    }
}


void SaverConfig::down()
{
    int current = m_files->currentItem();
    if ( current!=-1 && current<(int)m_files->count()-1 ) {
        TQString txt = m_files->currentText();
        m_files->removeItem( current );
        m_files->insertItem( txt, current+1 );
        m_files->setCurrentItem( current+1 );
    }
}

#include "kpartsaver.moc"
