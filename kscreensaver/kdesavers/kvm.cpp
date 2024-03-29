/*-
 * kvm.cpp - The Vm screensaver for KDE
 * Copyright (c) 2000 by Artur Rataj
 * This file is distributed under the terms of the GNU General Public License
 *
 * This file is partially based on kmatrix screen saver -- original copyright follows:
 * kmatrix.c - The Matrix screensaver for KDE
 * by Eric Plante Copyright (c) 1999
 * Distributed under the Gnu Public License
 *
 * Much of this code taken from xmatrix.c from xscreensaver;
 * original copyright follows:
 * xscreensaver, Copyright (c) 1999 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 */
// tqlayout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

/* for AIX at least */
#include <time.h>

#include <tqcolor.h>
#include <tqlabel.h>
#include <tqlayout.h>
#include <tqslider.h>
#include <tqpainter.h>
#include <tqbitmap.h>

#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>

#ifdef DEBUG_MEM
#include <mcheck.h>
#endif

#include "kvm.h"

#include "vm.xpm"
#include "vm.xbm"

#define CHAR_HEIGHT 22


// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "kvm.kss";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "Virtual Machine" );
    KDE_EXPORT const char *kss_version = "2.2.0";

    KDE_EXPORT KScreenSaver *kss_create( WId id )
    {
        return new kVmSaver( id );
    }

    KDE_EXPORT TQDialog *kss_setup()
    {
        return new kVmSetup();
    }
}



static void
load_images (m_state *state)
{
  if ( TQPixmap::defaultDepth() > 1 )
    {
      state->images = TQPixmap( vm );
    }
  else
    {
      state->images = TQBitmap( vm_width, vm_height, vm_bits );
    }
  state->image_width = state->images.width();
  state->image_height = state->images.height();
  state->nglyphs = state->image_height / CHAR_HEIGHT;
}


static m_state *
init_pool ( TQWidget *w )
{
  m_state *state = new m_state;
  state->w = w;

  load_images (state);

  state->char_width = state->image_width / 4;
  state->char_height = CHAR_HEIGHT;

  state->grid_width  = w->width()  / state->char_width;
  state->grid_height = w->height() / state->char_height;
  state->grid_margin_x = w->width()%state->char_width/2;
  state->grid_margin_y = w->height()%state->char_height/2;
  state->show_threads = 1;
  vm_init_pool( &(state->pool), state->grid_width*state->grid_height,
                THREAD_MAX_STACK_SIZE, MAX_THREADS_NUM );
   //vm_enable_reverse( state->pool, 1 );
   state->modified = new char[state->grid_height*state->grid_width];
   for( int x = 0; x < state->grid_width*state->grid_height; ++x )
    state->modified[x] = 1;
  return state;
}

static void
draw_pool (m_state *state)
{
  int x, y;
  struct tvm_process*	curr_thread;

  if( state->show_threads ) {
   curr_thread = state->pool->processes;
   while( curr_thread ) {
    state->modified[curr_thread->position] = 2;
    curr_thread = curr_thread->next;
   }
  }
  for (y = 0; y < state->grid_height; y++)
    for (x = 0; x < state->grid_width; x++) {
     int index = state->grid_width * y + x;
     if( state->modified[index] )
      {
        int op = state->pool->area[index];
        int pos_y;
        int pos_x = 0;
        switch( op ) {
         case VM_OP_STOP:
          pos_y = 14;
          break;

         case VM_OP_EXEC:
          pos_y = 15;
          break;

         case VM_OP_COPY:
          pos_y = 12;
          break;

         default:
          pos_y = op - VM_OP_PUSH;
          if( pos_y < 0 ) {
           pos_y = -pos_y;
           pos_x = 1;
          }
          break;
        }
        if( state->show_threads )
         if( state->modified[index] == 1 )
          pos_x += 2;
          TQPainter p(state->w);
          p.setPen( TQt::green );
          p.setBrush( TQt::black );
          p.drawPixmap( state->grid_margin_x + x*state->char_width,
                         state->grid_margin_y + y*state->char_height,
                         state->images, pos_x*state->char_width,
                         pos_y*state->char_height,
                         state->char_width, state->char_height );
       --state->modified[index];
      }
    }
}

//-----------------------------------------------------------------------------

kVmSaver::kVmSaver( WId id ) : KScreenSaver( id )
{
	readSettings();

    colorContext = TQColor::enterAllocContext();

	blank();
        setSpeed( speed );
        setRefreshTimeout( refreshTimeout );

        refreshStep = 0;

        pool_state = init_pool( this );
        vm_default_initstate( time(0), &(pool_state->pool->vm_random_data) );
	connect( &timer, TQT_SIGNAL( timeout() ), TQT_SLOT( slotTimeout() ) );
        timer.start( 100 - speed );
}

kVmSaver::~kVmSaver()
{
	timer.stop();
        vm_done_pool( pool_state->pool );
        delete[] pool_state->modified;
	TQColor::leaveAllocContext();
	TQColor::destroyAllocContext( colorContext );
}

void kVmSaver::blank()
{
  setBackgroundColor( black );
  erase();
}

void kVmSaver::setSpeed( int spd )
{
	speed = spd;
//	timer.changeInterval( (100 - speed)*(100 - speed)*(100 - speed)/10000 );
	timer.changeInterval( (100 - speed) );
}
void kVmSaver::setRefreshTimeout( const int refreshTimeout )
{
 this->refreshTimeout = refreshTimeout;
}

void kVmSaver::readSettings()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	speed = config->readNumEntry( "Speed", 50 );
	refreshTimeout = config->readNumEntry( "DisplayRefreshTimeout", 0 );
}
int kVmSaver::getRandom( const int max_value ) {
 return (int)( vm_random(&(pool_state->pool->vm_random_data))*1.0*(max_value + 1.0)/
               (VM_RAND_MAX + 1.0) );
// return (int)( rand()*1.0*(max_value + 1.0)/
//               (RAND_MAX + 1.0) );
}
void kVmSaver::modifyArea( const int op ) {
 int position;

 vm_modify( pool_state->pool, position =
            getRandom(pool_state->pool->area_size - 1), op );
 pool_state->modified[position] = 1;
}

void kVmSaver::slotTimeout()
{
 for( int i = 0; i < 1; ++i ) {
  if( getRandom(2) == 0 )
   modifyArea( VM_OP_PUSH + getRandom(11) - getRandom(11) );
  if( getRandom(8) == 0 )
   modifyArea( VM_OP_STOP );
  if( getRandom(8) == 0 )
   modifyArea( VM_OP_COPY );
  if( getRandom(8) == 0 )
   modifyArea( VM_OP_EXEC );
//  if( getRandom(5) == 0 )
//   modifyArea( VM_OP_WAIT );
 }
 if( getRandom(0) == 0 )
  vm_exec( pool_state->pool, getRandom(pool_state->pool->area_size - 1), 0,
           vm_get_reverse( pool_state->pool ) );
 vm_iterate( pool_state->pool, pool_state->modified );
// if( refreshStep++ >= refreshTimeout*refreshTimeout*refreshTimeout ) {
 if( refreshStep++ >= refreshTimeout ) {
  draw_pool( pool_state );
  refreshStep = 0;
 }
}

//-----------------------------------------------------------------------------

kVmSetup::kVmSetup( TQWidget *parent, const char *name )
	: KDialogBase( parent, name, true, i18n( "Setup Virtual Machine" ),
	  Ok|Cancel|Help, Ok, true )
{
	readSettings();

	setButtonText( Help, i18n( "A&bout" ) );
	TQWidget *main = makeMainWidget();

	TQHBoxLayout *tl = new TQHBoxLayout( main, 0, spacingHint() );
	TQVBoxLayout *tl1 = new TQVBoxLayout();
	tl->addLayout(tl1);

	TQLabel *label = new TQLabel( i18n("Virtual machine speed:"), main );
	tl1->addWidget(label);

	TQSlider *slider = new TQSlider( Qt::Horizontal, main );
	slider->setMinimumSize( 120, 20 );
	slider->setRange( 0, 100 );
	slider->setSteps( 10, 20 );
	slider->setTickmarks( TQSlider::Below );
	slider->setTickInterval( 10 );
	slider->setValue( speed );
	connect( slider, TQT_SIGNAL( valueChanged( int ) ),
		 TQT_SLOT( slotSpeed( int ) ) );
	tl1->addWidget(slider);

	label = new TQLabel( i18n("Display update speed:"), main );
	tl1->addWidget(label);

	slider = new TQSlider( Qt::Horizontal, main );
	slider->setMinimumSize( 120, 20 );
	slider->setRange( 0, MAX_REFRESH_TIMEOUT );
	slider->setSteps( MAX_REFRESH_TIMEOUT/10, MAX_REFRESH_TIMEOUT/5 );
	slider->setTickmarks( TQSlider::Below );
	slider->setTickInterval( MAX_REFRESH_TIMEOUT/10 );
	slider->setValue( MAX_REFRESH_TIMEOUT - refreshTimeout );
	connect( slider, TQT_SIGNAL( valueChanged( int ) ),
		 TQT_SLOT( slotRefreshTimeout( int ) ) );
	tl1->addWidget(slider);
	tl1->addStretch();

	preview = new TQWidget( main );
	preview->setFixedSize( 220, 165 );
	preview->show();    // otherwise saver does not get correct size
	saver = new kVmSaver( preview->winId() );
	tl->addWidget(preview);
}

kVmSetup::~kVmSetup()
{
    delete saver;
}

void kVmSetup::readSettings()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	speed = config->readNumEntry( "Speed", 50 );
	if ( speed > 100 )
		speed = 100;
	else if ( speed < 0 )
		speed = 0;
	refreshTimeout = config->readNumEntry( "DisplayRefreshTimeout", 0 );
	if ( refreshTimeout > MAX_REFRESH_TIMEOUT )
		refreshTimeout = MAX_REFRESH_TIMEOUT;
	else if ( refreshTimeout < 0 )
		refreshTimeout = 0;
}

void kVmSetup::slotSpeed( int num )
{
	speed = num;
	if ( saver )
		saver->setSpeed( num );
}
void kVmSetup::slotRefreshTimeout( int num )
{
	refreshTimeout = MAX_REFRESH_TIMEOUT - num;
	if ( saver )
		saver->setRefreshTimeout( refreshTimeout );
}

void kVmSetup::slotOk()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	TQString sspeed;
	sspeed.setNum( speed );
	config->writeEntry( "Speed", sspeed );
	sspeed.setNum( refreshTimeout );
	config->writeEntry( "DisplayRefreshTimeout", sspeed );

	config->sync();
	accept();
}

void kVmSetup::slotHelp()
{
	KMessageBox::about(this,
		i18n("Virtual Machine Version 0.1\n\nCopyright (c) 2000 Artur Rataj <art@zeus.polsl.gliwice.pl>\n"),
	        i18n("About Virtual Machine")
	);
}

#include "kvm.moc"

