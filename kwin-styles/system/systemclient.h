#ifndef __SYSTEMCLIENT_H
#define __SYSTEMCLIENT_H

#include <tqvariant.h>
#include <tqbitmap.h>
#include <tqbutton.h>

#include <kpixmap.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>


class TQLabel;
class TQSpacerItem;
class TQBoxLayout;

namespace System {

class SystemButton;

enum ButtonType {
    ButtonClose=0,
    ButtonSticky,
    ButtonMinimize,
    ButtonMaximize,
    ButtonHelp,
    ButtonTypeCount
};

class SystemClient : public KDecoration
{
   Q_OBJECT
   public:
      SystemClient(KDecorationBridge* bridge, KDecorationFactory* factory);
      ~SystemClient();
      virtual Position mousePosition(const TQPoint& p) const;
      virtual void resize(const TQSize&);
      virtual bool eventFilter(TQObject* o, TQEvent* e);
      virtual void init();
   protected:
      virtual void maximizeChange();
      virtual void captionChange();
      virtual void shadeChange() {};
      virtual void iconChange();
      virtual void desktopChange();
      virtual void activeChange();
      virtual TQSize minimumSize() const;
      virtual void borders(int&, int&, int&, int&) const;
      virtual void reset( unsigned long changed );
      void drawRoundFrame(TQPainter &p, int x, int y, int w, int h);
      void resizeEvent( TQResizeEvent* );
      void paintEvent( TQPaintEvent* );
      void showEvent( TQShowEvent* );
      void mouseDoubleClickEvent( TQMouseEvent * );
      void wheelEvent(TQWheelEvent *e);
      void doShape();
      void recalcTitleBuffer();
   private:
      void addButtons(TQBoxLayout* hb, const TQString& buttons);
   private slots:
      void maxButtonClicked();

   private:
      SystemButton* button[ButtonTypeCount];
      TQSpacerItem* titlebar;
      TQPixmap titleBuffer;
      TQString oldTitle;
};

class SystemButton : public QButton
{
   public:
      SystemButton(SystemClient *parent=0, const char *name=0,
                   const unsigned char *bitmap=NULL, const TQString& tip=NULL);
      void setBitmap(const unsigned char *bitmap);
      void reset();
      TQSize sizeHint() const;
      void setTipText(const TQString &tip);
      ButtonState last_button;
   protected:
      virtual void drawButton(TQPainter *p);
      void drawButtonLabel(TQPainter *){}
      TQBitmap deco;

      void mousePressEvent( TQMouseEvent* e );
      void mouseReleaseEvent( TQMouseEvent* e );

   private:
      SystemClient* client;
};


class SystemDecoFactory : public TQObject, public KDecorationFactory
{
   Q_OBJECT
   public:
      SystemDecoFactory();
      virtual ~SystemDecoFactory();
      virtual KDecoration *createDecoration(KDecorationBridge *);
      virtual bool reset(unsigned long);
      virtual bool supports( Ability ability );
      virtual TQValueList< BorderSize > borderSizes() const;
   private:
      void readConfig();
};



}

#endif
