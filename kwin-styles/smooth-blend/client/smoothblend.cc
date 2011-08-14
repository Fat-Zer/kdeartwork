//////////////////////////////////////////////////////////////////////////////
// smoothblend.cc
// -------------------
// Smooth Blend window decoration for KDE
// -------------------
// Copyright (c) 2005 Ryan Nickell
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <kdeversion.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kimageeffect.h>
#include <kpixmapeffect.h>
#include <kpixmap.h>

#include <tqbitmap.h>
#include <tqlabel.h>
#include <tqlayout.h>
#include <tqpainter.h>
#include <tqtooltip.h>
#include <tqtimer.h>
#include <tqapplication.h>

#include "smoothblend.h"
#include "buttons.h"

using namespace smoothblend;

//////////////////////////////////////////////////////////////////////////////
// smoothblendFactory Class
//////////////////////////////////////////////////////////////////////////////
smoothblendFactory* factory=NULL;

bool smoothblendFactory::initialized_              = false;
TQt::AlignmentFlags smoothblendFactory::titlealign_ = TQt::AlignHCenter;
bool smoothblendFactory::cornerflags_               = true;
int smoothblendFactory::titlesize_                 = 30;
int smoothblendFactory::buttonsize_                = 26;
int smoothblendFactory::framesize_                 = 4;
int smoothblendFactory::roundsize_                 = 50;
bool smoothblendFactory::titleshadow_              = true;
bool smoothblendFactory::animatebuttons            = true;
int smoothblendFactory::btnComboBox                = 0;
bool smoothblendFactory::menuClose                 = false;

// global constants
static const int TOPMARGIN       = 4; // do not change
static const int DECOHEIGHT      = 4; // do not change
static const int SIDETITLEMARGIN = 2;
// Default button tqlayout
const char default_left[]  = "M";
const char default_right[] = "HIAX";

static const uint TIMERINTERVAL = 50; // msec
static const uint ANIMATIONSTEPS = 4;

extern "C" KDecorationFactory* create_factory() {
    return new smoothblend::smoothblendFactory();
}

//////////////////////////////////////////////////////////////////////////////
// smoothblendFactory()
// ----------------
// Constructor

smoothblendFactory::smoothblendFactory() {
    readConfig();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~smoothblendFactory()
// -----------------
// Destructor

smoothblendFactory::~smoothblendFactory() {
    initialized_ = false;
}

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* smoothblendFactory::createDecoration(KDecorationBridge* b) {
    return new smoothblendClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a tqrepaint is necessary

bool smoothblendFactory::reset(unsigned long changed) {
    // read in the configuration
    initialized_ = false;
    bool confchange = readConfig();
    initialized_ = true;

    if (confchange ||
            (changed & (SettingDecoration | SettingButtons | SettingBorder))) {
        return true;
    } else {
        resetDecorations(changed);
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////
// readConfig()
// ------------
// Read in the configuration file

bool smoothblendFactory::readConfig() {
    // create a config object
    KConfig config("kwinsmoothblendrc");
    config.setGroup("General");

    // grab settings
    TQString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft")
        titlealign_ = TQt::AlignLeft;
    else if (value == "AlignHCenter")
        titlealign_ = TQt::AlignHCenter;
    else if (value == "AlignRight")
        titlealign_ = TQt::AlignRight;

    cornerflags_ = config.readBoolEntry("RoundCorners", true);
    titlesize_ = config.readNumEntry("TitleSize",30);
    buttonsize_ = config.readNumEntry("ButtonSize",26);
    framesize_ = config.readNumEntry("FrameSize",4);
    roundsize_ = config.readNumEntry("RoundPercent",50);
    titleshadow_ = config.readBoolEntry("TitleShadow", true);
    animatebuttons = config.readBoolEntry("AnimateButtons", true);
    btnComboBox = config.readNumEntry("ButtonComboBox", 0);
    menuClose = config.readBoolEntry("CloseOnMenuDoubleClick");

    if(buttonsize_ > titlesize_ - framesize_)
    {
        buttonsize_ = titlesize_-framesize_;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// smoothblendButton Class 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// smoothblendButton()
// ---------------
// Constructor
smoothblendButton::smoothblendButton(smoothblendClient *parent, const char *name, const TQString& tip, ButtonType type, int button_size, bool toggle): TQButton(parent->widget(), name), 
     client_(parent), 
     type_(type), 
     size_(button_size), 
     deco_(0), 
     lastmouse_(Qt::NoButton), 
     hover_(false)
{
    setBackgroundMode(NoBackground);
    setFixedSize( ::factory->buttonSize(), ::factory->buttonSize());
    setCursor(arrowCursor);
    TQToolTip::add(this, tip);
    setToggleButton(toggle);
    //button animation setup
    animTmr = new TQTimer(this);
    connect(animTmr, TQT_SIGNAL(timeout() ), this, TQT_SLOT(animate() ) );
    connect(this, TQT_SIGNAL(pressed() ), this, TQT_SLOT(buttonClicked() ) );
    connect(this, TQT_SIGNAL(released() ), this, TQT_SLOT(buttonReleased() ) );
    animProgress = 0;
    m_clicked=false;
}

smoothblendButton::~smoothblendButton() {
    if ( deco_ )
        delete deco_;
}

//////////////////////////////////////////////////////////////////////////////
// tqsizeHint()
// ----------
// Return size hint

TQSize smoothblendButton::tqsizeHint() const {
    return TQSize(::factory->buttonSize(), ::factory->buttonSize());
}

//////////////////////////////////////////////////////////////////////////////
// buttonClicked()
// ----------
// Button animation progress reset so we don't get any leave event animation
// when the mouse is still pressed
void smoothblendButton::buttonClicked() {
    m_clicked=true;
    animProgress=0;
}
void smoothblendButton::buttonReleased() {
    //This doesn't work b/c a released() signal is thrown when a leaveEvent occurs
    //m_clicked=false;
}

//////////////////////////////////////////////////////////////////////////////
// animate()
// ----------
// Button animation timing
void smoothblendButton::animate() {
    animTmr->stop();

    if(hover_) {
        if(animProgress < ANIMATIONSTEPS) {
            if (::factory->animateButtons() ) {
                animProgress++;
            } else {
                animProgress = ANIMATIONSTEPS;
            }
            animTmr->start(TIMERINTERVAL, true); // single-shot
        }
    } else {
        if(animProgress > 0) {
            if (::factory->animateButtons() ) {
                animProgress--;
            } else {
                animProgress = 0;
            }
            animTmr->start(TIMERINTERVAL, true); // single-shot
        }
    }
    tqrepaint(false);
}
//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void smoothblendButton::enterEvent(TQEvent *e) {
    // we wanted to pass on the event
    TQButton::enterEvent(e);
    // we want to do mouseovers, so keep track of it here
    hover_=true;
    if(!m_clicked)
    {
        animate();
    }
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void smoothblendButton::leaveEvent(TQEvent *e) {
    // we wanted to pass on the event
    TQButton::leaveEvent(e);
    // we want to do mouseovers, so keep track of it here
    hover_=false; 
    if(!m_clicked)
    {
        animate();
    }
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void smoothblendButton::mousePressEvent(TQMouseEvent* e) {
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = Qt::LeftButton;
    if ((type_ != ButtonMax) && (e->button() != Qt::LeftButton)) {
        button = Qt::NoButton; // middle & right buttons inappropriate
    }
    TQMouseEvent me(e->type(), e->pos(), e->globalPos(),
                   button, e->state());
    TQButton::mousePressEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// mouseReleaseEvent()
// -----------------
// Button has been released

void smoothblendButton::mouseReleaseEvent(TQMouseEvent* e) {
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = Qt::LeftButton;
    if ((type_ != ButtonMax) && (e->button() != Qt::LeftButton)) {
        button = Qt::NoButton; // middle & right buttons inappropriate
    }
    TQMouseEvent me(e->type(), e->pos(), e->globalPos(), button, e->state());
    TQButton::mouseReleaseEvent(&me);
    if(m_clicked)
    {
        m_clicked=false;
    }
}

void smoothblendButton::setOn(bool on)
{
    TQButton::setOn(on);
}

void smoothblendButton::setDown(bool on)
{
    TQButton::setDown(on);
}

//////////////////////////////////////////////////////////
// getButtonImage()
// ----------------
// get the button TQImage based on type and window mode
TQImage smoothblendButton::getButtonImage(ButtonType type)
{
    TQImage finalImage;
    switch(type) {
        case ButtonClose:
            finalImage = uic_findImage( "close.png" );
            break;
        case ButtonHelp:
            finalImage = uic_findImage( "help.png" );
            break;
        case ButtonMin:
            finalImage = uic_findImage( "minimize.png" );
            break;
        case ButtonMax:
            if(client_->maximizeMode() == KDecorationDefines::MaximizeFull)
            {
                finalImage = uic_findImage( "restore.png" );
            }
            else
            {
                finalImage = uic_findImage( "maximize.png" );
            }
            break;
        case ButtonSticky:
            if(client_->isOnAllDesktops())
            {
                finalImage = uic_findImage( "splat.png" );
            }
            else
            {
                finalImage = uic_findImage( "circle.png" );
            }
            break;
        case ButtonShade:
            if(client_->isShade())
            {
                finalImage = uic_findImage( "shade.png" );
            }
            else
            {
                finalImage = uic_findImage( "shade.png" );
            }
            break;
        case ButtonAbove:
            if(client_->keepAbove())
            {
                finalImage = uic_findImage( "keep_above_lit.png" );
            }
            else
            {
                finalImage = uic_findImage( "keep_above.png" );
            }
            break;
        case ButtonBelow:
            if(client_->keepBelow())
            {
                finalImage = uic_findImage( "keep_below_lit.png" );
            }
            else
            {
                finalImage = uic_findImage( "keep_below.png" );
            }
            break;
        default:
            finalImage = uic_findImage( "splat.png" );
            break;
    }
    return finalImage;
}

//////////////////////////////////////////////////////////
// drawButton()
// -------------------------
// draw the pixmap button

void smoothblendButton::drawButton( TQPainter *painter ) {
    if ( !smoothblendFactory::initialized() )
        return ;
    
    int newWidth = width() - 2;
    int newHeight = height() - 2;
    int dx = (width() - newWidth) / 2;
    int dy = (height() - newHeight) / 2;
    TQImage tmpResult;
    TQColorGroup group;
    TQColor redColor(red);
    bool active = client_->isActive();
    TQPixmap backgroundTile = client_->getTitleBarTile(active);
    group = KDecoration::options()->tqcolorGroup(KDecoration::ColorTitleBar, active);

    //draw the titlebar behind the buttons and app icons
    if ((client_->maximizeMode()==client_->MaximizeFull) && !KDecoration::options()->moveResizeMaximizedWindows())
    {
        painter->drawTiledPixmap(0, 0, width(), height(), backgroundTile);
    }
    else
    {
        painter->drawTiledPixmap(0, 0, width(), height(), backgroundTile, 0, y()-::factory->frameSize());
    }
    
    TQImage buttonImage = getButtonImage(type_).smoothScale( width(),height());
    buttonImage = KImageEffect::blend( group.background(), buttonImage, .50);
    if (type_ == ButtonMenu) {
        //slight movement to show the menu button is depressed
        if (isDown()) {
            dx++;
            dy++;
        }
        TQPixmap menuButtonPixmap(client_->icon().pixmap(TQIconSet::Large, TQIconSet::Normal));
        TQImage menuButtonImage(menuButtonPixmap.convertToImage());
        //draw the menu button the same size as the other buttons
        //using TQIconSet::Large gives us a 32x32 icon to resize, resizing larger than
        //that may produce pixilation of the image
        painter->drawImage( dx, dy, menuButtonImage.smoothScale(newWidth, newHeight) );
    } else {
        //highlight on a mouse over tqrepaint
        double factor = animProgress * 0.13;

        if(!isDown())
        {
            switch(::factory->getBtnComboBox())
            {
                case 0:
                    tmpResult = KImageEffect::intensity( buttonImage, factor);
                    break;
                case 1:
                    tmpResult = KImageEffect::fade( buttonImage, factor, group.background());
                    break;
            }
        }
        else
        {
            tmpResult = buttonImage;
        }
        painter->drawPixmap( 0, 0, TQPixmap( tmpResult ) );
    }
}


//////////////////////////////////////////////////////////////////////////////
// smoothblendClient Class
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// smoothblendClient()
// ---------------
// Constructor

smoothblendClient::smoothblendClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f),
    mainLayout_(0),
    titleLayout_(0),
    topSpacer_(0),
    titleSpacer_(0),
    leftTitleSpacer_(0), rightTitleSpacer_(0),
    decoSpacer_(0),
    leftSpacer_(0), rightSpacer_(0),
    bottomSpacer_(0), windowSpacer_(0),
    aCaptionBuffer(0), iCaptionBuffer(0),
    aTitleBarTile(0), iTitleBarTile(0), aTitleBarTopTile(0), iTitleBarTopTile(0),
    pixmaps_created(false),
    //captionBufferDirty(true),
    s_titleHeight(0),
    s_titleFont(TQFont()),
    closing(false)
    {
        aCaptionBuffer = new TQPixmap();
        iCaptionBuffer = new TQPixmap();
        //s_titleFont = isTool()?smoothblendFactory::titleFontTool():smoothblendFactory::titleFont();
        s_titleFont = options()->font();
        s_titleHeight = smoothblendFactory::titleSize();
    }
//////////////////////////////////////////////////////////////////////////////////
// ~smoothblendClient()
// --------------------
// Destructor
smoothblendClient::~smoothblendClient() {
    delete aCaptionBuffer;
    delete iCaptionBuffer;
}

void smoothblendClient::create_pixmaps() {
    if(pixmaps_created)
        return;
    KPixmap tempPixmap;
    TQPainter painter;
    TQColorGroup group,widgetGroup;
    int FRAMESIZE = ::factory->frameSize();
    // Get the color groups we need for the gradients
    group = options()->tqcolorGroup(KDecoration::ColorTitleBar, true);
    widgetGroup = widget()->tqcolorGroup();
    
    // active top title bar tile
    tempPixmap.resize(1, TOPMARGIN);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   -100);
    aTitleBarTopTile = new TQPixmap(1, TOPMARGIN);
    painter.begin(aTitleBarTopTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();
    
    // inactive top title bar tile
    group = options()->tqcolorGroup(KDecoration::ColorTitleBar, false);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   -100);
    iTitleBarTopTile = new TQPixmap(1, TOPMARGIN);
    painter.begin(iTitleBarTopTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();
    
    // active title bar tile
    tempPixmap.resize(1, s_titleHeight+FRAMESIZE);
    group = options()->tqcolorGroup(KDecoration::ColorTitleBar, true);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   200);
    aTitleBarTile = new TQPixmap(1, s_titleHeight+FRAMESIZE);
    painter.begin(aTitleBarTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();
    
    // inactive title bar tile
    group = options()->tqcolorGroup(KDecoration::ColorTitleBar, false);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   200);
    iTitleBarTile = new TQPixmap(1, s_titleHeight+FRAMESIZE);
    painter.begin(iTitleBarTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();

    pixmaps_created = true;
}

void smoothblendClient::delete_pixmaps() {
    delete aTitleBarTopTile;
    aTitleBarTopTile = 0;

    delete iTitleBarTopTile;
    iTitleBarTopTile = 0;

    delete aTitleBarTile;
    aTitleBarTile = 0;

    delete iTitleBarTile;
    iTitleBarTile = 0;

    pixmaps_created = false;
}
//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void smoothblendClient::init() {
    createMainWidget(WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);
    handlebar = ::factory->frameSize() < 4 ? 4 - ::factory->frameSize() : 0;
    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    _resetLayout();

    create_pixmaps();
}
void smoothblendClient::_resetLayout()
{
    // basic tqlayout:
    //  _______________________________________________________________
    // |                         topSpacer                             |
    // |_______________________________________________________________|
    // | leftTitleSpacer | btns |  titlebar   | bts | rightTitleSpacer |
    // |_________________|______|_____________|_____|__________________|
    // |                         decoSpacer                            |
    // |_______________________________________________________________|
    // | |                                                           | |
    // | |                      windowWrapper                        | |
    // | |                                                           | |
    // |leftSpacer                                          rightSpacer|
    // |_|___________________________________________________________|_|
    // |                           bottomSpacer                        |
    // |_______________________________________________________________|
    //
    if (!::factory->initialized()) return;
    
    delete mainLayout_;
    delete titleLayout_;
    delete topSpacer_;
    delete titleSpacer_;
    delete leftTitleSpacer_;
    delete rightTitleSpacer_;
    delete decoSpacer_;
    delete leftSpacer_;
    delete rightSpacer_;
    delete bottomSpacer_;
    delete windowSpacer_;

    mainLayout_ = new TQVBoxLayout(widget());
    // title
    titleLayout_ = new TQHBoxLayout();
    TQHBoxLayout *windowLayout_ = new TQHBoxLayout();
    int FRAMESIZE = ::factory->frameSize();

    topSpacer_        = new TQSpacerItem(1, FRAMESIZE, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    titlebar_         = new TQSpacerItem(1, ::factory->buttonSize(),
                                        TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    leftTitleSpacer_  = new TQSpacerItem(FRAMESIZE, s_titleHeight,
                                        TQSizePolicy::Fixed, TQSizePolicy::Fixed);
    rightTitleSpacer_ = new TQSpacerItem(FRAMESIZE, s_titleHeight,
                                        TQSizePolicy::Fixed, TQSizePolicy::Fixed);
    decoSpacer_       = new TQSpacerItem(1, FRAMESIZE, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    leftSpacer_       = new TQSpacerItem(::factory->frameSize(), 1,
                                        TQSizePolicy::Fixed, TQSizePolicy::Expanding);
    rightSpacer_      = new TQSpacerItem(::factory->frameSize(), 1,
                                        TQSizePolicy::Fixed, TQSizePolicy::Expanding);
    bottomSpacer_     = new TQSpacerItem(1, ::factory->frameSize(),
                                        TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    
    // sizeof(...) is calculated at compile time
    memset(button, 0, sizeof(smoothblendButton *) * ButtonTypeCount);

    // message in preview widget
    if (isPreview()) {
        windowLayout_->addWidget(
            new TQLabel( i18n("<b><center>Smooth Blend</center></b>"), widget() ), 1 ); 
    } else {
        windowLayout_->addItem(new TQSpacerItem(0, 0));
    }

    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++)
        button[n] = 0;
    //Deal with the title and buttons
    titleLayout_->addItem(leftTitleSpacer_);
    addButtons(titleLayout_,
               options()->customButtonPositions() ? options()->titleButtonsLeft() : TQString(default_left),
               ::factory->titleSize()-1);
    titleLayout_->addItem(titlebar_);
    addButtons(titleLayout_,
               options()->customButtonPositions() ? options()->titleButtonsRight() : TQString(default_right),
               ::factory->titleSize()-1);
    titleLayout_->addItem(rightTitleSpacer_);

    //Mid - left side, middle contents and right side
    TQHBoxLayout * midLayout_   = new TQHBoxLayout();
    midLayout_->addItem(leftSpacer_);
    midLayout_->addLayout(windowLayout_);
    midLayout_->addItem(rightSpacer_);

    //Layout order
    mainLayout_->addItem( topSpacer_ );
    mainLayout_->addLayout( titleLayout_ );
    mainLayout_->addItem( decoSpacer_ );
    mainLayout_->addLayout( midLayout_ );
    mainLayout_->addItem( bottomSpacer_ );
    
    // connections
    connect(this, TQT_SIGNAL(keepAboveChanged(bool)), TQT_SLOT(keepAboveChange(bool)));
    connect(this, TQT_SIGNAL(keepBelowChanged(bool)), TQT_SLOT(keepBelowChange(bool)));
}

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title tqlayout

void smoothblendClient::addButtons(TQBoxLayout *tqlayout, const TQString& s, int button_size) {
    TQString tip;
    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
            case 'M': // Menu button
                if (!button[ButtonMenu]) {
                    button[ButtonMenu] =
                        new smoothblendButton(this, "splat.png", i18n("Menu"),ButtonMenu,button_size);
                    connect(button[ButtonMenu], TQT_SIGNAL(pressed()), this, TQT_SLOT(menuButtonPressed()));
                    connect(button[ButtonMenu], TQT_SIGNAL(released()), this, TQT_SLOT(menuButtonReleased()));
                    tqlayout->addWidget(button[ButtonMenu]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'S': // Sticky button
                if (!button[ButtonSticky]) {
                    if (isOnAllDesktops()) {
                        tip = i18n("Un-Sticky");
                    } else {
                        tip = i18n("Sticky");
                    }
                    button[ButtonSticky] =
                        new smoothblendButton(this, "circle.png", tip, ButtonSticky, button_size, true);
                    connect(button[ButtonSticky], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(toggleOnAllDesktops()));
                    tqlayout->addWidget(button[ButtonSticky]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'H': // Help button
                if ((!button[ButtonHelp]) && providesContextHelp()) {
                    button[ButtonHelp] =
                        new smoothblendButton(this, "help.png", i18n("Help"), ButtonHelp, button_size);
                    connect(button[ButtonHelp], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(showContextHelp()));
                    tqlayout->addWidget(button[ButtonHelp]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'I': // Minimize button
                if ((!button[ButtonMin]) && isMinimizable())  {
                    button[ButtonMin] =
                        new smoothblendButton(this, "minimize.png", i18n("Minimize"), ButtonMin, button_size);
                    connect(button[ButtonMin], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(minimize()));
                    tqlayout->addWidget(button[ButtonMin]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'A': // Maximize button
                if ((!button[ButtonMax]) && isMaximizable()) {
                    if (maximizeMode() == MaximizeFull) {
                        tip = i18n("Restore");
                    } else {
                        tip = i18n("Maximize");
                    }
                    button[ButtonMax]  =
                        new smoothblendButton(this, "maximize.png", tip, ButtonMax, button_size, true);
                    connect(button[ButtonMax], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(maxButtonPressed()));
                    tqlayout->addWidget(button[ButtonMax]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'X': // Close button
                if ((!button[ButtonClose]) && isCloseable()) {
                    button[ButtonClose] =
                        new smoothblendButton(this, "close.png", i18n("Close"), ButtonClose, button_size);
                    connect(button[ButtonClose], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(closeWindow()));
                    tqlayout->addWidget(button[ButtonClose]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'F': // Above button
                if ((!button[ButtonAbove])) {
                    button[ButtonAbove] =
                        new smoothblendButton(this, "keep_above.png",
                                          i18n("Keep Above Others"), ButtonAbove, button_size, true);
                    connect(button[ButtonAbove], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(aboveButtonPressed()));
                    tqlayout->addWidget(button[ButtonAbove]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'B': // Below button
                if ((!button[ButtonBelow])) {
                    button[ButtonBelow] =
                        new smoothblendButton(this, "keep_below.png",
                                          i18n("Keep Below Others"), ButtonBelow, button_size, true);
                    connect(button[ButtonBelow], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(belowButtonPressed()));
                    tqlayout->addWidget(button[ButtonBelow]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case 'L': // Shade button
                if ((!button[ButtonShade]) && isShadeable()) {
                    if ( isSetShade()) {
                        tip = i18n("Unshade");
                    } else {
                        tip = i18n("Shade");
                    }
                    button[ButtonShade] =
                        new smoothblendButton(this, "shade.png", tip, ButtonShade, button_size, true);
                    connect(button[ButtonShade], TQT_SIGNAL(clicked()),
                            this, TQT_SLOT(shadeButtonPressed()));
                    tqlayout->addWidget(button[ButtonShade]);
                    if (n < s.length()-1) tqlayout->addSpacing(1);
                }
                break;

            case '_': // Spacer item
                tqlayout->addSpacing(::factory->frameSize());
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void smoothblendClient::activeChange() {
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n])
            button[n]->reset();
    widget()->tqrepaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void smoothblendClient::captionChange() {
    widget()->tqrepaint(titlebar_->tqgeometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void smoothblendClient::desktopChange() {
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        TQToolTip::remove(button[ButtonSticky]);
        TQToolTip::add(button[ButtonSticky], d ? i18n("Un-Sticky") : i18n("Sticky"));
        button[ButtonSticky]->tqrepaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void smoothblendClient::iconChange() {
    if (button[ButtonMenu]) {
        button[ButtonMenu]->tqrepaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void smoothblendClient::maximizeChange() {
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        TQToolTip::remove(button[ButtonMax]);
        TQToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
        button[ButtonMax]->tqrepaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void smoothblendClient::shadeChange() {
    bool s = isSetShade();
    if (button[ButtonShade]) {
        TQToolTip::remove(button[ButtonShade]);
        TQToolTip::add(button[ButtonShade], s ? i18n("Unshade") : i18n("Shade"));
        button[ButtonShade]->tqrepaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// keepAboveChange()
// ------------
// The above state has changed

void smoothblendClient::keepAboveChange(bool a) {
    if (button[ButtonAbove]) {
        button[ButtonAbove]->setOn(a);
        button[ButtonAbove]->tqrepaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// keepBelowChange()
// ------------
// The below state has changed

void smoothblendClient::keepBelowChange(bool b) {
    if (button[ButtonBelow]) {
        button[ButtonBelow]->setOn(b);
        button[ButtonBelow]->tqrepaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void smoothblendClient::borders(int &left, int &right, int &top, int &bottom) const {
    int FRAMESIZE = ::factory->frameSize();

    if ((maximizeMode()==MaximizeFull) && !options()->moveResizeMaximizedWindows()) {
        left = right = bottom = 0;
        top = ::factory->buttonSize();

        // update tqlayout etc.
        topSpacer_->changeSize(1, 0, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
        decoSpacer_->changeSize(1, 0, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
        leftSpacer_->changeSize(left, 1, TQSizePolicy::Fixed, TQSizePolicy::Expanding);
        leftTitleSpacer_->changeSize(left, top, TQSizePolicy::Fixed, TQSizePolicy::Fixed);
        rightSpacer_->changeSize(right, 1, TQSizePolicy::Fixed, TQSizePolicy::Expanding);
        rightTitleSpacer_->changeSize(right, top, TQSizePolicy::Fixed, TQSizePolicy::Fixed);
        bottomSpacer_->changeSize(1, bottom, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    } else {
        left = right = bottom = ::factory->frameSize();
        top = ::factory->titleSize() + (FRAMESIZE*2);

        // update tqlayout etc.
        topSpacer_->changeSize(1, FRAMESIZE, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
        decoSpacer_->changeSize(1, FRAMESIZE, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
        leftSpacer_->changeSize(left, 1, TQSizePolicy::Fixed, TQSizePolicy::Expanding);
        leftTitleSpacer_->changeSize(left, s_titleHeight, TQSizePolicy::Fixed, TQSizePolicy::Fixed);
        rightSpacer_->changeSize(right, 1, TQSizePolicy::Fixed, TQSizePolicy::Expanding);
        rightTitleSpacer_->changeSize(right,s_titleHeight,TQSizePolicy::Fixed, TQSizePolicy::Fixed);
        bottomSpacer_->changeSize(1, bottom, TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    }
    widget()->tqlayout()->activate();
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void smoothblendClient::resize(const TQSize &size) {
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// tqminimumSize()
// -------------
// Return the minimum allowable size for this window

TQSize smoothblendClient::tqminimumSize() const {
    return widget()->tqminimumSize();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

KDecoration::Position smoothblendClient::mousePosition(const TQPoint &point) const {
    const int corner = 24;
    Position pos;
    int fs = ::factory->frameSize() + handlebar;
    //int fs = ::factory->frameSize();

    if (point.y() <= fs) {
        // inside top frame
        if (point.x() <= corner)
            pos = PositionTopLeft;
        else if (point.x() >= (width()-corner))
            pos = PositionTopRight;
        else
            pos = PositionTop;
    } else if (point.y() >= (height()-fs*2)) {
        // inside handle
        if (point.x() <= corner)
            pos = PositionBottomLeft;
        else if (point.x() >= (width()-corner))
            pos = PositionBottomRight;
        else
            pos = PositionBottom;
    } else if (point.x() <= fs ) {
        // on left frame
        if (point.y() <= corner)
            pos = PositionTopLeft;
        else if (point.y() >= (height()-corner))
            pos = PositionBottomLeft;
        else
            pos = PositionLeft;
    } else if (point.x() >= width()-fs) {
        // on right frame
        if (point.y() <= corner)
            pos = PositionTopRight;
        else if (point.y() >= (height()-corner))
            pos = PositionBottomRight;
        else
            pos = PositionRight;
    } else {
        // inside the frame
        pos = PositionCenter;
    }
    return pos;
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Event filter

bool smoothblendClient::eventFilter(TQObject *obj, TQEvent *e) {
    if (TQT_BASE_OBJECT(obj) != TQT_BASE_OBJECT(widget()))
        return false;

    switch (e->type()) {
    case TQEvent::MouseButtonDblClick: {
            mouseDoubleClickEvent(TQT_TQMOUSEEVENT(e));
            return true;
        }
    case TQEvent::MouseButtonPress: {
            processMousePressEvent(TQT_TQMOUSEEVENT(e));
            return true;
        }
    case TQEvent::Paint: {
            paintEvent(TQT_TQPAINTEVENT(e));
            return true;
        }
    case TQEvent::Resize: {
            resizeEvent(TQT_TQRESIZEEVENT(e));
            return true;
        }
    case TQEvent::Show: {
            showEvent(TQT_TQSHOWEVENT(e));
            return true;
        }
    case TQEvent::Wheel: {
            wheelEvent(TQT_TQWHEELEVENT( e ));
            return true;
        }
    default: {
            return false;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// mouseDoubleClickEvent()
// -----------------------
// Doubleclick on title

void smoothblendClient::mouseDoubleClickEvent(TQMouseEvent *e) {
    if (titlebar_->tqgeometry().contains(e->pos()))
        titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// wheelEvent()
// ------------
// Mouse wheel on titlebar

void smoothblendClient::wheelEvent(TQWheelEvent *e)
{
    if (titleLayout_->tqgeometry().contains(e->pos()) )
        titlebarMouseWheelOperation( e->delta());
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void smoothblendClient::paintEvent(TQPaintEvent*) {
    if (!::factory->initialized())
    {
        return;
    }
    
    //int FRAMESIZE = ::factory->frameSize();
    const uint maxCaptionLength = 300; // truncate captions longer than this!
    TQString captionText(caption());
    if (captionText.length() > maxCaptionLength) {
        captionText.truncate(maxCaptionLength);
        captionText.append(" [...]");
    }

    TQColor blackColor(black);
    TQColor redColor(red);
    TQColorGroup group,widgetGroup;
    TQPainter painter(widget());
    bool active = isActive();
    //get group information first
    group = options()->tqcolorGroup(KDecoration::ColorTitleBar, isActive());
    widgetGroup = widget()->tqcolorGroup();

    TQRect topRect( topSpacer_->tqgeometry() );
    TQRect titleRect( titleLayout_->tqgeometry() );
    TQRect textRect( titlebar_->tqgeometry() );
    TQRect Rltitle( leftTitleSpacer_->tqgeometry() );
    TQRect Rrtitle( rightTitleSpacer_->tqgeometry() );
    TQRect Rdeco( decoSpacer_->tqgeometry() );
    TQRect Rleft( leftSpacer_->tqgeometry() );
    TQRect Rright( rightSpacer_->tqgeometry() );
    TQRect Rbottom( bottomSpacer_->tqgeometry() );
    TQRect tempRect;
    
    
    /*
    if(active)
    {
        qDebug("paintEvent() topRect.y()   = %i\tbottom() = %i",topRect.top(),topRect.bottom());
        qDebug("paintEvent() titleRect.y() = %i\tbottom() = %i",titleRect.top(),titleRect.bottom());
        qDebug("paintEvent() textRect.y()  = %i\tbottom() = %i",textRect.top(),textRect.bottom());
        qDebug("paintEvent() Rltitle.y()   = %i\tbottom() = %i",Rltitle.top(),Rltitle.bottom());
        qDebug("paintEvent() Rrtitle.y()   = %i\tbottom() = %i",Rrtitle.top(),Rrtitle.bottom());
        qDebug("paintEvent() Rdeco.y()     = %i\tbottom() = %i",Rdeco.top(),Rdeco.bottom());
        qDebug("paintEvent() Rleft.y()     = %i\tbottom() = %i",Rleft.top(),Rleft.bottom());
        qDebug("paintEvent() Rright.y()    = %i\tbottom() = %i",Rright.top(),Rright.bottom());
        qDebug("paintEvent() Rbottom.y()   = %i\tbottom() = %i",Rbottom.top(),Rbottom.bottom());
    }
    */

    // top
    painter.drawTiledPixmap(topRect, active ? *aTitleBarTopTile:*iTitleBarTopTile);
    painter.drawTiledPixmap(titleRect.x(),
                            titleRect.y(),
                            titleRect.width(),
                            titleRect.height() + Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);

    textRect.setRect(textRect.x()+SIDETITLEMARGIN,
                     textRect.y(),
                     textRect.width()-SIDETITLEMARGIN*2,
                     textRect.height());
    TQRect shadowRect(textRect.x()+1,textRect.y()+1,textRect.width(),textRect.height());
    //if we are shadowing title bar text
    if(::factory->titleShadow())
    {
        //shadow text
        painter.setFont(options()->font(isActive(), false));
        painter.setPen(blackColor);
        painter.drawText(shadowRect,
                         ::factory->titleAlign() | AlignVCenter | TQt::SingleLine,
                         captionText);
    }
    // draw title text
    painter.setFont(options()->font(isActive(), false));
    painter.setPen(options()->color(KDecoration::ColorFont, isActive()));
    painter.drawText(textRect,
                     ::factory->titleAlign() | AlignVCenter | TQt::SingleLine,
                     captionText);
    
    //left of buttons and title
    painter.drawTiledPixmap(Rltitle.x(),
                            Rltitle.y(),
                            Rltitle.width(),
                            Rltitle.height()+Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);
    // left mid tqlayout
    painter.fillRect(Rleft,widgetGroup.background());

    // right of buttons and title
    painter.drawTiledPixmap(Rrtitle.x(),
                            Rrtitle.y(),
                            Rrtitle.width(),
                            Rrtitle.height()+Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);
    // right mid tqlayout
    painter.fillRect(Rright,widgetGroup.background());
    
    // bottom
    /*
    if(isShade())
    {
        frame.setRect(0,::factory->titleSize()+FRAMESIZE, width(), FRAMESIZE);
    }
    else
    {
        frame.setRect(0, height() - (FRAMESIZE*2), width(),  (FRAMESIZE*2));
    }
    */
    painter.fillRect(Rbottom, widgetGroup.background());

    //draw a line between title bar and window contents
    painter.setPen(group.background());

    // outline outside the frame but not the corners if they are rounded
    tempRect = widget()->rect();
    painter.drawRect(tempRect);

    bool cornersFlag = ::factory->roundedCorners();
    if(cornersFlag) {
        // local temp right and bottom
        int r(width());
        painter.setPen(group.background());
        painter.drawPoint(4, 1);
        painter.drawPoint(3, 1);
        painter.drawPoint(2, 2);
        painter.drawPoint(1, 3);
        painter.drawPoint(1, 4);
        painter.drawPoint(r - 5, 1);
        painter.drawPoint(r - 4, 1);
        painter.drawPoint(r - 3, 2);
        painter.drawPoint(r - 2, 3);
        painter.drawPoint(r - 2, 4);
    } 

}

//////////////////////////////////////////////////////////////////////////////
// updateMask()
// ------------
// update the frame mask
void smoothblendClient::updateMask() {
    bool cornersFlag = ::factory->roundedCorners();
    if ( (!options()->moveResizeMaximizedWindows() && maximizeMode() == MaximizeFull ) ) 
    {
        setMask(TQRegion(widget()->rect()));
        return;
    }

    int r(width());
    int b(height());
    TQRegion mask;

    mask=TQRegion(widget()->rect());

   // Remove top-left corner.
    if(cornersFlag) {
        mask -= TQRegion(0, 0, 5, 1);
        mask -= TQRegion(0, 1, 3, 1);
        mask -= TQRegion(0, 2, 2, 1);
        mask -= TQRegion(0, 3, 1, 2);
        mask -= TQRegion(r - 5, 0, 5, 1);
        mask -= TQRegion(r - 3, 1, 3, 1);
        mask -= TQRegion(r - 2, 2, 2, 1);
        mask -= TQRegion(r - 1, 3, 1, 2);
    }
    //always remove one corner pixel so it simulates a soft corner like plastik
    mask -= TQRegion(0,0,1,1);
    mask -= TQRegion(r-1,0,1,1);
    mask -= TQRegion(0, b-1, 1,1);
    mask -= TQRegion(r-1,b-1,1,1);

    setMask(mask);
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void smoothblendClient::resizeEvent(TQResizeEvent *) {
    if (widget()->isShown()) {
        TQRegion region = widget()->rect();
        region = region.subtract(titlebar_->tqgeometry());
        widget()->erase(region);
        updateMask();
    }
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void smoothblendClient::showEvent(TQShowEvent *) {
    updateMask();
    widget()->tqrepaint();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void smoothblendClient::maxButtonPressed() {
    if (button[ButtonMax]) {
#if KDE_IS_VERSION(3, 3, 0)
        maximize(button[ButtonMax]->lastMousePress());
#else

        switch (button[ButtonMax]->lastMousePress()) {
        case MidButton:
            maximize(maximizeMode() ^ MaximizeVertical);
            break;
        case RightButton:
            maximize(maximizeMode() ^ MaximizeHorizontal);
            break;
        default:
            (maximizeMode() == MaximizeFull) ? maximize(MaximizeRestore)
            : maximize(MaximizeFull);
        }
#endif

    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeButtonPressed()
// -----------------
// Shade button was pressed

void smoothblendClient::shadeButtonPressed() {
    if (button[ButtonShade]) {
        setShade( !isSetShade());
    }
}

//////////////////////////////////////////////////////////////////////////////
// aboveButtonPressed()
// -----------------
// Above button was pressed

void smoothblendClient::aboveButtonPressed() {
    if (button[ButtonAbove]) {
        setKeepAbove( !keepAbove());
    }
}

//////////////////////////////////////////////////////////////////////////////
// belowButtonPressed()
// -----------------
// Below button was pressed

void smoothblendClient::belowButtonPressed() {
    if (button[ButtonBelow]) {
        setKeepBelow( !keepBelow());
    }
}

//////////////////////////////////////////////////////////////////////////////
// menuButtonPressed()
// -------------------
// Menu button was pressed (popup the menu)

void smoothblendClient::menuButtonPressed() {
    static TQTime* t = NULL;
    static smoothblendClient* lastClient = NULL;
    if (t == NULL)
        t = new TQTime;
    bool dbl = (lastClient==this && t->elapsed() <= TQApplication::doubleClickInterval());
    lastClient = this;
    t->start();
    //if (button[ButtonMenu] && !dbl && !::factory->menuClosed()) {
    if ( !dbl || !::factory->menuClosed()) {
        TQPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y());
        KDecorationFactory* f = factory();
        showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
        if (!f->exists(this))
            return; // decoration was destroyed
        button[ButtonMenu]->setDown(false);
    }
    else
    {
        closing = true;
    }
}

void smoothblendClient::menuButtonReleased()
{
    if(closing)
    {
        closeWindow();
    }
}

#include "smoothblend.moc"
