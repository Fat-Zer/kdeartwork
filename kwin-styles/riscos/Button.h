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

#ifndef RISC_OS_BUTTON_H
#define RISC_OS_BUTTON_H

#include <tqpixmap.h>
#include <tqwidget.h>
#include <klocale.h>
#include <kdecoration.h>

namespace RiscOS
{

class Button : public QWidget
{
   Q_OBJECT

   public:

      enum Type { Lower, Close, Iconify, Maximise, Sticky, Help };

      enum Alignment { Left, Right };

      Button(TQWidget *parent, const TQString &tip,
             const ButtonState realizeButton = LeftButton);
      virtual ~Button();

      void setAlignment(Alignment);

      Alignment alignment() const;

   protected slots:

      void setActive(bool);

   protected:

      void paintEvent(TQPaintEvent *);

      void mousePressEvent(TQMouseEvent *);
      void mouseReleaseEvent(TQMouseEvent *);
      void mouseMoveEvent(TQMouseEvent *) { /* Empty. */ }

      void setPixmap(const TQPixmap &);

      ButtonState realizeButtons_;
      ButtonState lastButton_;

   private:

      Alignment   alignment_;
      bool        down_;
      bool        active_;
      TQPixmap     aPixmap_, iPixmap_;
};

} // End namespace

#endif

// vim:ts=2:sw=2:tw=78
