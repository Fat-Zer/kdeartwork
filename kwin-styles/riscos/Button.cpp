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

#include <tqtooltip.h>
#include "Button.h"
#include "Static.h"

namespace RiscOS
{

Button::Button(TQWidget *parent, const TQString& tip,
               const ButtonState realizeButtons)
  : TQWidget(parent, "Button", 0),
    realizeButtons_(realizeButtons),
    lastButton_(Qt::NoButton),
    alignment_(Left),
    down_     (false),
    active_   (false)
{
   TQToolTip::add(this, tip);
   setBackgroundColor(TQt::black);

   setFixedSize(Static::instance()->titleHeight() - 1,
                Static::instance()->titleHeight());
}

Button::~Button()
{
  // Empty.
}

void Button::tqsetAlignment(Alignment a)
{
   alignment_ = a;
   tqrepaint();
}

void Button::setActive(bool b)
{
   active_ = b;
   tqrepaint();
}

Button::Alignment Button::tqalignment() const
{
   return alignment_;
}

void Button::mousePressEvent(TQMouseEvent *e)
{
   down_ = true;
   lastButton_ = e->button();
   tqrepaint();

   TQMouseEvent me(e->type(), e->pos(), e->globalPos(),
                  (e->button()&realizeButtons_) ? Qt::LeftButton : Qt::NoButton,
                  e->state());
   TQWidget::mousePressEvent(&me);
}

void Button::mouseReleaseEvent(TQMouseEvent *e)
{
   down_ = false;
   lastButton_ = e->button();
   tqrepaint();
   TQMouseEvent me(e->type(), e->pos(), e->globalPos(),
                  (e->button()&realizeButtons_) ? Qt::LeftButton : Qt::NoButton,
                  e->state());
   TQWidget::mouseReleaseEvent(&me);
}

void Button::setPixmap(const TQPixmap &p)
{
   if (TQPixmap::defaultDepth() <= 8)
      aPixmap_ = iPixmap_ = p;
   else
   {
      TQRgb light;
      TQRgb* data = NULL;
      TQRgb w = tqRgb(255, 255, 255);

      TQImage aTx(p.convertToImage());
      TQImage iTx(aTx.copy());

      const KDecorationOptions* options = KDecoration::options();
      light = options->color(KDecoration::ColorButtonBg, true).light(150).rgb();

      if (light == tqRgb(0, 0, 0))
         light = tqRgb(228, 228, 228);

      data = (TQRgb *)aTx.bits();

      for (int x = 0; x < 144; x++)
         if (data[x] == w)
            data[x] = light;

      light = options->color(KDecoration::ColorButtonBg, false).light(150).rgb();

      if (light == tqRgb(0, 0, 0))
         light = tqRgb(228, 228, 228);

      data = (TQRgb *)iTx.bits();

      for (int x = 0; x < 144; x++)
         if (data[x] == w)
            data[x] = light;

      aPixmap_.convertFromImage(aTx);
      iPixmap_.convertFromImage(iTx);

      if (0 != p.mask())
      {
         aPixmap_.setMask(*p.mask());
         iPixmap_.setMask(*p.mask());
      }
   }
   tqrepaint();
}

void Button::paintEvent(TQPaintEvent *)
{
   bitBlt(this, alignment_ == Left ? 1 : 0, 0,
          &Static::instance()->buttonBase(active_, down_));

   int i = width() / 2 - 6;

   bitBlt(this, alignment_ == Left ? i + 1 : i,
          i + 1, active_ ? &aPixmap_ : &iPixmap_);
}

} // End namespace

// vim:ts=2:sw=2:tw=78
#include "Button.moc"
