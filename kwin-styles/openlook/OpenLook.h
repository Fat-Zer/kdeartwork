/*
  'OpenLook' kwin client

  Porting to kde3.2 API 
    Copyright 2003 Luciano Montanaro <mikelima@cirulla.net>
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef KWIN_WEB_H
#define KWIN_WEB_H

#include <tqptrlist.h>
#include <tqbutton.h>

#include <kdecoration.h>
#include <kdecorationfactory.h>

class TQSpacerItem;

namespace OpenLook
{
  class OpenLookButton;

  class OpenLook : public KDecoration
  {
    Q_OBJECT
  TQ_OBJECT
    public:

      OpenLook(KDecorationBridge *b, KDecorationFactory *f);
      ~OpenLook();
      void init();

    protected:
      bool eventFilter(TQObject *o, TQEvent *e);
      void resizeEvent(TQResizeEvent *e);
      void paintEvent(TQPaintEvent *e);
      void showEvent(TQShowEvent *e);

      virtual void captionChange();
      void desktopChange();
      void activeChange();
      void shadeChange();
      void iconChange();
      void maximizeChange();
      void borders(int &left, int &right, int &top, int &bottom) const;
      TQSize tqminimumSize() const;
      void resize( const TQSize& );
      virtual void mouseDoubleClickEvent(TQMouseEvent *);
      virtual void wheelEvent(TQWheelEvent *e);

      virtual Position mousePosition(const TQPoint &) const;
      virtual bool animateMinimize(bool);

    private:

      void doLayout();

      TQRect titleRect() const;

      TQRect topLeftRect() const;
      TQRect topRightRect() const;
      TQRect bottomLeftRect() const;
      TQRect bottomRightRect() const;

      TQRect buttonRect() const;

      void paintBorder(TQPainter &) const;

      void paintTopLeftRect(TQPainter &) const;
      void paintTopRightRect(TQPainter &) const;
      void paintBottomLeftRect(TQPainter &) const;
      void paintBottomRightRect(TQPainter &) const;

      void paintButton(TQPainter &) const;
      void paintArrow(TQPainter &) const;

      bool isButtonPress(TQMouseEvent *);
      bool isButtonRelease(TQMouseEvent *);

      TQSpacerItem   * titleSpacer_;
      TQPoint          mousePressPoint_;
      bool            tool_;
      bool            buttonDown_;
  };
  
  class DecorationFactory: public TQObject, public KDecorationFactory
  {
  public:
    DecorationFactory();
    virtual ~DecorationFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *);
    virtual bool reset(unsigned long changed);
    virtual bool supports( Ability ability );
    TQValueList< DecorationFactory::BorderSize > borderSizes() const;
  };
}

#endif
// vim:ts=2:sw=2:tw=78:set et:
