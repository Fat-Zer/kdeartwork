//-----------------------------------------------------------------------------
//
// klines 0.1.1 - Basic screen saver for KDE
// by Dirk Staneker 1997
// based on kpolygon from Martin R. Jones 1996
// mailto:dirk.staneker@student.uni-tuebingen.de
//
// tqlayout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>
// 2001/03/04 Converted to libkscreensaver by Martin R. Jones

#include <config.h>
#include <stdlib.h>
#include <time.h>
#include <tqcolor.h>
#include <tqlabel.h>
#include <tqslider.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kcolorbutton.h>

#include "kcolordialog.h"
#include "lines.h"
#include "lines.moc"

#include <tqlayout.h>
#include <klocale.h>
#include <kglobal.h>
#include <tqpainter.h>

#define MAXLENGTH	256

// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "klines.kss";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "KLines" );
    KDE_EXPORT const char *kss_version = "2.2.0";

    KDE_EXPORT KScreenSaver *kss_create( WId id )
    {
        return new kLinesSaver( id );
    }

    KDE_EXPORT TQDialog *kss_setup()
    {
        return new kLinesSetup();
    }
}

// Methods of the Lines-class
Lines::Lines(int x){
	uint i;
	numLn=x;
	offx1=12;
	offy1=16;
	offx2=9;
	offy2=10;
	start=new Ln;
	end=start;
	for(i=1; i<numLn; i++){
		end->next=new Ln;
		end=end->next;
	}
	end->next=start;
	akt=start;
}

Lines::~Lines(){
	uint i;
	for(i=0; i<numLn; i++){
		end=start->next;
		delete start;
		start=end;
	}
}

inline void Lines::reset(){	akt=start;	}

inline void Lines::getKoord(int& a, int& b, int& c, int& d){
	a=akt->x1; b=akt->y1;
	c=akt->x2; d=akt->y2;
	akt=akt->next;
}

inline void Lines::setKoord(const int& a, const int& b, const int& c, const int& d){
	akt->x1=a; akt->y1=b;
	akt->x2=c; akt->y2=d;
}

inline void Lines::next(void){ akt=akt->next; }

void Lines::turn(const int& w, const int& h){
	start->x1=end->x1+offx1;
	start->y1=end->y1+offy1;
	start->x2=end->x2+offx2;
	start->y2=end->y2+offy2;
	if(start->x1>=w) offx1=-8;
	if(start->x1<=0) offx1=7;
	if(start->y1>=h) offy1=-11;
	if(start->y1<=0) offy1=13;
	if(start->x2>=w) offx2=-17;
	if(start->x2<=0) offx2=15;
	if(start->y2>=h) offy2=-10;
	if(start->y2<=0) offy2=13;
	end->next=start;
	start=start->next;
	end=end->next;
}


//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
kLinesSetup::kLinesSetup(TQWidget *parent, const char *name)
	: KDialogBase(parent, name, true, i18n( "Setup Lines Screen Saver" ),
	  Ok|Cancel|Help, Ok, true ), saver( 0 ), length( 10 ), speed( 50 )
{
	readSettings();

	setButtonText( Help, i18n( "A&bout" ) );
	TQWidget *main = makeMainWidget();

	TQHBoxLayout *tl = new TQHBoxLayout(main, 0, spacingHint());
	TQVBoxLayout *tl1 = new TQVBoxLayout;
	tl->addLayout(tl1);

	TQLabel *label=new TQLabel(i18n("Length:"), main);
	tl1->addWidget(label);

	TQSlider *sb= new TQSlider(1, MAXLENGTH+1, 16, length, Qt::Horizontal,
		main);
	sb->setMinimumSize(120, 20);
	sb->setTickmarks(TQSlider::Below);
	sb->setTickInterval(32);
	connect(sb, TQT_SIGNAL(valueChanged(int)), TQT_SLOT(slotLength(int)));
	tl1->addWidget(sb);

	label=new TQLabel(i18n("Speed:"), main);
	tl1->addWidget(label);

	sb = new TQSlider(0, 100, 10, speed, Qt::Horizontal, main);
	sb->setMinimumSize(120, 20);
	sb->setTickmarks(TQSlider::Below);
	sb->setTickInterval(10);
	connect( sb, TQT_SIGNAL( valueChanged( int ) ), TQT_SLOT( slotSpeed( int ) ) );
	tl1->addWidget(sb);

	label=new TQLabel(i18n("Beginning:"), main);
	tl1->addWidget(label);

	colorPush0=new KColorButton(colstart, main);
	connect(colorPush0, TQT_SIGNAL(changed(const TQColor &)),
		TQT_SLOT(slotColstart(const TQColor &)));
	tl1->addWidget(colorPush0);

	label=new TQLabel(i18n("Middle:"), main);
	tl1->addWidget(label);

	colorPush1=new KColorButton(colmid, main);
	connect(colorPush1, TQT_SIGNAL(changed(const TQColor &)),
		TQT_SLOT(slotColmid(const TQColor &)));
	tl1->addWidget(colorPush1);

	label=new TQLabel(i18n("End:"), main);
	tl1->addWidget(label);

	colorPush2=new KColorButton(colend, main);
	connect(colorPush2, TQT_SIGNAL(changed(const TQColor &)),
		TQT_SLOT(slotColend(const TQColor &)));
	tl1->addWidget(colorPush2);
	tl1->addStretch();

	preview = new TQWidget( main );
	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
	preview->show();    // otherwise saver does not get correct size
	saver=new kLinesSaver(preview->winId());
	tl->addWidget(preview);
}

kLinesSetup::~kLinesSetup()
{
    delete saver;
}

// read settings from config file
void kLinesSetup::readSettings(){
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

    TQString str;

    length = config->readNumEntry("Length", length);
    if(length>MAXLENGTH) length=MAXLENGTH;
    else if(length<1) length=1;

    speed = config->readNumEntry("Speed", speed);
    if(speed>100) speed=100;
    else if(speed<50) speed=50;

    str=config->readEntry("StartColor");
    if(!str.isNull()) colstart.setNamedColor(str);
    else colstart=white;
    str=config->readEntry("MidColor");
    if(!str.isNull()) colmid.setNamedColor(str);
    else colmid=blue;
    str=config->readEntry("EndColor");
    if(!str.isNull()) colend.setNamedColor(str);
    else colend=black;
}

void kLinesSetup::slotLength(int len){
	length=len;
	if(saver) saver->setLines(length);
}

void kLinesSetup::slotSpeed(int num){
	speed=num;
	if(saver) saver->setSpeed(speed);
}

void kLinesSetup::slotColstart(const TQColor &col){
    colstart = col;
    if(saver) saver->setColor(colstart, colmid, colend);
}

void kLinesSetup::slotColmid(const TQColor &col){
    colmid = col;
    if(saver) saver->setColor(colstart, colmid, colend);
}

void kLinesSetup::slotColend(const TQColor &col){
    colend = col;
    if(saver) saver->setColor(colstart, colmid, colend);
}

void kLinesSetup::slotHelp(){
	KMessageBox::about(this,
		i18n("Lines Version 2.2.0\n\n"
				   "Written by Dirk Staneker 1997\n"
				   "dirk.stanerker@student.uni-tuebingen.de"));
}

// Ok pressed - save settings and exit
void kLinesSetup::slotOk(){
    KConfig *config = KGlobal::config();
    config->setGroup("Settings");

    TQString slength;
    slength.setNum(length);
    config->writeEntry("Length", slength);

    TQString sspeed;
    sspeed.setNum( speed );
    config->writeEntry( "Speed", sspeed );

    TQString colName0, colName1, colName2;
    colName0.sprintf("#%02x%02x%02x", colstart.red(),
		     colstart.green(), colstart.blue() );
    config->writeEntry( "StartColor", colName0 );

    colName1.sprintf("#%02x%02x%02x", colmid.red(),
		     colmid.green(), colmid.blue() );
    config->writeEntry( "MidColor", colName1 );

    colName2.sprintf("#%02x%02x%02x", colend.red(),
		     colend.green(), colend.blue() );
    config->writeEntry( "EndColor", colName2 );

    config->sync();
    accept();
}

//-----------------------------------------------------------------------------


kLinesSaver::kLinesSaver( WId id ) : KScreenSaver( id ){
	readSettings();
	lines=new Lines(numLines);
	colorContext=TQColor::enterAllocContext();
	blank();
	initialiseColor();
	initialiseLines();
	timer.start(speed);
	connect(&timer, TQT_SIGNAL(timeout()), TQT_SLOT(slotTimeout()));
}

kLinesSaver::~kLinesSaver(){
	timer.stop();
	TQColor::leaveAllocContext();
	TQColor::destroyAllocContext(colorContext);
	delete lines;
}

// set lines properties
void kLinesSaver::setLines(int len){
	timer.stop();
	numLines=len;
	initialiseLines();
	initialiseColor();
	blank();
	timer.start(speed);
}

// set the speed
void kLinesSaver::setSpeed(int spd){
	timer.stop();
	speed=100-spd;
	timer.start(speed);
}

void kLinesSaver::setColor(const TQColor& cs, const TQColor& cm, const TQColor& ce){
	colstart=cs;
	colmid=cm;
	colend=ce;
        initialiseColor();
}

// read configuration settings from config file
void kLinesSaver::readSettings(){
    KConfig *config=KGlobal::config();
    config->setGroup("Settings");

    numLines=config->readNumEntry("Length", 10);
    speed = 100- config->readNumEntry("Speed", 50);
    if(numLines>MAXLENGTH) numLines=MAXLENGTH;
    else if(numLines<1) numLines = 1;

    colstart=config->readColorEntry("StartColor", &white);
    colmid=config->readColorEntry("MidColor", &blue);
    colend=config->readColorEntry("EndColor", &black);
}

// draw next lines and erase tail
void kLinesSaver::slotTimeout(){
	uint i;
	int x1,y1,x2,y2;
	int col=0;

	lines->reset();
    TQPainter p( this );
    p.setPen( black );

	for(i=0; i<numLines; i++){
		lines->getKoord(x1,y1,x2,y2);
        p.drawLine( x1, y1, x2, y2 );
		p.setPen( colors[col] );
		col=(int)(i*colscale);
		if(col>63) col=0;
	}
	lines->turn(width(), height());
}

void kLinesSaver::blank(){
	setBackgroundColor( black );
	erase();
}

// initialise the lines
void kLinesSaver::initialiseLines(){
	uint i;
	int x1,y1,x2,y2;
	delete lines;
	lines=new Lines(numLines);
	lines->reset();
	x1=rnd.getLong(width());
	y1=rnd.getLong(height());
	x2=rnd.getLong(width());
	y2=rnd.getLong(height());
	for(i=0; i<numLines; i++){
		lines->setKoord(x1,y1,x2,y2);
		lines->next();
	}
}

// create a color table of 64 colors
void kLinesSaver::initialiseColor(){
	int i;
	double mr, mg, mb;
	double cr, cg, cb;
    mr=(double)(colmid.red()-colstart.red())/32;
    mg=(double)(colmid.green()-colstart.green())/32;
    mb=(double)(colmid.blue()-colstart.blue())/32;
    cr=colstart.red();
    cg=colstart.green();
    cb=colstart.blue();
	for(i=0; i<32; i++){
		colors[63-i].setRgb((int)(mr*i+cr), (int)(mg*i+cg), (int)(mb*i+cb));
	}
	mr=(double)(colend.red()-colmid.red())/32;
	mg=(double)(colend.green()-colmid.green())/32;
	mb=(double)(colend.blue()-colmid.blue())/32;
	cr=colmid.red();
	cg=colmid.green();
	cb=colmid.blue();
	for(i=0; i<32; i++){
		colors[31-1].setRgb((int)(mr*i+cr), (int)(mg*i+cg), (int)(mb*i+cb));
	}
	colscale=64.0/(double)numLines;
}
