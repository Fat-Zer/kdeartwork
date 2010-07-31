#ifndef __NEXTCLIENT_H
#define __NEXTCLIENT_H

#include <tqvariant.h>
#include <tqbitmap.h>
#include <kpixmap.h>
#include <tqlayout.h>
#include <tqbutton.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QLabel;
class QSpacerItem;

namespace KStep {

class NextClient;

class NextButton : public QButton
{
public:
    NextButton(NextClient *parent=0, const char *name=0,
               const unsigned char *bitmap=NULL, int bw=0, int bh=0,
               const TQString& tip=NULL, const int realizeBtns = LeftButton);
    void setBitmap(const unsigned char *bitmap, int bw, int bh);
    void reset();
    ButtonState lastButton() { return last_button; }

protected:
    void mousePressEvent( TQMouseEvent* e );
    void mouseReleaseEvent( TQMouseEvent* e );
    virtual void drawButton(TQPainter *p);
    void drawButtonLabel(TQPainter *){;}

    KPixmap aBackground, iBackground;
    TQBitmap* deco;
    NextClient *client;
    ButtonState last_button;
    int realizeButtons;
};

class NextClient : public KDecoration
{
    Q_OBJECT
public:
    NextClient(KDecorationBridge *b, KDecorationFactory *f);
    ~NextClient() {;}
    void init();
    virtual bool drawbound(const TQRect& geom, bool clear);
protected:
    bool eventFilter(TQObject *o, TQEvent *e);
    void resizeEvent( TQResizeEvent* );
    void paintEvent( TQPaintEvent* );
    void showEvent( TQShowEvent* );

    void mouseDoubleClickEvent( TQMouseEvent * );
    void wheelEvent( TQWheelEvent * );
    void captionChange();
    void desktopChange();
    void activeChange();
    void shadeChange();
    void iconChange();
    TQSize minimumSize() const;
    void resize(const TQSize &size);
    void borders(int &left, int &right, int &top, int &bottom) const;
    void reset(unsigned long changed);
    void calcHiddenButtons();
    void updateActiveBuffer();

    Position mousePosition(const TQPoint &) const;
    void maximizeChange();

protected slots:
    void slotReset();
    void menuButtonPressed();
    void maximizeButtonClicked();
    void shadeClicked();
    void aboveClicked();
    void belowClicked();
    void resizePressed();

    void keepAboveChange(bool above);
    void keepBelowChange(bool below);

private:
    void initializeButtonsAndTitlebar(TQBoxLayout* titleLayout);
    void addButtons(TQBoxLayout* titleLayout, const TQString& buttons);
    bool mustDrawHandle() const;

    TQSpacerItem* titlebar;

    // Helpful constants for buttons in array
    enum { CLOSE_IDX = 0,
           HELP_IDX,
           ICONIFY_IDX,
           MAXIMIZE_IDX,
           MENU_IDX,
           SHADE_IDX,
           ABOVE_IDX,
           BELOW_IDX,
           RESIZE_IDX,
           STICKY_IDX,
           MAX_NUM_BUTTONS = STICKY_IDX + 1 };

    // WARNING: button[i] may be null for any given i.  Make sure you
    // always check for null before doing button[i]->foo().
    NextButton* button[MAX_NUM_BUTTONS];
};

class NextClientFactory: public TQObject, public KDecorationFactory
{
public:
    NextClientFactory();
    virtual ~NextClientFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *);
    virtual bool reset(unsigned long changed);
    virtual bool supports( Ability ability );

    TQValueList< NextClientFactory::BorderSize > borderSizes() const;

};

}

#endif
