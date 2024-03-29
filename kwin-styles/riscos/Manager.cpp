/*
  RISC OS KWin client

  Copyright 2000
    Rik Hemsley <rik@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include <unistd.h> // for usleep
#include <config.h> // for usleep on non-linux platforms
#include <math.h> // for sin and cos

#include <tqapplication.h>
#include <tqimage.h>
#include <tqlabel.h>
#include <tqlayout.h>
#include <tqpainter.h>

#include <netwm.h>

#include "Manager.h"
#include "Static.h"
#include "AboveButton.h"
#include "CloseButton.h"
#include "HelpButton.h"
#include "IconifyButton.h"
#include "LowerButton.h"
#include "MaximiseButton.h"
#include "StickyButton.h"

extern "C"
{
   KDE_EXPORT KDecorationFactory* create_factory()
   {
      return new RiscOS::Factory();
   }
}


namespace RiscOS
{

Manager::Manager(KDecorationBridge *bridge,
                 KDecorationFactory *factory)
  : KDecoration(bridge, factory),
    topLayout_    (NULL),
    titleLayout_  (NULL),
    titleSpacer_  (NULL)
{
}

Manager::~Manager()
{
}

void Manager::init()
{
   createMainWidget(WNoAutoErase);

   widget()->installEventFilter(this);
   widget()->setBackgroundMode(NoBackground);

   leftButtonList_.setAutoDelete(true);
   rightButtonList_.setAutoDelete(true);

   resetLayout();
}

bool Manager::eventFilter(TQObject *o, TQEvent *e)
{
   if (TQT_BASE_OBJECT(o) != TQT_BASE_OBJECT(widget())) return false;
   switch (e->type())
   {
      case TQEvent::Resize:
         resizeEvent(TQT_TQRESIZEEVENT(e));
         return true;
      case TQEvent::Paint:
         paintEvent(TQT_TQPAINTEVENT(e));
         return true;
      case TQEvent::MouseButtonDblClick:
         mouseDoubleClickEvent(TQT_TQMOUSEEVENT(e));
         return true;
      case TQEvent::MouseButtonPress:
         processMousePressEvent(TQT_TQMOUSEEVENT(e));
         return true;
      case TQEvent::Wheel:
         wheelEvent( TQT_TQWHEELEVENT( e ));
         return true;
      case TQEvent::MouseButtonRelease:
         return false;
      case TQEvent::Show:
         return false;
      case TQEvent::MouseMove:
         return false;
      case TQEvent::Enter:
         return false;
      case TQEvent::Leave:
         return false;
      case TQEvent::Move:
         return false;
      default:
         return false;
   }
}

void Manager::reset(unsigned long /*changed*/)
{
   resetLayout();
}

void Manager::borders(int &left, int &right, int &top, int &bottom) const
{
   left = right = 1;
   top = Static::instance()->titleHeight();
   bottom = isResizable() ? Static::instance()->resizeHeight() : 1;
}

void Manager::resize(const TQSize &s)
{
   widget()->resize(s);
}

TQSize Manager::tqminimumSize() const
{
   return widget()->tqminimumSize();
}

void Manager::activeChange()
{
   updateTitleBuffer();
   widget()->tqrepaint();
   emit(activeChanged(isActive()));
}

void Manager::captionChange()
{
   updateTitleBuffer();
   widget()->tqrepaint();
}

void Manager::iconChange()
{
}

void Manager::maximizeChange()
{
   emit(maximizeChanged(maximizeMode() == MaximizeFull));
}

void Manager::desktopChange()
{
}

void Manager::shadeChange()
{
}

void Manager::paintEvent(TQPaintEvent *e)
{
   TQPainter p(widget());

   TQRect r(e->rect());

   bool intersectsLeft = r.intersects(TQRect(0, 0, 1, height()));

   bool intersectsRight =
      r.intersects(TQRect(width() - 1, 0, width(), height()));

   if (intersectsLeft || intersectsRight)
   {
      p.setPen(TQt::black);

      if (intersectsLeft)
         p.drawLine(0, r.top(), 0, r.bottom());

      if (intersectsRight)
         p.drawLine(width() - 1, r.top(), width() - 1, r.bottom());
   }

   Static * s = Static::instance();

   bool active = isActive();

   // Title bar.

   TQRect tr = titleSpacer_->tqgeometry();
   bitBlt(widget(), tr.topLeft(), &titleBuf_);

   // Resize bar.

   if (isResizable())
   {
      int rbt = height() - Static::instance()->resizeHeight(); // Resize bar top

      bitBlt(widget(), 0, rbt, &(s->resize(active)));
      bitBlt(widget(), 30, rbt, &(s->resizeMidLeft(active)));

      p.drawTiledPixmap(32, rbt, width() - 34,
                        Static::instance()->resizeHeight(),
                        s->resizeMidMid(active));

      bitBlt(widget(), width() - 32, rbt, &(s->resizeMidRight(active)));
      bitBlt(widget(), width() - 30, rbt, &(s->resize(active)));
   }
   else
      p.drawLine(1, height() - 1, width() - 2, height() - 1);
}

void Manager::resizeEvent(TQResizeEvent*)
{
   updateButtonVisibility();
   updateTitleBuffer();
   widget()->tqrepaint();
}

void Manager::updateButtonVisibility()
{
#if 0
   enum SizeProblem = { None, Small, Medium, Big };
   SizeProblem sizeProblem = None;

   if (width() < 80) sizeProblem = Big;
   else if (width() < 100) sizeProblem = Medium;
   else if (width() < 180) sizeProblem = Small;

   switch (sizeProblem) {

      case Small:
         above_    ->hide();
         lower_    ->hide();
         sticky_   ->hide();
         help_     ->hide();
         iconify_  ->show();
         maximise_ ->hide();
         close_    ->show();
         break;

      case Medium:
         above_    ->hide();
         lower_    ->hide();
         sticky_   ->hide();
         help_     ->hide();
         iconify_  ->hide();
         maximise_ ->hide();
         close_    ->show();
         break;

      case Big:
         above_    ->hide();
         lower_    ->hide();
         sticky_   ->hide();
         help_     ->hide();
         iconify_  ->hide();
         maximise_ ->hide();
         close_    ->hide();
         break;

      case None:
      default:
         above_    ->show();
         lower_    ->show();
         sticky_   ->show();
         if (providesContextHelp())
           help_->show();
         iconify_  ->show();
         maximise_ ->show();
         close_    ->show();
         break;
   }

   tqlayout()->activate();
#endif
}

void Manager::updateTitleBuffer()
{
   bool active = isActive();

   Static * s = Static::instance();

   TQRect tr = titleSpacer_->tqgeometry();

   if (tr.width() == 0 || tr.height() == 0)
      titleBuf_.resize(8, 8);
   else
      titleBuf_.resize(tr.size());

   TQPainter p(&titleBuf_);

   p.drawPixmap(0, 0, s->titleTextLeft(active));

   p.drawTiledPixmap(3, 0, tr.width() - 6, Static::instance()->titleHeight(),
                     s->titleTextMid(active));

   p.setPen(options()->color(KDecorationOptions::ColorFont, active));

   p.setFont(options()->font(active));

   p.drawText(4, 2, tr.width() - 8, Static::instance()->titleHeight() - 4,
              AlignCenter, caption());

   p.drawPixmap(tr.width() - 3, 0, s->titleTextRight(active));
}

KDecoration::Position Manager::mousePosition(const TQPoint& p) const
{
   Position m = PositionCenter;

   // Look out for off-by-one errors here.

   if (isResizable())
   {
      if (p.y() > (height() - (Static::instance()->resizeHeight() + 1)))
      {
        // Keep order !

         if (p.x() >= (width() - 30))
            m = PositionBottomRight;
         else if (p.x() <= 30)
            m = PositionBottomLeft;
         else
            m = PositionBottom;
      }
      else
      {
         m = PositionCenter;
         // Client::mousePosition(p);
      }
   }
   else
   {
      m = PositionCenter;
      // Client::mousePosition(p);
   }

   return m;
}

void Manager::mouseDoubleClickEvent(TQMouseEvent *e)
{
   if (e->button() == Qt::LeftButton && titleSpacer_->tqgeometry().contains(e->pos()))
      titlebarDblClickOperation();
}

void Manager::wheelEvent(TQWheelEvent *e)
{
    if (isSetShade() || titleLayout_->tqgeometry().contains(e->pos()) )
        titlebarMouseWheelOperation( e->delta());
}

void Manager::paletteChange(const TQPalette &)
{
   resetLayout();
}

void Manager::stickyChange(bool b)
{
   emit(stickyChanged(b));
}

void Manager::slotToggleSticky()
{
   toggleOnAllDesktops();
   emit(stickyChanged(isOnAllDesktops()));
}

void Manager::slotAbove()
{
   setKeepAbove(!keepAbove());
}

void Manager::slotLower()
{
   setKeepBelow(!keepBelow());
}

void Manager::slotMaximizeClicked(ButtonState state)
{
#if KDE_IS_VERSION(3, 3, 0)
   maximize(state);
#else
   switch (state)
   {
      case RightButton:
         maximize(maximizeMode() ^ MaximizeHorizontal);
         break;

      case MidButton:
         maximize(maximizeMode() ^ MaximizeVertical);
         break;

      case LeftButton:
      default:
         maximize(maximizeMode() == MaximizeFull ? MaximizeRestore
                                                 : MaximizeFull);
         break;
   }
#endif
   emit(maximizeChanged(maximizeMode() == MaximizeFull));
}

bool Manager::animateMinimize(bool iconify)
{
   int style = Static::instance()->animationStyle();

   switch (style)
   {
      case 1:
      {
         // Double twisting double back, with pike ;)

         if (!iconify) // No animation for restore.
            return true;

         // Go away quick.
         helperShowHide(false);
         tqApp->syncX();

         TQRect r = iconGeometry();

         if (!r.isValid())
            return true;

         // Algorithm taken from Window Maker (http://www.windowmaker.org)

         int sx = geometry().x();
         int sy = geometry().y();
         int sw = width();
         int sh = height();
         int dx = r.x();
         int dy = r.y();
         int dw = r.width();
         int dh = r.height();

         double steps = 12;

         double xstep = double((dx-sx)/steps);
         double ystep = double((dy-sy)/steps);
         double wstep = double((dw-sw)/steps);
         double hstep = double((dh-sh)/steps);

         double cx = sx;
         double cy = sy;
         double cw = sw;
         double ch = sh;

         double finalAngle = 3.14159265358979323846;

         double delta  = finalAngle / steps;

         TQPainter p(workspaceWidget());
         p.setRasterOp(TQt::NotROP);

         for (double angle = 0; ; angle += delta)
         {
            if (angle > finalAngle)
               angle = finalAngle;

            double dx = (cw / 10) - ((cw / 5) * sin(angle));
            double dch = (ch / 2) * cos(angle);
            double midy = cy + (ch / 2);

            TQPoint p1(int(cx + dx), int(midy - dch));
            TQPoint p2(int(cx + cw - dx), p1.y());
            TQPoint p3(int(cx + dw + dx), int(midy + dch));
            TQPoint p4(int(cx - dx), p3.y());

            grabXServer();

            p.drawLine(p1, p2);
            p.drawLine(p2, p3);
            p.drawLine(p3, p4);
            p.drawLine(p4, p1);

            p.flush();

            usleep(500);

            p.drawLine(p1, p2);
            p.drawLine(p2, p3);
            p.drawLine(p3, p4);
            p.drawLine(p4, p1);

            ungrabXServer();

            cx += xstep;
            cy += ystep;
            cw += wstep;
            ch += hstep;

            if (angle >= finalAngle)
               break;
         }
      }
      break;

      case 2:
      {
         // KVirc style ? Maybe. For qwertz.

         if (!iconify) // No animation for restore.
            return true;

         // Go away quick.
         helperShowHide(false);
         tqApp->syncX();

         int stepCount = 12;

         TQRect r(geometry());

         int dx = r.width() / (stepCount * 2);
         int dy = r.height() / (stepCount * 2);

         TQPainter p(workspaceWidget());
         p.setRasterOp(TQt::NotROP);

         for (int step = 0; step < stepCount; step++)
         {
            r.moveBy(dx, dy);
            r.setWidth(r.width() - 2 * dx);
            r.setHeight(r.height() - 2 * dy);

            grabXServer();

            p.drawRect(r);
            p.flush();
            usleep(200);
            p.drawRect(r);

            ungrabXServer();
         }
      }
      break;


      default:
      {
         TQRect icongeom = iconGeometry();

         if (!icongeom.isValid())
            return true;

         TQRect wingeom = geometry();

         TQPainter p(workspaceWidget());

         p.setRasterOp(TQt::NotROP);
#if 0
         if (iconify)
            p.setClipRegion(TQRegion(workspaceWidget()->rect()) - wingeom);
#endif
         grabXServer();

         p.drawLine(wingeom.bottomRight(), icongeom.bottomRight());
         p.drawLine(wingeom.bottomLeft(), icongeom.bottomLeft());
         p.drawLine(wingeom.topLeft(), icongeom.topLeft());
         p.drawLine(wingeom.topRight(), icongeom.topRight());

         p.flush();

         tqApp->syncX();

         usleep(30000);

         p.drawLine(wingeom.bottomRight(), icongeom.bottomRight());
         p.drawLine(wingeom.bottomLeft(), icongeom.bottomLeft());
         p.drawLine(wingeom.topLeft(), icongeom.topLeft());
         p.drawLine(wingeom.topRight(), icongeom.topRight());

         ungrabXServer();
      }
      break;
   }
   return true;
}

void Manager::createTitle()
{
   leftButtonList_.clear();
   rightButtonList_.clear();

   TQString buttons;

   if (options()->customButtonPositions())
      buttons = options()->titleButtonsLeft() + "|" +
                options()->titleButtonsRight();
   else
      buttons = "XSH|IA";

   TQPtrList<Button> *buttonList = &leftButtonList_;

   for (unsigned int i = 0; i < buttons.length(); ++i)
   {
      Button * tb = NULL;

      switch (buttons[i].latin1())
      {
         case 'S': // Sticky
            tb = new StickyButton(widget());
            connect(this, TQT_SIGNAL(stickyChanged(bool)),
                    tb, TQT_SLOT(setOn(bool)));
            connect(tb, TQT_SIGNAL(toggleSticky()), this, TQT_SLOT(slotToggleSticky()));
            emit(stickyChanged(isOnAllDesktops()));
            break;

         case 'H': // Help
            if (providesContextHelp())
            {
               tb = new HelpButton(widget());
               connect(tb, TQT_SIGNAL(help()), this, TQT_SLOT(showContextHelp()));
            }
            break;

         case 'I': // Minimize
            if (isMinimizable())
            {
               tb = new IconifyButton(widget());
               connect(tb, TQT_SIGNAL(iconify()), this, TQT_SLOT(minimize()));
            }
            break;

         case 'A': // Maximize
            if (isMaximizable())
            {
               tb = new MaximiseButton(widget());
               connect(tb, TQT_SIGNAL(maximizeClicked(ButtonState)),
                       this, TQT_SLOT(slotMaximizeClicked(ButtonState)));
               connect(this, TQT_SIGNAL(maximizeChanged(bool)),
                       tb, TQT_SLOT(setOn(bool)));
               emit(maximizeChanged(maximizeMode() == MaximizeFull));
            }
            break;

         case 'F': // Above
            tb = new AboveButton(widget());
            connect(tb, TQT_SIGNAL(above()), this, TQT_SLOT(slotAbove()));
            break;

         case 'B': // Lower
            tb = new LowerButton(widget());
            connect(tb, TQT_SIGNAL(lower()), this, TQT_SLOT(slotLower()));
            break;

         case 'X': // Close
            if (isCloseable())
            {
               tb = new CloseButton(widget());
               connect(tb, TQT_SIGNAL(closeWindow()), this, TQT_SLOT(closeWindow()));
            }
            break;

         case '|':
            buttonList = &rightButtonList_;
            break;
      }

      if (tb != NULL)
      {
         connect(this, TQT_SIGNAL(activeChanged(bool)), tb, TQT_SLOT(setActive(bool)));
         buttonList->append(tb);
      }
   }

   for (TQPtrListIterator<Button> it(leftButtonList_); it.current(); ++it)
   {
      it.current()->tqsetAlignment(Button::Left);
      titleLayout_->addWidget(it.current());
   }

   titleSpacer_ = new TQSpacerItem(0, Static::instance()->titleHeight(),
                                  TQSizePolicy::Expanding, TQSizePolicy::Fixed);

   titleLayout_->addItem(titleSpacer_);

   for (TQPtrListIterator<Button> it(rightButtonList_); it.current(); ++it)
   {
      it.current()->tqsetAlignment(Button::Right);
      titleLayout_->addWidget(it.current());
   }
}

void Manager::resetLayout()
{
   delete topLayout_;
   topLayout_ = new TQVBoxLayout(widget(), 0, 0);
   topLayout_->setResizeMode(TQLayout::FreeResize);

   titleLayout_ = new TQBoxLayout(topLayout_, TQBoxLayout::LeftToRight, 0, 0);
   titleLayout_->setResizeMode(TQLayout::FreeResize);

   createTitle();

   TQBoxLayout *midLayout = new TQBoxLayout(topLayout_, TQBoxLayout::LeftToRight,
                                          0, 0);
   midLayout->setResizeMode(TQLayout::FreeResize);
   midLayout->addSpacing(1);
   if (isPreview())
      midLayout->addWidget(
         new TQLabel(i18n("<center><b>RiscOS preview</b></center>"), widget()));
   midLayout->addSpacing(1);

   if (isResizable())
      topLayout_->addSpacing(Static::instance()->resizeHeight());
   else
      topLayout_->addSpacing(1);
}

// --------------------

Factory::Factory()
{
   (void) RiscOS::Static::instance();
}

Factory::~Factory()
{
   delete RiscOS::Static::instance();
}

bool Factory::reset(unsigned long /*changed*/)
{
   RiscOS::Static::instance()->reset();
   return true;
}

bool Factory::supports( Ability ability )
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
        case AbilityButtonAboveOthers:
        case AbilityButtonBelowOthers:
            return true;
        default:
            return false;
    };
}

KDecoration* Factory::createDecoration(KDecorationBridge *bridge)
{
   return new Manager(bridge, this);
}

} // End namespace

// vim:ts=2:sw=2:tw=78
#include "Manager.moc"
