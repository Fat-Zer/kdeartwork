#include "nextclient.h"
#include <tqdatetime.h>
#include <tqdrawutil.h>
#include <tqlayout.h>
#include <tqpainter.h>
#include <tqbitmap.h>
#include <tqlabel.h>
#include <tqtooltip.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpixmapeffect.h>

namespace KStep {

static const unsigned char close_bits[] = {
  0x03, 0x03, 0x87, 0x03, 0xce, 0x01, 0xfc, 0x00, 0x78, 0x00, 0x78, 0x00,
  0xfc, 0x00, 0xce, 0x01, 0x87, 0x03, 0x03, 0x03};

static const unsigned char iconify_bits[] = {
  0xff, 0x03, 0xff, 0x03, 0xff, 0x03, 0xff, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0xff, 0x03, 0xff, 0x03};

static const unsigned char question_bits[] = {
  0x00, 0x00, 0x78, 0x00, 0xcc, 0x00, 0xc0, 0x00, 0x60, 0x00, 0x30, 0x00,
  0x00, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00};

static const unsigned char sticky_bits[] = {
  0x00, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0xfe, 0x01, 0xfe, 0x01,
  0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00};

static const unsigned char unsticky_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0xfe, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char maximize_bits[] = {
   0x30, 0x00, 0x78, 0x00, 0xfc, 0x00, 0xfe, 0x01, 0x00, 0x00, 0xfe, 0x01,
   0x02, 0x01, 0x84, 0x00, 0x48, 0x00, 0x30, 0x00 };

static const unsigned char shade_bits[] = {
    0xff,0x03,
    0xff,0x03,
    0x03,0x03,
    0xff,0x03,
    0xff,0x03,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00 
};
    
static const unsigned char unshade_bits[] = {
    0xff,0x03,
    0xff,0x03,
    0x03,0x03,
    0x03,0x03,
    0x03,0x03,
    0x03,0x03,
    0x03,0x03,
    0x03,0x03,
    0xff,0x03,
    0xff,0x03 
};

static const unsigned char keep_above_bits[] = {
    0x30,0x00,
    0x78,0x00,
    0xfc,0x00,
    0x00,0x00,
    0xff,0x03,
    0xff,0x03,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00 
};

static const unsigned char from_above_bits[] = {
    0xff,0x03,
    0xff,0x03,
    0x00,0x00,
    0xfc,0x00,
    0x78,0x00,
    0x30,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00 
};
    
static const unsigned char keep_below_bits[] = {
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0xff,0x03,
    0xff,0x03,
    0x00,0x00,
    0xfc,0x00,
    0x78,0x00,
    0x30,0x00
};

static const unsigned char from_below_bits[] = {
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x30,0x00,
    0x78,0x00,
    0xfc,0x00,
    0x00,0x00,
    0xff,0x03,
    0xff,0x03
};

static const unsigned char resize_bits[] = {
    0xff, 0x03, 
    0xff, 0x03, 
    0x33, 0x03, 
    0x33, 0x03, 
    0xf3, 0x03, 
    0xf3, 0x03,
    0x03, 0x03, 
    0x03, 0x03, 
    0xff, 0x03, 
    0xff, 0x03
};


// If the maximize graphic above (which I did quickly in about a
// minute, just so I could have something) doesn't please, maybe one
// of the following would be better.  IMO it doesn't matter, as long
// as it's not offensive---people will get used to whatever you use.
// True NeXT fans won't turn on the maximize button anyway.
//
// static const unsigned char maximize_bits[] = {
//    0xcf, 0x03, 0x87, 0x03, 0xcf, 0x03, 0xfd, 0x02, 0x48, 0x00, 0x48, 0x00,
//    0xfd, 0x02, 0xcf, 0x03, 0x87, 0x03, 0xcf, 0x03 };
//
// static const unsigned char maximize_bits[] = {
//    0xcf, 0x03, 0x87, 0x03, 0x87, 0x03, 0x79, 0x02, 0x48, 0x00, 0x48, 0x00,
//    0x79, 0x02, 0x87, 0x03, 0x87, 0x03, 0xcf, 0x03 };
//
// static const unsigned char maximize_bits[] = {
//    0x87, 0x03, 0x03, 0x03, 0xfd, 0x02, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00,
//    0x84, 0x00, 0xfd, 0x02, 0x03, 0x03, 0x87, 0x03 };
//
// static const unsigned char maximize_bits[] = {
//    0x30, 0x00, 0x78, 0x00, 0xcc, 0x00, 0x86, 0x01, 0x33, 0x03, 0x79, 0x02,
//    0xcd, 0x02, 0x87, 0x03, 0x03, 0x03, 0x01, 0x02 };
//
// static const unsigned char maximize_bits[] = {
//    0x30, 0x00, 0x78, 0x00, 0x78, 0x00, 0xfc, 0x00, 0xfc, 0x00, 0xfe, 0x01,
//    0xfe, 0x01, 0xff, 0x03, 0xff, 0x03, 0xff, 0x03 };


static KPixmap *aTitlePix;
static KPixmap *iTitlePix;
static KPixmap *aFramePix;
static KPixmap *iFramePix;
static KPixmap *aHandlePix;
static KPixmap *iHandlePix;
static KPixmap *aBtn;
static KPixmap *aBtnDown;
static KPixmap *iBtn;
static KPixmap *iBtnDown;
static TQColor *btnForeground;
static bool pixmaps_created = false;

static int titleHeight = 16;

// Precomputed border sizes for accessibility
// The sizes are applied for tiny -> normal -> large -> very large -> huge -> very huge -> oversized
static const int borderSizes[] = { 4,  6,  9, 14, 21, 32, 48 };

static int handleSize = 6;	// the resize handle size in pixels

static inline const KDecorationOptions* options()
{
    return KDecoration::options();
}

static void create_pixmaps(NextClientFactory *f)
{
    if(pixmaps_created)
        return;
    pixmaps_created = true;

    // find preferred border size
    int i = options()->preferredBorderSize(f);
    if (i >= 0 && i <= 6) handleSize = borderSizes[i];

    titleHeight = TQFontMetrics(options()->font(true)).height() + 4;
    if (titleHeight < handleSize) titleHeight = handleSize;
    titleHeight &= ~1; // Make title height even
    if (titleHeight < 16) titleHeight = 16;

    aTitlePix = new KPixmap();
    aTitlePix->resize(32, titleHeight - 2);
    KPixmapEffect::gradient(*aTitlePix,
                            options()->color(KDecoration::ColorTitleBar, true),
                            options()->color(KDecoration::ColorTitleBlend, true),
                            KPixmapEffect::VerticalGradient);
    iTitlePix = new KPixmap();
    iTitlePix->resize(32, titleHeight - 2);
    KPixmapEffect::gradient(*iTitlePix,
                            options()->color(KDecoration::ColorTitleBar, false),
                            options()->color(KDecoration::ColorTitleBlend, false),
                            KPixmapEffect::VerticalGradient);
    // Bottom frame gradient
    aFramePix = new KPixmap();
    aFramePix->resize(32, handleSize);
    KPixmapEffect::gradient(*aFramePix,
                            options()->color(KDecoration::ColorFrame, true).light(150),
                            options()->color(KDecoration::ColorFrame, true).dark(120),
                            KPixmapEffect::VerticalGradient);
    iFramePix = new KPixmap();
    iFramePix->resize(32, handleSize);
    KPixmapEffect::gradient(*iFramePix,
                            options()->color(KDecoration::ColorFrame, false).light(150),
                            options()->color(KDecoration::ColorFrame, false).dark(120),
                            KPixmapEffect::VerticalGradient);

    // Handle gradient
    aHandlePix = new KPixmap();
    aHandlePix->resize(32, handleSize);
    KPixmapEffect::gradient(*aHandlePix,
                            options()->color(KDecoration::ColorHandle, true).light(150),
                            options()->color(KDecoration::ColorHandle, true).dark(120),
                            KPixmapEffect::VerticalGradient);
    iHandlePix = new KPixmap();
    iHandlePix->resize(32, handleSize);
    KPixmapEffect::gradient(*iHandlePix,
                            options()->color(KDecoration::ColorHandle, false).light(150),
                            options()->color(KDecoration::ColorHandle, false).dark(120),
                            KPixmapEffect::VerticalGradient);

    int btnWidth = titleHeight;
    iBtn = new KPixmap;
    iBtn->resize(btnWidth, btnWidth);
    iBtnDown = new KPixmap;
    iBtnDown->resize(btnWidth, btnWidth);
    aBtn = new KPixmap;
    aBtn->resize(btnWidth, btnWidth);
    aBtnDown = new KPixmap;
    aBtnDown->resize(btnWidth, btnWidth);
    KPixmap internal;
    int internalHeight = btnWidth - 6;
    internal.resize(internalHeight, internalHeight);

    // inactive buttons
    TQColor c(options()->color(KDecoration::ColorButtonBg, false));
    KPixmapEffect::gradient(*iBtn, c.light(120), c.dark(120),
                            KPixmapEffect::DiagonalGradient);
    KPixmapEffect::gradient(internal, c.dark(120), c.light(120),
                            KPixmapEffect::DiagonalGradient);
    bitBlt(TQT_TQPAINTDEVICE(iBtn), 3, 3, TQT_TQPAINTDEVICE(&internal), 0, 0, internalHeight, internalHeight, TQt::CopyROP, true);

    KPixmapEffect::gradient(*iBtnDown, c.dark(120), c.light(120),
                            KPixmapEffect::DiagonalGradient);
    KPixmapEffect::gradient(internal, c.light(120), c.dark(120),
                            KPixmapEffect::DiagonalGradient);
    bitBlt(TQT_TQPAINTDEVICE(iBtnDown), 3, 3, TQT_TQPAINTDEVICE(&internal), 0, 0, internalHeight, internalHeight, TQt::CopyROP, true);

    // active buttons
    c = options()->color(KDecoration::ColorButtonBg, true);
    KPixmapEffect::gradient(*aBtn, c.light(120), c.dark(120),
                            KPixmapEffect::DiagonalGradient);
    KPixmapEffect::gradient(internal, c.dark(120), c.light(120),
                            KPixmapEffect::DiagonalGradient);
    bitBlt(TQT_TQPAINTDEVICE(aBtn), 3, 3, TQT_TQPAINTDEVICE(&internal), 0, 0, internalHeight, internalHeight, TQt::CopyROP, true);

    KPixmapEffect::gradient(*aBtnDown, c.dark(120), c.light(120),
                            KPixmapEffect::DiagonalGradient);
    KPixmapEffect::gradient(internal, c.light(120), c.dark(120),
                            KPixmapEffect::DiagonalGradient);
    bitBlt(TQT_TQPAINTDEVICE(aBtnDown), 3, 3, TQT_TQPAINTDEVICE(&internal), 0, 0, internalHeight, internalHeight, TQt::CopyROP, true);

    TQPainter p;
    p.begin(aBtn);
    p.setPen(TQt::black);
    p.drawRect(0, 0, btnWidth, btnWidth);
    p.end();
    p.begin(iBtn);
    p.setPen(TQt::black);
    p.drawRect(0, 0, btnWidth, btnWidth);
    p.end();
    p.begin(aBtnDown);
    p.setPen(TQt::black);
    p.drawRect(0, 0, btnWidth, btnWidth);
    p.end();
    p.begin(iBtnDown);
    p.setPen(TQt::black);
    p.drawRect(0, 0, btnWidth, btnWidth);
    p.end();

    if(tqGray(options()->color(KDecoration::ColorButtonBg, true).rgb()) > 128)
        btnForeground = new TQColor(TQt::black);
    else
        btnForeground = new TQColor(TQt::white);
}

static void delete_pixmaps()
{
    delete aTitlePix;
    delete iTitlePix;
    delete aFramePix;
    delete iFramePix;
    delete aHandlePix;
    delete iHandlePix;
    delete aBtn;
    delete iBtn;
    delete aBtnDown;
    delete iBtnDown;
    delete btnForeground;

    pixmaps_created = false;
}

// =====================================

NextButton::NextButton(NextClient *parent, const char *name,
                       const unsigned char *bitmap, int bw, int bh,
                       const TQString& tip, const int realizeBtns)
    : TQButton(parent->widget(), name),
      deco(NULL), client(parent), last_button(Qt::NoButton)
{
    realizeButtons = realizeBtns;

    setBackgroundMode( NoBackground );
    resize(titleHeight, titleHeight);
    setFixedSize(titleHeight, titleHeight);

    if(bitmap)
        setBitmap(bitmap, bw, bh);

    TQToolTip::add(this, tip);
}

void NextButton::reset()
{
    tqrepaint(false);
}

void NextButton::setBitmap(const unsigned char *bitmap, int w, int h)
{
    deco = new TQBitmap(w, h, bitmap, true);
    deco->setMask(*deco);
    tqrepaint();
}

void NextButton::drawButton(TQPainter *p)
{
    if(client->isActive())
        p->drawPixmap(0, 0, isDown() ? *aBtnDown : *aBtn);
    else
        p->drawPixmap(0, 0, isDown() ? *iBtnDown : *iBtn);

    // If we have a decoration, draw it; otherwise, we have the menu
    // button (remember, we set the bitmap to NULL).
    int offset;
    if (deco) {
        offset = (titleHeight - 10) / 2 + (isDown() ? 1 : 0);
        p->setPen(*btnForeground);
        p->drawPixmap(offset, offset, *deco);
    } else {
        offset = (titleHeight - 16) / 2;
	KPixmap btnpix = client->icon().pixmap(TQIconSet::Small,
		client->isActive() ? TQIconSet::Normal : TQIconSet::Disabled);
        p->drawPixmap( offset, offset, btnpix );
    }
}

void NextButton::mousePressEvent( TQMouseEvent* e )
{
    last_button = e->button();
    TQMouseEvent me( e->type(), e->pos(), e->globalPos(),
                    (e->button()&realizeButtons)?Qt::LeftButton:Qt::NoButton, e->state() );
    TQButton::mousePressEvent( &me );
}

void NextButton::mouseReleaseEvent( TQMouseEvent* e )
{
    last_button = e->button();
    TQMouseEvent me( e->type(), e->pos(), e->globalPos(),
                    (e->button()&realizeButtons)?Qt::LeftButton:Qt::NoButton, e->state() );
    TQButton::mouseReleaseEvent( &me );
}

// =====================================

NextClient::NextClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f)
{
}

void NextClient::init()
{
    createMainWidget(WResizeNoErase | WStaticContents);
    widget()->installEventFilter(this);

    widget()->setBackgroundMode( NoBackground );

    TQVBoxLayout *mainLayout = new TQVBoxLayout(widget());
    TQBoxLayout  *titleLayout = new TQBoxLayout(0, TQBoxLayout::LeftToRight, 0, 0, 0);
    TQHBoxLayout *windowLayout = new TQHBoxLayout();
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(windowLayout, 1);
    mainLayout->addSpacing(mustDrawHandle() ? handleSize : 1);

    windowLayout->addSpacing(1);
    if (isPreview())
        windowLayout->addWidget(new TQLabel(i18n(
			"<center><b>KStep preview</b></center>"), widget()));
    else
        windowLayout->addItem(new TQSpacerItem( 0, 0 ));

    windowLayout->addSpacing(1);

    initializeButtonsAndTitlebar(titleLayout);
}

/**
   Preconditions:
       + button is an array of length MAX_NUM_BUTTONS

   Postconditions:
       + Title bar and buttons have been initialized and laid out
       + for all i in 0..(MAX_NUM_BUTTONS-1), button[i] points to
         either (1) a valid NextButton instance, if the corresponding
         button is selected in the current button scheme, or (2) null
         otherwise.
 */
void NextClient::initializeButtonsAndTitlebar(TQBoxLayout* titleLayout)
{
    // Null the buttons to begin with (they are not guaranteed to be null).
    for (int i=0; i<MAX_NUM_BUTTONS; i++) {
        button[i] = NULL;
    }

    // The default button positions for other styles do not match the
    // behavior of older versions of KStep, so we have to set these
    // manually when customButtonPositions isn't enabled.
    TQString left, right;
    if (options()->customButtonPositions()) {
        left = options()->titleButtonsLeft();
        right = options()->titleButtonsRight();
    } else {
        left = TQString("I");
        right = TQString("SX");
    }

    // Do actual creation and addition to titleLayout
    addButtons(titleLayout, left);

    titlebar = new TQSpacerItem(10, titleHeight, TQSizePolicy::Expanding,
                               TQSizePolicy::Minimum );
    titleLayout->addItem(titlebar);
    addButtons(titleLayout, right);

    // Finally, activate all live buttons
    for ( int i = 0; i < MAX_NUM_BUTTONS; i++) {
        if (button[i]) {
            button[i]->setMouseTracking( TRUE );
        }
    }
}

/** Adds the buttons for one side of the title bar, based on the spec
 * string; see the KWinInternal::KDecoration class, methods
 * titleButtonsLeft and titleBUttonsRight. */
void NextClient::addButtons(TQBoxLayout* titleLayout, const TQString& spec)
{
    for (unsigned int i=0; i<spec.length(); i++) {
        switch (spec[i].latin1()) {
        case 'A':
            if (isMaximizable()) {
                button[MAXIMIZE_IDX] =
                    new NextButton(this, "maximize", maximize_bits, 10, 10,
                                   i18n("Maximize"), Qt::LeftButton|Qt::MidButton|Qt::RightButton);
                titleLayout->addWidget( button[MAXIMIZE_IDX] );
                connect( button[MAXIMIZE_IDX], TQT_SIGNAL(clicked()),
                         this, TQT_SLOT(maximizeButtonClicked()) );
            }
            break;

        case 'H':
	    if (providesContextHelp()) {
		button[HELP_IDX] = new NextButton(this,
			"help", question_bits, 10, 10, i18n("Help"));
		titleLayout->addWidget( button[HELP_IDX] );
		connect( button[HELP_IDX], TQT_SIGNAL(clicked()),
			 this, TQT_SLOT(showContextHelp()) );
	    }
            break;

        case 'I':
            if (isMinimizable()) {
                button[ICONIFY_IDX] =
                    new NextButton(this, "iconify", iconify_bits, 10, 10,
                                   i18n("Minimize"));
                titleLayout->addWidget( button[ICONIFY_IDX] );
                connect( button[ICONIFY_IDX], TQT_SIGNAL(clicked()),
                         this, TQT_SLOT(minimize()) );
            }
            break;

        case 'M':
            button[MENU_IDX] =
                new NextButton(this, "menu", NULL, 10, 10, i18n("Menu"), Qt::LeftButton|Qt::RightButton);
            titleLayout->addWidget( button[MENU_IDX] );
            // NOTE DIFFERENCE: capture pressed(), not clicked()
            connect( button[MENU_IDX], TQT_SIGNAL(pressed()),
                     this, TQT_SLOT(menuButtonPressed()) );
            break;

        case 'L':
            button[SHADE_IDX] =
                new NextButton(this, "shade", NULL, 0, 0, i18n("Shade"));
            titleLayout->addWidget( button[SHADE_IDX] );
            connect( button[SHADE_IDX], TQT_SIGNAL(clicked()),
                     this, TQT_SLOT(shadeClicked()) );
            // NOTE DIFFERENCE: set the pixmap separately (2 states)
	    shadeChange();
            break;
	    
        case 'S':
            button[STICKY_IDX] =
                new NextButton(this, "sticky", NULL, 0, 0, i18n("On all desktops"));
            titleLayout->addWidget( button[STICKY_IDX] );
            connect( button[STICKY_IDX], TQT_SIGNAL(clicked()),
                     this, TQT_SLOT(toggleOnAllDesktops()) );
            // NOTE DIFFERENCE: set the pixmap separately (2 states)
	    desktopChange();
            break;

	case 'F':
            button[ABOVE_IDX] = new NextButton(this, "above", NULL, 0, 0, "");
            titleLayout->addWidget( button[ABOVE_IDX] );
            connect( button[ABOVE_IDX], TQT_SIGNAL(clicked()),
                     this, TQT_SLOT(aboveClicked()) );
	    connect(this, TQT_SIGNAL(keepAboveChanged(bool)), 
		    TQT_SLOT(keepAboveChange(bool)));
	    keepAboveChange(keepAbove());
	    break;
	    
	case 'B':
            button[BELOW_IDX] = new NextButton(this, "below", NULL, 0, 0, "");
            titleLayout->addWidget( button[BELOW_IDX] );
            connect( button[BELOW_IDX], TQT_SIGNAL(clicked()),
                     this, TQT_SLOT(belowClicked()) );
	    connect(this, TQT_SIGNAL(keepBelowChanged(bool)), 
		    TQT_SLOT(keepBelowChange(bool)));
	    keepBelowChange(keepBelow());
	    break;
        
	case 'X':
            if (isCloseable()) {
		button[CLOSE_IDX] =
		    new NextButton(this, "close", close_bits, 10, 10,
				   i18n("Close"));
		titleLayout->addWidget(button[CLOSE_IDX]);
		connect(button[CLOSE_IDX], TQT_SIGNAL(clicked()),
			 this, TQT_SLOT(closeWindow()));
            }
            break;

	case 'R':
            if (mustDrawHandle()) {
		button[RESIZE_IDX] =
		    new NextButton(this, "resize", resize_bits, 10, 10,
				   i18n("Resize"));
		titleLayout->addWidget(button[RESIZE_IDX]);
		// NOTE DIFFERENCE: capture pressed(), not clicked()
		connect(button[RESIZE_IDX], TQT_SIGNAL(pressed()),
			 this, TQT_SLOT(resizePressed()));
            }
	    break;
        case '_':
            // TODO: Add spacer handling
            break;

        default:
            kdDebug() << " Can't happen: unknown button code "
                      << TQString(spec[i]);
            break;
        }
    }
}

bool NextClient::mustDrawHandle() const 
{ 
    bool drawSmallBorders = !options()->moveResizeMaximizedWindows();
    if (drawSmallBorders && (maximizeMode() & MaximizeVertical)) {
	return false;
    } else {
	return isResizable();
    }
}

void NextClient::iconChange()
{
    if (button[MENU_IDX] && button[MENU_IDX]->isVisible())
	button[MENU_IDX]->tqrepaint(false);
}

void NextClient::menuButtonPressed()
{
    // Probably don't need this null check, but we might as well.
    if (button[MENU_IDX]) {
	TQRect menuRect = button[MENU_IDX]->rect();
        TQPoint menuTop = button[MENU_IDX]->mapToGlobal(menuRect.topLeft());
        TQPoint menuBottom = button[MENU_IDX]->mapToGlobal(menuRect.bottomRight());
	menuTop += TQPoint(1, 1);
	menuBottom += TQPoint(1, 1);
        KDecorationFactory* f = factory();
        showWindowMenu(TQRect(menuTop, menuBottom));
        if( !f->exists( this )) // 'this' was deleted
            return;
	button[MENU_IDX]->setDown(false);
    }
}

// Copied, with minor edits, from KDEDefaultClient::slotMaximize()
void NextClient::maximizeButtonClicked()
{
    if (button[MAXIMIZE_IDX]) {
        maximize(button[MAXIMIZE_IDX]->lastButton());
    }
}

void NextClient::shadeClicked()
{
    setShade(!isSetShade());
}

void NextClient::aboveClicked()
{
    setKeepAbove(!keepAbove());
}

void NextClient::belowClicked()
{
    setKeepBelow(!keepBelow());
    keepAboveChange(keepAbove());
    keepBelowChange(keepBelow());
}

void NextClient::resizePressed()
{
    performWindowOperation(ResizeOp);
}

void NextClient::resizeEvent(TQResizeEvent *)
{
    if (widget()->isVisible()) {
	// TODO ? update border area only?
        widget()->update();
#if 0
        widget()->update(titlebar->tqgeometry());
        TQPainter p(widget());
	TQRect t = titlebar->tqgeometry();
	t.setTop( 0 );
	TQRegion r = widget()->rect();
	r = r.subtract( t );
	p.setClipRegion( r );
	p.eraseRect(widget()->rect());
#endif
    }
}

void NextClient::captionChange()
{
    widget()->tqrepaint(titlebar->tqgeometry(), false);
}


void NextClient::paintEvent( TQPaintEvent* )
{
    TQPainter p(widget());

    // Draw black frame
    TQRect fr = widget()->rect();
    p.setPen(TQt::black);
    p.drawRect(fr);

    // Draw title bar
    TQRect t = titlebar->tqgeometry();
    t.setTop(1);
    p.drawTiledPixmap(t.x()+1, t.y()+1, t.width()-2, t.height()-2,
                      isActive() ? *aTitlePix : *iTitlePix);
    qDrawShadePanel(&p, t.x(), t.y(), t.width(), t.height()-1,
                   options()->tqcolorGroup(KDecoration::ColorTitleBar, isActive()));
    p.drawLine(t.x(), t.bottom(), t.right(), t.bottom());

#if 0
    // Why setting up a clipping region if it is not used? (setClipping(false))
    TQRegion r = fr;
    r = r.subtract( t );
    p.setClipRegion( r );
    p.setClipping(false);
#endif

    t.setTop( 1 );
    t.setHeight(t.height()-2);
    t.setLeft( t.left() + 4 );
    t.setRight( t.right() - 2 );

    p.setPen(options()->color(KDecoration::ColorFont, isActive()));
    p.setFont(options()->font(isActive()));
    p.drawText( t, AlignCenter | AlignVCenter, caption() );

    // Draw resize handle
    if (mustDrawHandle()) {
        int corner = 16 + 3*handleSize/2;
	qDrawShadePanel(&p,
		fr.x() + 1, fr.bottom() - handleSize, corner-1, handleSize,
		options()->tqcolorGroup(KDecoration::ColorHandle, isActive()),
		false);
	p.drawTiledPixmap(fr.x() + 2, fr.bottom() - handleSize + 1,
		corner - 3, handleSize - 2, isActive() ? *aHandlePix : *iHandlePix);

	qDrawShadePanel(&p,
		fr.x() + corner, fr.bottom() - handleSize,
		fr.width() - 2*corner, handleSize,
		options()->tqcolorGroup(KDecoration::ColorFrame, isActive()),
		false);
	p.drawTiledPixmap(fr.x() + corner + 1, fr.bottom() - handleSize + 1,
		fr.width() - 2*corner - 2, handleSize - 2,
		isActive() ? *aFramePix : *iFramePix);

	qDrawShadePanel(&p,
		fr.right() - corner + 1, fr.bottom() - handleSize, corner - 1, handleSize,
		options()->tqcolorGroup(KDecoration::ColorHandle, isActive()),
		false);
	p.drawTiledPixmap(fr.right() - corner + 2, fr.bottom() - handleSize + 1,
		corner - 3, handleSize - 2, isActive() ? *aHandlePix : *iHandlePix);
    }
}

void NextClient::mouseDoubleClickEvent( TQMouseEvent * e )
{
    if (e->button() == Qt::LeftButton && titlebar->tqgeometry().contains( e->pos() ) )
	titlebarDblClickOperation();
}

void NextClient::wheelEvent( TQWheelEvent * e )
{
    if (isSetShade() || TQRect( 0, 0, width(), titleHeight ).contains( e->pos() ) )
	titlebarMouseWheelOperation( e->delta());
}

void NextClient::showEvent(TQShowEvent *)
{
    widget()->tqrepaint();
}

void NextClient::desktopChange()
{
    bool on = isOnAllDesktops();
    if (NextButton * b = button[STICKY_IDX]) {
        b->setBitmap( on ? unsticky_bits : sticky_bits, 10, 10);
	TQToolTip::remove(b);
	TQToolTip::add(b, on ? i18n("Not on all desktops") : i18n("On all desktops"));
    }
}

void NextClient::maximizeChange()
{
    if (button[MAXIMIZE_IDX]) {
	bool m = maximizeMode() == MaximizeFull;
	//button[MAXIMIZE_IDX]->setBitmap(m ? minmax_bits : maximize_bits);
	TQToolTip::remove(button[MAXIMIZE_IDX]);
	TQToolTip::add(button[MAXIMIZE_IDX],
		m ? i18n("Restore") : i18n("Maximize"));
    }
    //spacer->changeSize(10, mustDrawHandle() ? handleSize : 1,
    //		    TQSizePolicy::Expanding, TQSizePolicy::Minimum);
    //mainLayout->activate();
}

void NextClient::activeChange()
{
    widget()->tqrepaint(false);
    slotReset();
}

void NextClient::slotReset()
{
    for (int i=0; i<MAX_NUM_BUTTONS; i++) {
        if (button[i]) {
            button[i]->reset();
        }
    }
}

KDecoration::Position
NextClient::mousePosition( const TQPoint& p ) const
{
  Position m = PositionCenter;

  if (p.y() < (height() - handleSize))
    m = KDecoration::mousePosition(p);

  else {
    int corner = 16 + 3*handleSize/2;
    if (p.x() >= (width() - corner))
      m = PositionBottomRight;
    else if (p.x() <= corner)
      m = PositionBottomLeft;
    else
      m = PositionBottom;
  }

  return m;
}

void NextClient::borders(int &left, int &right, int &top, int &bottom) const
{
    left = right = 1;
    top = titleHeight; // FRAME is this ok?
    bottom = mustDrawHandle() ? handleSize : 1;
}

void NextClient::shadeChange()
{
    if (NextButton *b = button[SHADE_IDX]) {
        b->setBitmap(isSetShade() ? unshade_bits : shade_bits, 10, 10);
	TQToolTip::remove(b);
	TQToolTip::add(b, isSetShade() ? i18n("Unshade") : i18n("Shade"));
    }
}

void NextClient::keepAboveChange(bool above)
{
    if (NextButton *b = button[ABOVE_IDX]) {
        b->setBitmap(above ? from_above_bits : keep_above_bits, 10, 10);
	TQToolTip::remove(b);
	TQToolTip::add(b, above ? 
		i18n("Do not keep above others") : i18n("Keep above others"));
	b->tqrepaint(false);
    }
}

void NextClient::keepBelowChange(bool below)
{
    if (NextButton *b = button[BELOW_IDX]) {
        b->setBitmap(below ? from_below_bits : keep_below_bits, 10, 10);
	TQToolTip::remove(b);
	TQToolTip::add(b, below ? 
		i18n("Do not keep below others") : i18n("Keep below others"));
	b->tqrepaint(false);
    }
}

TQSize NextClient::tqminimumSize() const
{
    return TQSize(titleHeight * 6 + 2, titleHeight + handleSize + 2);
}

void NextClient::resize(const TQSize& s)
{
    widget()->resize(s);
}

void NextClient::reset(unsigned long)
{
    for (int i = 0; i < MAX_NUM_BUTTONS; ++i) {
        if (button[i])
            button[i]->reset();
    }
    widget()->tqrepaint();
}

bool NextClient::eventFilter(TQObject *o, TQEvent *e)
{
    if (TQT_BASE_OBJECT(o) != TQT_BASE_OBJECT(widget()))
	return false;
    switch (e->type()) {
    case TQEvent::Resize:
	resizeEvent(TQT_TQRESIZEEVENT( e ));
	return true;
    case TQEvent::Paint:
	paintEvent(TQT_TQPAINTEVENT( e ));
	return true;
    case TQEvent::MouseButtonDblClick:
	mouseDoubleClickEvent(TQT_TQMOUSEEVENT( e ));
	return true;
    case TQEvent::Wheel:
	wheelEvent( TQT_TQWHEELEVENT( e ));
	return true;
    case TQEvent::MouseButtonPress:
	processMousePressEvent(TQT_TQMOUSEEVENT( e ));
	return true;
    case TQEvent::Show:
	showEvent(TQT_TQSHOWEVENT( e ));
	return true;
    default:
	break;
    }
    return false;
}

bool NextClient::drawbound(const TQRect& geom, bool /* clear */)
{
    TQPainter p(workspaceWidget());
    p.setPen(TQPen(TQt::white, 3));
    p.setRasterOp(TQt::XorROP);
    p.drawRect(geom);
    int leftMargin = geom.left() + 2;
    p.fillRect(leftMargin, geom.top() + titleHeight - 1,
	    geom.width() - 4, 3, TQt::white);
    if (mustDrawHandle()) {
	p.fillRect(leftMargin, geom.bottom() - handleSize - 1,
		geom.width() - 4, 3, TQt::white);
    }	    
    return true;
}

// =====================================

NextClientFactory::NextClientFactory()
{
    create_pixmaps(this);
}

NextClientFactory::~NextClientFactory()
{
    delete_pixmaps();
}

KDecoration *NextClientFactory::createDecoration(KDecorationBridge *b)
{
    return new NextClient(b, this);
}

bool NextClientFactory::reset(unsigned long /*changed*/)
{
    // TODO Do not recreate decorations if it is not needed. Look at
    // ModernSystem for how to do that
    delete_pixmaps();
    create_pixmaps(this);
    // For now just return true.
    return true;
}

bool NextClientFactory::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonMenu:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
        case AbilityButtonAboveOthers:
        case AbilityButtonBelowOthers:
        case AbilityButtonShade:
        case AbilityButtonResize:
            return true;
        default:
            return false;
    };
}

TQValueList< NextClientFactory::BorderSize >
NextClientFactory::borderSizes() const
{
    // the list must be sorted
    return TQValueList< BorderSize >() << BorderTiny << BorderNormal <<
	BorderLarge << BorderVeryLarge <<  BorderHuge <<
	BorderVeryHuge << BorderOversized;
}

} // KStep namespace

extern "C" KDE_EXPORT KDecorationFactory* create_factory()
{
    return new KStep::NextClientFactory();
}

#include "nextclient.moc"

// vim: sw=4
