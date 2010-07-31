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

#include "config.h"
#include <unistd.h> // for usleep
#include <math.h>

#include <tqlayout.h>
#include <tqpainter.h>
#include <tqdrawutil.h>
#include <tqtoolbutton.h>
#include <tqimage.h>
#include <tqlabel.h>
#include <tqpixmap.h>

#include <kapplication.h>
#include <klocale.h>

#include "OpenLook.h"

extern "C" KDE_EXPORT KDecorationFactory* create_factory()
{
    return new OpenLook::DecorationFactory();
}

namespace OpenLook {

static uint openLookCornerSize = 11;
static uint openLookMargin = 5;
static const uint openLookTextVMargin = 1;
static uint titleHeight;

// --------------------------------------- 

DecorationFactory::DecorationFactory()
{
}

DecorationFactory::~DecorationFactory()
{
}

KDecoration *DecorationFactory::createDecoration(KDecorationBridge *b)
{
    return new OpenLook(b, this);
}

bool DecorationFactory::reset(unsigned long /*changed*/)
{
    // TODO Do not recreate decorations if it is not needed. Look at
    // ModernSystem for how to do that
    // For now just return true.
    return true;
}

bool DecorationFactory::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonMinimize:
            return true;
        default:
            return false;
    };
}

TQValueList< DecorationFactory::BorderSize > 
DecorationFactory::borderSizes() const
{ 
    // the list must be sorted
    return TQValueList< BorderSize >() << BorderNormal << 
	BorderLarge << BorderVeryLarge <<  BorderHuge << 
	BorderVeryHuge << BorderOversized;
}

// --------------------------------------- 

static inline const KDecorationOptions* options()
{
    return KDecoration::options();
}

// --------------------------------------- 

OpenLook::OpenLook(KDecorationBridge *b, KDecorationFactory *f) : 
    KDecoration(b, f), 
    titleSpacer_    (0),
    mousePressPoint_(0, 0), // Anywhere outside button rect.
    buttonDown_     (false)
{
}

void OpenLook::init()
{
  static const int SUPPORTED_WINDOW_TYPES_MASK = NET::NormalMask | 
    NET::DesktopMask | NET::DockMask | NET::ToolbarMask | NET::MenuMask | 
    NET::DialogMask | NET::OverrideMask | NET::TopMenuMask | 
    NET::UtilityMask | NET::SplashMask;

  createMainWidget(WStaticContents | WResizeNoErase | WRepaintNoErase);
  widget()->installEventFilter(this);
  widget()->setBackgroundMode(NoBackground);
  NET::WindowType type = windowType(SUPPORTED_WINDOW_TYPES_MASK);
  tool_ = (type == NET::Toolbar || type == NET::Utility || type == NET::Menu);
  TQFontMetrics fm(options()->font(isActive(), tool_));

  titleHeight = fm.height() + openLookTextVMargin * 2;
  
  switch(KDecoration::options()->preferredBorderSize(factory())) {
      case KDecoration::BorderLarge:
          openLookMargin = 9;
          break;
      case KDecoration::BorderVeryLarge:
          openLookMargin = 13;
          break;
      case KDecoration::BorderHuge:
          openLookMargin = 18;
          break;
      case KDecoration::BorderVeryHuge:
          openLookMargin = 27;
          break;
      case KDecoration::BorderOversized:
          openLookMargin = 40;
          break;
      case KDecoration::BorderNormal:
      default:
          openLookMargin = 5;
  }
  openLookCornerSize = 2*openLookMargin + 1;
  if (titleHeight <= openLookMargin)
      titleHeight = openLookMargin+1;
  
  doLayout();
}

OpenLook::~OpenLook()
{
  // Empty.
}

bool OpenLook::eventFilter(TQObject *o, TQEvent *e)
{
  if (o != widget()) return false;
  switch (e->type()) {
  case TQEvent::Resize:
    resizeEvent(static_cast< TQResizeEvent* >(e));
    return true;
  case TQEvent::Paint:
    paintEvent(static_cast< TQPaintEvent* >(e));
    return true;
  case TQEvent::MouseButtonDblClick:
    mouseDoubleClickEvent(static_cast< TQMouseEvent* >(e));
    return true;
  case TQEvent::Wheel:
    wheelEvent( static_cast< TQWheelEvent* >( e ));
    return true;
  case TQEvent::MouseButtonPress:
    if (!isButtonPress(static_cast< TQMouseEvent* >(e))) {
      processMousePressEvent(static_cast< TQMouseEvent* >(e));
    }
    return true;
  case TQEvent::MouseButtonRelease:
    if (isButtonRelease(static_cast< TQMouseEvent* >(e))) {
      return true;
    } else {
      return false;
    }
  case TQEvent::Show:
    showEvent(static_cast< TQShowEvent* >(e));
    return true;
  default:
    break;
  }
  return false;
}

  void 
OpenLook::borders(int &left, int &right, int &top, int &bottom) const
{
    // FRAME XXX Check
    left = right = bottom = openLookMargin;
    top = titleHeight + openLookMargin;
}

  void
OpenLook::captionChange()
{
  widget()->update(titleRect());
}

  void 
OpenLook::shadeChange()
{
}

TQSize OpenLook::minimumSize() const
{
    int left, right, top, bottom;
    borders(left, right, top, bottom);
    return TQSize(left  + right + 2 * titleHeight, top + bottom); 
}

  void 
OpenLook::resize(const TQSize& s)
{
    widget()->resize(s);
    widget()->repaint(); //there is some strange wrong repaint of the frame without
}

  void
OpenLook::paintEvent(TQPaintEvent * pe)
{
  TQRect tr(titleRect());

  TQPainter p(widget());

  TQRegion clipRegion(pe->region());

  p.setClipRegion(clipRegion);

  paintBorder(p);

  paintTopLeftRect(p);
  paintTopRightRect(p);
  paintBottomLeftRect(p);
  paintBottomRightRect(p);

  p.setClipRegion(clipRegion + buttonRect());

  TQBrush titleBackground(options()->color(KDecoration::ColorTitleBar, true));

  if (isActive())
    qDrawShadePanel(&p, tr, widget()->colorGroup(), true, 1, &titleBackground);
  else
    p.fillRect(tr, widget()->colorGroup().brush(TQColorGroup::Background));

  p.setClipRegion(clipRegion);

  paintButton(p);

  p.setFont(options()->font(isActive(), tool_));

  p.setPen(options()->color(KDecoration::ColorFont, isActive()));

  tr.setLeft(openLookCornerSize + 3 + buttonRect().width() + 2);

  p.drawText(tr, AlignCenter, caption());
}

  void
OpenLook::showEvent(TQShowEvent *)
{
    widget()->repaint();
}

  void
OpenLook::mouseDoubleClickEvent(TQMouseEvent * e)
{
  if (e->button() == LeftButton && titleRect().contains(e->pos()))
  {
	  titlebarDblClickOperation();
  }
}

  void
OpenLook::wheelEvent(TQWheelEvent *e)
{
  if (isSetShade() || titleRect().contains(e->pos()))
  {
          titlebarMouseWheelOperation( e->delta());
  }
}

  void 
OpenLook::resizeEvent(TQResizeEvent* e)
{
  widget()->update();
}

  void
OpenLook::activeChange()
{
  widget()->repaint();
}

  KDecoration::Position
OpenLook::mousePosition(const TQPoint & p) const
{
  if (topLeftRect().contains(p))
    return PositionTopLeft;
  else if (topRightRect().contains(p))
    return PositionTopRight;
  else if (bottomLeftRect().contains(p))
    return PositionBottomLeft;
  else if (bottomRightRect().contains(p))
    return PositionBottomRight;
  else
    return PositionCenter;
}

  void 
OpenLook::iconChange()
{
}

  void 
OpenLook::desktopChange()
{
}

  void 
OpenLook::maximizeChange()
{
    widget()->repaint(false);
}

  void
OpenLook::doLayout()
{
  TQVBoxLayout * layout = new TQVBoxLayout(widget(), openLookMargin);

  titleSpacer_ =
    new QSpacerItem
    (
     0,
     titleHeight,
     TQSizePolicy::Expanding,
     TQSizePolicy::Fixed
    );

  layout->addItem(titleSpacer_);

  layout->addSpacing(2);

  TQBoxLayout * midLayout = 
    new TQBoxLayout(layout, TQBoxLayout::LeftToRight, 0, 0);
 
  if (isPreview()) {
    midLayout->addWidget(new TQLabel(
          i18n("<center><b>OpenLook preview</b></center>"), 
          widget()), 
        1);
  } else {
    midLayout->addItem( new TQSpacerItem( 0, 0 ));
  }
      
}

  bool
OpenLook::animateMinimize(bool /*iconify*/)
{
#if 0
  TQRect icongeom(iconGeometry());

  if (!icongeom.isValid())
    return false;

  TQRect wingeom(geometry());

  TQPainter p(workspaceWidget());

  p.setRasterOp(Qt::NotROP);

#if 0
  if (iconify)
    p.setClipRegion(
      TQRegion(workspace()->desktopWidget()->rect()) - wingeom
      );
#endif

  for (uint count = 0; count < 4; count++)
  {
    grabXServer();

    p.drawLine(wingeom.bottomRight(), icongeom.bottomRight());
    p.drawLine(wingeom.bottomLeft(), icongeom.bottomLeft());
    p.drawLine(wingeom.topLeft(), icongeom.topLeft());
    p.drawLine(wingeom.topRight(), icongeom.topRight());

    p.flush();

    kapp->syncX();

    usleep(10000);

    p.drawLine(wingeom.bottomRight(), icongeom.bottomRight());
    p.drawLine(wingeom.bottomLeft(), icongeom.bottomLeft());
    p.drawLine(wingeom.topLeft(), icongeom.topLeft());
    p.drawLine(wingeom.topRight(), icongeom.topRight());

    p.flush();

    kapp->syncX();

    usleep(10000);

    ungrabXServer();
  }
#endif
  return true;
}

  QRect
OpenLook::topLeftRect() const
{
  return QRect
    (
     0,
     0,
     openLookCornerSize,
     openLookCornerSize
    );
}

  QRect
OpenLook::topRightRect() const
{
  return QRect
    (
     width() - openLookCornerSize,
     0,
     openLookCornerSize,
     openLookCornerSize
    );
}

  QRect
OpenLook::bottomLeftRect() const
{
  return QRect
    (
     0,
     height() - openLookCornerSize,
     openLookCornerSize,
     openLookCornerSize
    );
}

  QRect
OpenLook::bottomRightRect() const
{
  return QRect
    (
     width() - openLookCornerSize,
     height() - openLookCornerSize,
     openLookCornerSize,
     openLookCornerSize
    );
}

  void
OpenLook::paintTopLeftRect(TQPainter & p) const
{
  TQColor handleColour(options()->color(KDecoration::ColorHandle, isActive()));

  TQRect r(topLeftRect());

  int x1(r.left());
  int y1(r.top());
  int x2(r.right());
  int y2(r.bottom());

  p.setPen(widget()->colorGroup().light());

  p.drawLine(x1, y1, x2, y1);
  p.drawLine(x1, y1 + 1, x1, y2);

  p.fillRect(x1 + 1, y1 + 1, r.width()-2, openLookMargin-2,  handleColour);
  p.fillRect(x1 + 1, y1 + 1, openLookMargin-2, r.height()-2, handleColour);

  p.setPen(widget()->colorGroup().dark());

  p.drawLine(x2, y1 + 1, x2, y1 + openLookMargin-1);

  p.drawLine(x1 + openLookMargin-1, y1 + openLookMargin-1, x2 - 1, y1 + openLookMargin-1);

  p.drawLine(x1 + openLookMargin-1, y1 + openLookMargin, x1 + openLookMargin-1, y2 - 1);

  p.drawLine(x1 + 1, y2, x1 + openLookMargin-1, y2);
}

  void
OpenLook::paintTopRightRect(TQPainter & p) const
{
  TQColor handleColour(options()->color(KDecoration::ColorHandle, isActive()));

  TQRect r(topRightRect());

  int x1(r.left());
  int y1(r.top());
  int x2(r.right());
  int y2(r.bottom());

  p.setPen(widget()->colorGroup().light());

  p.drawLine(x1, y1, x2, y1);
  p.drawLine(x1, y1 + 1, x1, y1 + openLookMargin-1);
  p.drawLine(x2 - openLookMargin+1, y1 + openLookMargin, x2 - openLookMargin+1, y2);

  p.fillRect(x1 + 1, y1 + 1, r.width()-2, openLookMargin-2, handleColour);
  p.fillRect(x2 - openLookMargin + 2, y1 + 1, openLookMargin-2, r.height()-2, handleColour);

  p.setPen(widget()->colorGroup().dark());

  p.drawLine(x1 + 1, y1 + openLookMargin-1, x2 - openLookMargin+1, y1 + openLookMargin-1);
  p.drawLine(x2, y1 + 1, x2, y2);
  p.drawLine(x2 - openLookMargin+1, y2, x2 - 1, y2);
}

  void
OpenLook::paintBottomLeftRect(TQPainter & p) const
{
  TQColor handleColour(options()->color(KDecoration::ColorHandle, isActive()));

  TQRect r(bottomLeftRect());

  int x1(r.left());
  int y1(r.top());
  int x2(r.right());
  int y2(r.bottom());

  p.setPen(widget()->colorGroup().light());

  p.drawLine(x1, y1, x1 + openLookMargin-1, y1);
  p.drawLine(x1, y1 + 1, x1, y2);
  p.drawLine(x1 + openLookMargin, y2 - openLookMargin+1, x2, y2 - openLookMargin+1);

  p.fillRect(x1 + 1, y2 - openLookMargin + 2, r.width()-2, openLookMargin-2, handleColour);
  p.fillRect(x1 + 1, y1 + 1, openLookMargin-2, r.height()-2, handleColour);

  p.setPen(widget()->colorGroup().dark());

  p.drawLine(x1 + openLookMargin-1, y1 + 1, x1 + openLookMargin-1, y2 - openLookMargin);
  p.drawLine(x1 + 1, y2, x2, y2);
  p.drawLine(x2, y2 - openLookMargin+2, x2, y2 - 1);
}

  void
OpenLook::paintBottomRightRect(TQPainter & p) const
{
  TQColor handleColour(options()->color(KDecoration::ColorHandle, isActive()));

  TQRect r(bottomRightRect());

  int x1(r.left());
  int y1(r.top());
  int x2(r.right());
  int y2(r.bottom());

  p.setPen(widget()->colorGroup().light());

  p.drawLine(x1, y2 - openLookMargin+1, x1, y2);
  p.drawLine(x1 + 1, y2 - openLookMargin+1, x2 - openLookMargin+1, y2 - openLookMargin+1);
  p.drawLine(x2 - openLookMargin+1, y1 + 1, x2 - openLookMargin+1, y2 - openLookMargin);
  p.drawLine(x2 - openLookMargin+1, y1, x2, y1);

  p.fillRect(x1 + 1, y2 - openLookMargin + 2, r.width()-2, openLookMargin-2,  handleColour);
  p.fillRect(x2 - openLookMargin + 2, y1 + 1, openLookMargin-2, r.height()-2, handleColour);

  p.setPen(widget()->colorGroup().dark());

  p.drawLine(x1 + 1, y2, x2, y2);
  p.drawLine(x2, y1 + 1, x2, y2 - 1);
}

  QRect
OpenLook::buttonRect() const
{
  return QRect
    (
      openLookCornerSize + 3,
      titleRect().top(),
      titleRect().height(),
      titleRect().height()
    );
}

  void
OpenLook::paintButton(TQPainter & p) const
{
  TQRect r(buttonRect());

  p.fillRect
    (
     r.left() + 1,
     r.top() + 1,
     r.width() - 2,
     r.height() - 2,
     buttonDown_
     ? widget()->colorGroup().dark()
     : options()->color(KDecoration::ColorButtonBg, isActive())
    );

  p.setPen(buttonDown_ ? widget()->colorGroup().dark() : widget()->colorGroup().light());

  p.drawLine(r.left() + 1, r.top(), r.right() - 1, r.top());
  p.drawLine(r.left(), r.top() + 1, r.left(), r.bottom() - 1);

  p.setPen(buttonDown_ ? widget()->colorGroup().light() : widget()->colorGroup().dark());

  p.drawLine(r.right(), r.top() + 1, r.right(), r.bottom() - 1);
  p.drawLine(r.left() + 1, r.bottom(), r.right() - 1, r.bottom());

  paintArrow(p);
}

  void
OpenLook::paintArrow(TQPainter & p) const
{
  TQRect br(buttonRect());

  int x = br.left()   + 5;
  int y = br.top()    + 5;
  int w = br.width()  - 10;
  int h = br.height() - 10;

  TQPointArray poly(3);

  p.setBrush(widget()->colorGroup().mid());

  poly.setPoint(0, x, y);
  poly.setPoint(1, x + w - 1, y);
  poly.setPoint(2, x + (w / 2), y + h - 1);

  p.drawPolygon(poly);

  p.setPen(widget()->colorGroup().dark());

  p.drawLine(x, y, x + w - 1, y);

  p.drawLine(x, y, x + (w / 2), y + h - 1);

  p.setPen(widget()->colorGroup().light());

  p.drawLine(x + (w / 2), y + h - 1, x + w - 1, y);
}

  void
OpenLook::paintBorder(TQPainter & p) const
{
  const uint cs(openLookCornerSize);

  uint x = widget()->rect().left();
  uint y = widget()->rect().top();
  uint w = widget()->rect().width();
  uint h = widget()->rect().height();
  uint r = widget()->rect().right();
  uint b = widget()->rect().bottom();

  p.fillRect(x + cs, y, w - cs - cs, 2,     widget()->colorGroup().shadow());
  p.fillRect(x + cs, b - 1, w - cs - cs, 2, widget()->colorGroup().shadow());
  p.fillRect(x, y + cs, 2, h - cs - cs,     widget()->colorGroup().shadow());
  p.fillRect(r - 1, y + cs, 2, h - cs - cs, widget()->colorGroup().shadow());

  TQColor frameColour(options()->color(KDecoration::ColorFrame, isActive()));

  p.fillRect(x + cs, y + 2, w - cs - cs, openLookMargin-2, frameColour);
  p.fillRect(x + cs, b - openLookMargin + 1, w - cs - cs, openLookMargin-2, frameColour);
  p.fillRect(x + 2, y + cs, openLookMargin-2, h - cs - cs, frameColour);
  p.fillRect(r - openLookMargin + 1, y + cs, openLookMargin-2, h - cs - cs, frameColour);

  p.fillRect
    (
     openLookMargin,
     titleRect().bottom() + 1,
     width() - 2 * openLookMargin,
     2,
     widget()->colorGroup().background()
    );
}

  QRect
OpenLook::titleRect() const
{
  return titleSpacer_->geometry();
}

  bool
OpenLook::isButtonPress(TQMouseEvent * e)
{
  mousePressPoint_ = e->pos();

  buttonDown_ = buttonRect().contains(mousePressPoint_);

  widget()->repaint(buttonRect());
  return buttonDown_;
}

  bool
OpenLook::isButtonRelease(TQMouseEvent * e)
{
  if (buttonDown_ && buttonRect().contains(e->pos()))
  {
    minimize();
    return true;
  }
  buttonDown_ = false;
  widget()->repaint(buttonRect());

  return false;
}

}

#include "OpenLook.moc"
// vim:ts=2:sw=2:tw=78:set et:
