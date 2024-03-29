//-----------------------------------------------------------------------------
//
// kscience - screen saver for KDE
//
// Copyright (c)  Rene Beutler 1998
//

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>

#include <tqpainter.h>
#include <tqpixmap.h>
#include <tqlabel.h>
#include <tqlistbox.h>
#include <tqcheckbox.h>
#include <tqslider.h>
#include <tqlayout.h>

#include <kapplication.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <krandomsequence.h>

#include "science.h"
#include "science.moc"

#if defined Q_WS_X11 && !defined K_WS_TQTONLY
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#define SCI_DEFAULT_MODE          0
#define SCI_DEFAULT_MOVEX         6
#define SCI_DEFAULT_MOVEY         8
#define SCI_DEFAULT_SIZE         15
#define SCI_DEFAULT_INTENSITY     4
#define SCI_DEFAULT_SPEED        70
#define SCI_DEFAULT_INVERSE   false
#define SCI_DEFAULT_GRAVITY   false
#define SCI_DEFAULT_HIDE      false
#define SCI_MAX_SPEED           100
#define SCI_MAX_MOVE             20

#undef Below


// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "kscience.kss";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "Science Screen Saver" );
    KDE_EXPORT const char *kss_version = "2.2.0";

    KDE_EXPORT KScreenSaver *kss_create( WId id )
    {
        return new KScienceSaver( id );
    }

    KDE_EXPORT TQDialog *kss_setup()
    {
        return new KScienceSetup();
    }
}

static struct {
	TQString name;
	bool inverseEnable;
	} modeInfo[MAX_MODES];

enum { MODE_WHIRL=0, MODE_CURVATURE, MODE_SPHERE, MODE_WAVE, MODE_EXPONENTIAL, MODE_CONTRACTION };

void initModeInfo()
{
	modeInfo[MODE_WHIRL].name = i18n( "Whirl" );
	modeInfo[MODE_WHIRL].inverseEnable = true;

	modeInfo[MODE_SPHERE].name = i18n( "Sphere" );
	modeInfo[MODE_SPHERE].inverseEnable = true;

	modeInfo[MODE_EXPONENTIAL].name = i18n( "Exponential" );
	modeInfo[MODE_EXPONENTIAL].inverseEnable = false;

	modeInfo[MODE_CONTRACTION].name = i18n( "Contraction" );
	modeInfo[MODE_CONTRACTION].inverseEnable = false;

	modeInfo[MODE_WAVE].name = i18n( "Wave" );
	modeInfo[MODE_WAVE].inverseEnable = false;

	modeInfo[MODE_CURVATURE].name = i18n( "Curvature" );
	modeInfo[MODE_CURVATURE].inverseEnable = true;
}

//-----------------------------------------------------------------------------
// KPreviewWidget
//

KPreviewWidget::KPreviewWidget( TQWidget *parent ) :
                TQWidget ( parent ) { }

void KPreviewWidget::paintEvent( TQPaintEvent *event )
{
	if( saver != 0 )
		saver->do_refresh( event->rect() );
}

void KPreviewWidget::notifySaver( KScienceSaver *s )
{
	saver = s;
}

//-----------------------------------------------------------------------------
// Screen Saver
//

struct KScienceData
{
    T32bit     **offset;
    XImage     *buffer;
    XImage     *xRootWin;
    GC         gc;
};

KScienceSaver::KScienceSaver( WId id, bool s, bool gP )
    : KScreenSaver( id )
{
    d = new KScienceData;
    d->gc = XCreateGC(qt_xdisplay(), id, 0, 0);
	d->xRootWin = 0;
    d->buffer = 0;

	moveOn = true;
	grabPixmap = gP;
	setup = s;

	vx = vy = 0.0;
	readSettings();

	if( !grabPixmap )
	{
		grabRootWindow();
		initialize();
		do_refresh( TQRect ( 0, 0, width(), height() ) );
	}

	connect( &timer, TQT_SIGNAL( timeout() ), TQT_SLOT( slotTimeout() ) );
	timer.start( SCI_MAX_SPEED - speed[mode] );
}

KScienceSaver::~KScienceSaver()
{
	timer.stop();
	releaseLens();
	if ( d->xRootWin )
		XDestroyImage( d->xRootWin );
    XFreeGC(qt_xdisplay(), d->gc );
    delete d;
}

void KScienceSaver::myAssert( bool term, const char *eMsg )
{
	if( !term ) {
		fprintf(stderr, "Error in KScreensaver - mode Science: %s\n", eMsg);
		releaseLens();
		exit(-1);
	}
}

void KScienceSaver::initialize()
{
	KRandomSequence rnd;
	initLens();
	signed int ws = (signed int) (width() -  diam);
	signed int hs = (signed int) (height() - diam);

	x = (ws > 0) ? (rnd.getDouble() * ws ) : 0.0;
	y = (hs > 0) ? (rnd.getDouble() * hs ) : 0.0;

	xcoord = (int) x;
	ycoord = (int) y;

	switch( bpp ) {
		case 1 : applyLens = &KScienceSaver::applyLens8bpp;  break;
		case 2 : applyLens = &KScienceSaver::applyLens16bpp; break;
		case 3 : applyLens = &KScienceSaver::applyLens24bpp; break;
		case 4 : applyLens = &KScienceSaver::applyLens32bpp; break;
		default: myAssert( false, "unsupported colordepth "\
		                   "(only 8, 16, 24, 32 bpp supported)" );
	}
}

void KScienceSaver::initWhirlLens()
{
	double dx, dy, r, phi, intens;
	T32bit *off;
	T32bit xo, yo;

	intens = double( intensity[mode] + 1) / 5.0;
	if( inverse[mode] )
		intens = -intens;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if ( dx == 0.0 )
				    phi = (dy > 0.0) ? M_PI_2 :-(M_PI_2);
			    else
				    phi = atan2( dy, dx );
			    phi +=  intens * ( radius - r ) / ( r+7.0 );
			    xo = (T32bit) ( origin + r*cos( phi ) - x );
			    yo = (T32bit) ( origin + r*sin( phi ) - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
        }
}

void KScienceSaver::initSphereLens()
{
	double dx, dy, r, xr, yr, phi, intens;
	T32bit *off;
	T32bit xo, yo;

	intens = 1.0 - double( intensity[mode] ) / 20.0;

	if( inverse[mode] )
		intens = -intens;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		if( r < radius )
		{
			xr = (double) radius*cos(asin(dy/radius));
			yr = (double) radius*cos(asin(dx/radius));
			phi = (xr != 0.0) ? asin(dx/xr) : 0.0;
			xo = (T32bit) (origin + intens*2.0*phi*xr / M_PI - x);
			phi = (yr != 0.0) ? asin(dy/yr) : 0.0;
			yo = (T32bit) (origin + intens*2.0*phi*yr / M_PI - y);
			off[x] = xo*bpp + yo*imgnext;
		}
		else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
        }
}

void KScienceSaver::initExponentialLens()
{
	double dx, dy, r, rnew, f, intens;
	T32bit *off;
	T32bit xo, yo;

	if( mode == MODE_EXPONENTIAL )
		intens = - (0.1 + 0.8 * double( intensity[mode] + 2) / 10.0);
	else
		intens = 0.9 - 0.8 * double( intensity[mode] ) / 10.0;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if( r == 0.0 )
				    f = 0.0;
			    else
			    {
				    rnew = radius*(pow(r, intens) /  pow(radius, intens));
				    f = double ((int)rnew % radius) / r;
			    }
			    xo = (T32bit) ( origin + f*dx - x );
			    yo = (T32bit) ( origin + f*dy - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
        }
}

void KScienceSaver::initCurvatureLens()
{
	double dx, dy, r, f, intens;
	T32bit *off;
	T32bit xo, yo;

	intens = (double) radius*intensity[mode] / 20.0;
	if( inverse[mode] ) intens = -intens;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if( r == 0.0 )
				    f = 0.0;
			    else
				    f = (r - intens * sin(M_PI * r/(double)radius)) / r;
			    xo = (T32bit) ( origin + f*dx - x );
			    yo = (T32bit) ( origin + f*dy - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
	}
}

void KScienceSaver::initWaveLens()
{
	double dx, dy, r, rnew, f, intens, k;
	T32bit *off;
	T32bit xo, yo;

	intens = (double) intensity[mode] + 1.0;
	k = (intensity[mode] % 2) ? -12.0 : 12.0;

	for(int y = side-1; y >= 0; y--)
	{
		dy = y - origin;
		off = d->offset[y] = (T32bit *) malloc(sizeof(T32bit) * side);
		myAssert( off != 0, "too few memory" );
		for(int x = side-1; x >= 0; x--)
		{
		    dx = x - origin;
		    r = sqrt( dx*dx + dy*dy );

		    if( r < radius )
		    {
			    if( r == 0.0 )
				    f = 0.0;
			    else
			    {
				    rnew = r - k * sin( M_PI * intens * r/(double)radius);
				    f = double ((int)rnew % radius) / r;
			    }
			    xo = (T32bit) ( origin + f*dx - x );
			    yo = (T32bit) ( origin + f*dy - y );
			    off[x] = xo*bpp + yo*imgnext;
		    }
		    else
			if( hideBG[mode] )
				off[x] = (border-y)*imgnext + (border-x)*bpp;
			else
				off[x] = 0;
		}
	}
}

void KScienceSaver::initLens()
{
	int min = (width() < height()) ? width() : height();
	border = 1 + SCI_MAX_MOVE;

	radius = (size[mode] * min) / 100;
	if( radius<<1 == min ) radius--;
	diam = radius << 1;
	myAssert( diam < min, "assertion violated: diam < min" );
	origin = radius + border;
	side  = origin << 1;

	d->buffer = XSubImage( d->xRootWin, 0, 0, side, side );
        myAssert( d->buffer != 0, "can't allocate pixmap" );

	d->offset = (T32bit **) malloc( sizeof(T32bit *) * side );
	myAssert( d->offset != 0, "too few memory" );

	switch( mode ) {
		case MODE_WHIRL: 	initWhirlLens();  break;
		case MODE_SPHERE: 	initSphereLens(); break;
		case MODE_EXPONENTIAL:
		case MODE_CONTRACTION: 	initExponentialLens(); break;
		case MODE_CURVATURE:    initCurvatureLens(); break;
		case MODE_WAVE: 	initWaveLens(); break;
		default: myAssert( false, "internal error (wrong mode in initLens() )" );
	}
}

void KScienceSaver::releaseLens()
{
	if( d->offset != 0 ) {
		for(int i=0; i<side; i++)
    			if( d->offset[i] != 0 ) free( d->offset[i] );
    		free( d->offset );
		d->offset = 0;
	}
	if( d->buffer != 0 ) {
		XDestroyImage( d->buffer );
		d->buffer = 0;
	}
}

void KScienceSaver::setMode( int m )
{
	timer.stop();

	releaseLens();
	int old = mode;
	mode = m;
	vx = copysign( moveX[mode], vx );
	vy = copysign( moveY[mode], vy );
	int dm = diam;
	initLens();
	if( hideBG[old] ^ hideBG[m] )
		do_refresh( TQRect( 0, 0, width(), height() ) );
	else
		if( diam < dm )
		{
			do_refresh( TQRect( (int) x+diam, (int) y,      dm-diam, diam    ) );
			do_refresh( TQRect( (int) x,      (int) y+diam, dm,      dm-diam ) );
		}

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setMoveX( int s )
{
	timer.stop();

	moveX[mode] = s;
	vx = copysign( moveX[mode], vx );

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setMoveY( int s )
{
	timer.stop();

	moveY[mode] = s;
	vy = copysign( moveY[mode], vy );

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setMove( bool s )
{
	moveOn = s;
}

void KScienceSaver::setSize( int s )
{
	timer.stop();

	releaseLens();
	int dm = diam;
	size[mode] = s;
	initLens();
	if( diam < dm )
	{
		do_refresh( TQRect( (int) x+diam, (int) y,      dm-diam, diam    ) );
		do_refresh( TQRect( (int) x,      (int) y+diam, dm,      dm-diam ) );
	}

	timer.start( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setSpeed( int s )
{
	speed[mode] = s;

	timer.changeInterval( SCI_MAX_SPEED - speed[mode] );
}

void KScienceSaver::setIntensity( int i )
{
	timer.stop();

	releaseLens();
	intensity[mode] = i;
	initLens();

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::setInverse( bool b )
{
	timer.stop();

	releaseLens();
	inverse[mode] = b;
	initLens();

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::setGravity( bool b )
{
	timer.stop();

	releaseLens();
	gravity[mode] = b;
	vy = copysign( moveY[mode], vy );
	initLens();

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::setHideBG( bool b )
{
	timer.stop();

	releaseLens();
	hideBG[mode] = b;
	initLens();
	do_refresh( TQRect( 0, 0, width(), height() ) );

	timer.start( SCI_MAX_SPEED - speed[mode]);
}

void KScienceSaver::readSettings()
{
    KConfig *config = KGlobal::config();
        TQString sMode;

	config->setGroup( "Settings" );
	mode = config->readNumEntry( "ModeNr", SCI_DEFAULT_MODE );

	for(int i=0; i < MAX_MODES; i++)
	{
		sMode.setNum( i );
		config->setGroup( "Mode" + sMode );
		moveX[i]     = config->readNumEntry(  "MoveX",     SCI_DEFAULT_MOVEX);
		moveY[i]     = config->readNumEntry(  "MoveY",     SCI_DEFAULT_MOVEY);
		size[i]      = config->readNumEntry(  "Size",      SCI_DEFAULT_SIZE);
		speed[i]     = config->readNumEntry(  "Speed",     SCI_DEFAULT_SPEED);
		intensity[i] = config->readNumEntry(  "Intensity", SCI_DEFAULT_INTENSITY);
		inverse[i]   = config->readBoolEntry( "Inverse",   SCI_DEFAULT_INVERSE);
		gravity[i]   = config->readBoolEntry( "Gravity",   SCI_DEFAULT_GRAVITY);
		hideBG[i]    = config->readBoolEntry( "HideBG",    SCI_DEFAULT_HIDE);
	}

	vx = copysign( moveX[mode], vx );
	vy = copysign( moveY[mode], vy );
}

void KScienceSaver::do_refresh( const TQRect & rect )
{
	if( grabPixmap )
		return;
	rect.normalize();

	if( hideBG[mode] )
	{
		XSetWindowBackground( qt_xdisplay(), winId(), black.pixel() );
		XClearArea( qt_xdisplay(), winId(), rect.left(), rect.top(),
                            rect.width(), rect.height(), false );
	}
	else
	{
		myAssert( d->xRootWin != 0, "root window not grabbed" );
		XPutImage( qt_xdisplay(), winId(), d->gc, d->xRootWin,
		           rect.left(), rect.top(),
                           rect.left(), rect.top(),
                           rect.width(), rect.height() );
	}
}

void KScienceSaver::slotTimeout()
{
	if( grabPixmap ) {
		if( !TQWidget::find(winId())->isActiveWindow() )
			return;
		grabPreviewWidget();
		grabPixmap = false;
		initialize();
		if( hideBG[mode] )
			do_refresh( TQRect ( 0, 0, width(), height() ) );
	}

	signed int oldx = xcoord, oldy = ycoord;

	if( gravity[mode] ) {
		double h = double(y+1.0) / double(height()-diam);
		if( h > 1.0 ) h = 1.0;
		vy = sqrt( h ) * ( (vy > 0.0) ? moveY[mode] : -moveY[mode] );
	}
	myAssert( abs((int)rint(vy)) <= border, "assertion violated: vy <= border" );

	if( moveOn )
	{
		x += vx;
		y += vy;
	}

	if( x <= 0.0 ) {
		vx = -vx;
		x = 0.0;
	}
	if( int(x) + diam >= width()) {
		vx = -vx;
		myAssert( width()-diam > 0, "assertion violated: width-diam > 0" );
		x = (double) (width() - diam - 1);
	}
	if( y <= 0.0 ) {
		vy = -vy;
		y = 0.0;
	}
	if( int(y) + diam >= height() ) {
		vy = -vy;
		myAssert( height() - diam > 0, "assertion violated: height-diam > 0" );
		y = (double) (height() - diam - 1);
	}

	xcoord = (int) x ;
	ycoord = (int) y ;
	signed int dx = (signed int) xcoord - oldx;
	signed int dy = (signed int) ycoord - oldy;
	signed int xs, ys, xd, yd, w, h;

	if( dx > 0 ) {
		w = diam+dx;
		xd = oldx;
		xs = border-dx;
		if( dy > 0 ) {
			h = diam+dy;
			yd = oldy;
			ys = border-dy;
		}
		else {
			h = diam-dy;
			yd = ycoord;
			ys = border;
		}
	}
	else {
		w = diam-dx;
		xd = xcoord;
		xs = border;
		if( dy > 0 ) {
			h = diam+dy;
			yd = oldy;
			ys = border-dy;
		} else {
			h = diam-dy;
			yd = ycoord;
			ys = border;
		}
	}

	if(  xd + w >= width()  ) w = width()  - xd - 1;
	if(  yd + h >= height() ) h = height() - yd - 1;

//printf("%d: (dx: %3d, dy: %3d), diam: %3d, (xc: %3d, yc: %3d), (xs: %3d, ys: %3d), (xd: %3d, yd: %3d), (w: %3d, h: %3d)\n", mode, dx, dy, diam, xcoord, ycoord, xs, ys, xd, yd, w, h);
	myAssert( dx <= border && dy <=border, "assertion violated: dx or dy <= border");
	myAssert( xcoord >= 0 && ycoord >= 0, "assertion violated: xcoord, ycoord >= 0 ");
	myAssert( xd+w < width(), "assertion violated: xd+w < width" );
	myAssert( yd+h < height(), "assertion violated: yd+h < height" );

	if( hideBG[mode] )
		blackPixel( xcoord, ycoord );
	(this->*applyLens)(xs, ys, xd, yd, w, h);
	XPutImage( qt_xdisplay(), winId(), d->gc, d->buffer, 0, 0, xd, yd, w, h );
	if( hideBG[mode] )
		blackPixelUndo( xcoord, ycoord );
}

void KScienceSaver::grabRootWindow()
{
	Display *dsp = qt_xdisplay();
	Window rootwin = RootWindow( dsp, qt_xscreen() );

	// grab contents of root window
	if( d->xRootWin )
		XDestroyImage( d->xRootWin );

	d->xRootWin = XGetImage( dsp, rootwin, 0, 0, width(),
	                      height(), AllPlanes, ZPixmap);
	myAssert( d->xRootWin, "unable to grab root window\n" );

	imgnext = d->xRootWin->bytes_per_line;
	bpp = ( d->xRootWin->bits_per_pixel ) >> 3;
}

void KScienceSaver::grabPreviewWidget()
{
	myAssert( TQWidget::find(winId())->isActiveWindow(), "can't grab preview widget: dialog not active()" );

	if( d->xRootWin )
		XDestroyImage( d->xRootWin );

	Display *dsp = qt_xdisplay();
	d->xRootWin = XGetImage( dsp, winId(), 0, 0, width(), height(), AllPlanes, ZPixmap);
	myAssert( d->xRootWin, "unable to grab preview window\n" );

	imgnext = d->xRootWin->bytes_per_line;
	bpp = ( d->xRootWin->bits_per_pixel ) >> 3;
}

void KScienceSaver::blackPixel( int x, int y )
{
	unsigned char black = (char) BlackPixel( qt_xdisplay(), qt_xscreen() );
	unsigned int adr = x*bpp + y*imgnext;

	for(int i=0; i<bpp; i++) {
		blackRestore[i] = d->xRootWin->data[adr];
		d->xRootWin->data[adr++] = black;
	}
}

void KScienceSaver::blackPixelUndo( int x, int y )
{
	unsigned int adr = x*bpp + y*imgnext;
	for(int i=0; i<bpp; i++)
		d->xRootWin->data[adr++] = blackRestore[i];
}

// hm....

void KScienceSaver::applyLens8bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	signed int ix, iy, datanext = d->buffer->bytes_per_line - w;

	img1 = d->xRootWin->data + xd + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--)
			*data++ = img2++[*off++];
		img1 += imgnext;
		data += datanext;
	}

}

void KScienceSaver::applyLens16bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	int ix, iy, datanext = d->buffer->bytes_per_line - (w << 1);

	img1 = d->xRootWin->data + (xd << 1) + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--) {
			*data++ = img2++[*off];
			*data++ = img2++[*off++];
		}
		img1 += imgnext;
		data += datanext;
	}
}

void KScienceSaver::applyLens24bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	signed int ix, iy, datanext = d->buffer->bytes_per_line - 3*w;

	img1 = d->xRootWin->data + 3*xd + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--) {
			*data++ = img2++[*off];
			*data++ = img2++[*off];
			*data++ = img2++[*off++];
		}
		img1 += imgnext;
		data += datanext;
	}
}

void KScienceSaver::applyLens32bpp(int xs, int ys, int xd, int yd, int w, int h)
{
	T32bit *off;
	char *img1, *img2, *data;
	signed int ix, iy, datanext = d->buffer->bytes_per_line - (w << 2);

	img1 = d->xRootWin->data + (xd << 2) + yd*imgnext;
	data = d->buffer->data;
	for(iy = ys; iy < ys+h; iy++)
	{
		off = d->offset[iy] + xs;
		img2 = img1;
		for(ix = w; ix > 0; ix--) {
			*data++ = img2++[*off];
			*data++ = img2++[*off];
			*data++ = img2++[*off];
			*data++ = img2++[*off++];
		}
		img1 += imgnext;
		data += datanext;
	}
}


//-----------------------------------------------------------------------------

KScienceSetup::KScienceSetup(  TQWidget *parent, const char *name )
	: KDialogBase( parent, name, true, i18n( "Setup Science Screen Saver" ),
	  Ok|Cancel|Help, Ok, true ), saver( 0 )
{
	readSettings();
	initModeInfo();

	TQWidget *main = makeMainWidget();

	TQHBoxLayout *lt  = new TQHBoxLayout( main, 0, spacingHint());
	TQVBoxLayout *ltm = new TQVBoxLayout;
	lt->addLayout( ltm );
	TQVBoxLayout *ltc = new TQVBoxLayout;
	lt->addLayout( ltc );

	// mode
	TQLabel *label = new TQLabel( i18n("Mode:"), main );
	ltm->addWidget( label );

	TQListBox *c = new TQListBox( main );
	for(int i = 0; i<MAX_MODES; i++)
		c->insertItem( modeInfo[i].name );
	c->setCurrentItem( mode );
	c->setFixedHeight( 5 * c->fontMetrics().height() );
	connect( c, TQT_SIGNAL( highlighted( int ) ), TQT_SLOT( slotMode( int ) ) );
	ltm->addWidget( c );

	// inverse
	TQCheckBox *cbox = checkInverse = new TQCheckBox( i18n("Inverse"), main );
	cbox->setEnabled( modeInfo[mode].inverseEnable );
	cbox->setChecked( inverse[mode] );
	connect( cbox, TQT_SIGNAL( clicked() ), TQT_SLOT( slotInverse() ) );
	ltm->addWidget( cbox );

	// gravity
	cbox = checkGravity = new TQCheckBox( i18n("Gravity"), main );
	cbox->setChecked( gravity[mode] );
	connect( cbox, TQT_SIGNAL( clicked() ), TQT_SLOT( slotGravity() ) );
	ltm->addWidget( cbox );

	// hide background
	cbox = checkHideBG = new TQCheckBox( i18n("Hide background"), main );
	cbox->setChecked( hideBG[mode] );
	connect( cbox, TQT_SIGNAL( clicked() ), TQT_SLOT( slotHideBG() ) );
	ltm->addWidget( cbox );
	ltm->addStretch();

	// size
	label = new TQLabel( i18n("Size:"), main );
	ltc->addWidget( label );

	slideSize = new TQSlider(9, 50, 5, size[mode], Qt::Horizontal,
                                main );
	slideSize->setMinimumSize( 90, 20 );
    slideSize->setTickmarks(TQSlider::Below);
    slideSize->setTickInterval(5);
	connect( slideSize, TQT_SIGNAL( sliderMoved( int ) ),
		TQT_SLOT( slotSize( int ) ) );
	connect( slideSize, TQT_SIGNAL( sliderPressed() ),
		TQT_SLOT( slotSliderPressed() ) );
	connect( slideSize, TQT_SIGNAL( sliderReleased() ),
		TQT_SLOT( slotSliderReleased() ) );

	ltc->addWidget( slideSize );

	// intensity
	label = new TQLabel( i18n("Intensity:"), main );
	ltc->addWidget( label );

	slideIntensity = new TQSlider(0, 10, 1, intensity[mode],
                                     Qt::Horizontal, main );
	slideIntensity->setMinimumSize( 90, 20 );
    slideIntensity->setTickmarks(TQSlider::Below);
    slideIntensity->setTickInterval(1);
	connect( slideIntensity, TQT_SIGNAL( sliderMoved( int ) ),
		TQT_SLOT( slotIntensity( int )) );
	connect( slideIntensity, TQT_SIGNAL( sliderPressed() ),
		TQT_SLOT( slotSliderPressed() ) );
	connect( slideIntensity, TQT_SIGNAL( sliderReleased() ),
		TQT_SLOT( slotSliderReleased() ) );
	ltc->addWidget( slideIntensity );

	// speed
	label = new TQLabel( i18n("Speed:"), main );
	ltc->addWidget( label );

	slideSpeed = new TQSlider(0, SCI_MAX_SPEED, 10, speed[mode],
                             Qt::Horizontal, main );
	slideSpeed->setMinimumSize( 90, 20 );
    slideSpeed->setTickmarks(TQSlider::Below);
    slideSpeed->setTickInterval(10);
	connect( slideSpeed, TQT_SIGNAL( sliderMoved( int ) ),
		TQT_SLOT( slotSpeed( int ) ) );
	ltc->addWidget( slideSpeed );

	// motion
	label = new TQLabel( i18n("Motion:"), main );
	ltc->addWidget( label );

	TQHBoxLayout *ltcm = new TQHBoxLayout;
	ltc->addLayout( ltcm );

	slideMoveX = new TQSlider(0, SCI_MAX_MOVE, 5, moveX[mode],
                                 Qt::Horizontal, main );
	slideMoveX->setMinimumSize( 40, 20 );
    slideMoveX->setTickmarks(TQSlider::Below);
    slideMoveX->setTickInterval(5);
	connect( slideMoveX, TQT_SIGNAL( sliderMoved( int ) ),
		TQT_SLOT( slotMoveX( int ) ) );
	ltcm->addWidget( slideMoveX );

	slideMoveY = new TQSlider(0, SCI_MAX_MOVE, 5, moveY[mode],
                                Qt::Horizontal, main );
	slideMoveY->setMinimumSize( 40, 20 );
    slideMoveY->setTickmarks(TQSlider::Below);
    slideMoveY->setTickInterval(5);
	connect( slideMoveY, TQT_SIGNAL( sliderMoved( int ) ),
		TQT_SLOT( slotMoveY( int ) ) );
	ltcm->addWidget( slideMoveY );

	ltc->addStretch();

	// preview
	preview = new KPreviewWidget( main );
	preview->setFixedSize( 220, 170 );
	TQPixmap p( locate("data", "kscreensaver/pics/kscience.png") );
	if( p.isNull() )
		preview->setBackgroundColor( black );
	else
		preview->setBackgroundPixmap( p );
	preview->show();	// otherwise saver does not get correct size
	lt->addWidget( preview );

	// let the preview window display before creating the saver
	kapp->processEvents();

	saver = new KScienceSaver( preview->winId(), true, !p.isNull() );
	preview->notifySaver( saver );
}

KScienceSetup::~KScienceSetup()
{
	delete saver;		// be sure to delete this first
}

void KScienceSetup::updateSettings()
{
	// update dialog
	slideMoveX    ->setValue(   moveX[mode]     );
	slideMoveY    ->setValue(   moveY[mode]     );
	slideSize     ->setValue(   size[mode]      );
	slideSpeed    ->setValue(   speed[mode]     );
	slideIntensity->setValue(   intensity[mode] );
	checkInverse  ->setEnabled( modeInfo[mode].inverseEnable );
	checkInverse  ->setChecked( inverse[mode]   );
	checkGravity  ->setChecked( gravity[mode]   );
	checkHideBG   ->setChecked( hideBG[mode]    );
}

// read settings from config file
void KScienceSetup::readSettings()
{
    KConfig *config = KGlobal::config();
        TQString sMode;

	config->setGroup( "Settings" );
	mode = config->readNumEntry( "ModeNr", SCI_DEFAULT_MODE );

	for(int i=0; i < MAX_MODES; i++)
	{
		sMode.setNum( i );
		config->setGroup( "Mode" + sMode );
		moveX[i]     = config->readNumEntry(  "MoveX",     SCI_DEFAULT_MOVEX);
		moveY[i]     = config->readNumEntry(  "MoveY",     SCI_DEFAULT_MOVEY);
		size[i]      = config->readNumEntry(  "Size",      SCI_DEFAULT_SIZE);
		speed[i]     = config->readNumEntry(  "Speed",     SCI_DEFAULT_SPEED);
		intensity[i] = config->readNumEntry(  "Intensity", SCI_DEFAULT_INTENSITY);
		inverse[i]   = config->readBoolEntry( "Inverse",   SCI_DEFAULT_INVERSE);
		gravity[i]   = config->readBoolEntry( "Gravity",   SCI_DEFAULT_GRAVITY);
		hideBG[i]    = config->readBoolEntry( "HideBG",    SCI_DEFAULT_HIDE);
	}
}

void KScienceSetup::slotMode( int m )
{
	mode = m;

	if( saver )
		saver->setMode( mode );

	updateSettings();
}

void KScienceSetup::slotInverse( )
{
	inverse[mode] = checkInverse->isChecked();

	if( saver )
		saver->setInverse( inverse[mode] );
}

void KScienceSetup::slotGravity( )
{
	gravity[mode] = checkGravity->isChecked();

	if( saver )
		saver->setGravity( gravity[mode] );
}

void KScienceSetup::slotHideBG( )
{
	hideBG[mode] = checkHideBG->isChecked();

	if( saver )
		saver->setHideBG( hideBG[mode] );
}

void KScienceSetup::slotMoveX( int x )
{
	moveX[mode] = x;

	if( saver )
		saver->setMoveX( x );
}

void KScienceSetup::slotMoveY( int y )
{
	moveY[mode] = y;

	if( saver )
		saver->setMoveY( y );
}

void KScienceSetup::slotSize( int s )
{
	size[mode] = s;

	if( saver )
		saver->setSize( s );
}

void KScienceSetup::slotSpeed( int s )
{
	speed[mode] = s;

	if( saver )
		saver->setSpeed( s );
}

void KScienceSetup::slotIntensity( int i )
{
	intensity[mode] = i;

	if( saver )
		saver->setIntensity( i );
}

void KScienceSetup::slotSliderPressed()
{
	if( saver )
		saver->setMove( false );
}

void KScienceSetup::slotSliderReleased()
{
	if( saver )
		saver->setMove( true );
}

// Ok pressed - save settings and exit
void KScienceSetup::slotOk()
{
    KConfig *config = KGlobal::config();
	TQString sSize, sSpeed, sIntensity, sMode;

	config->setGroup( "Settings" );
	config->writeEntry( "ModeNr", mode );

	for(int i=0; i<MAX_MODES; i++)
	{
		sMode.setNum( i );
		config->setGroup( "Mode" + sMode );
		config->writeEntry( "MoveX",     moveX[i]     );
		config->writeEntry( "MoveY",     moveY[i]     );
		config->writeEntry( "Size",      size[i]      );
		config->writeEntry( "Speed",     speed[i]     );
		config->writeEntry( "Intensity", intensity[i] );
		config->writeEntry( "Inverse",   inverse[i]   );
		config->writeEntry( "Gravity",   gravity[i]   );
		config->writeEntry( "HideBG",    hideBG[i]    );
	}

	config->sync();

	accept();
}

void KScienceSetup::slotHelp()
{
	TQString about = i18n("Science Version 0.26.5\n\nWritten by Rene Beutler (1998)\nrbeutler@g26.ethz.ch");
	KMessageBox::about(this,
	                      about);
}
