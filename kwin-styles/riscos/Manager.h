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

#ifndef RISC_OS_MANAGER_H
#define RISC_OS_MANAGER_H

#include <tqdict.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class TQSpacerItem;
class TQVBoxLayout;
class TQBoxLayout;

namespace RiscOS
{

class LowerButton;
class CloseButton;
class IconifyButton;
class MaximiseButton;
class StickyButton;
class HelpButton;
class Button;

class Manager : public KDecoration
{
   Q_OBJECT
  TQ_OBJECT

   public:

      Manager(KDecorationBridge*, KDecorationFactory*);
      ~Manager();
      void init();
      bool eventFilter(TQObject*, TQEvent*);
      void reset(unsigned long changed);
      void borders(int&, int&, int&, int&) const;
      void resize(const TQSize&);
      TQSize tqminimumSize() const;
      void activeChange();
      void captionChange();
      void iconChange();
      void maximizeChange();
      void desktopChange();
      void shadeChange();

   signals:

      void maximizeChanged(bool);
      void stickyChanged(bool);
      void activeChanged(bool);

   public slots:

      void slotAbove();
      void slotLower();
      void slotMaximizeClicked(ButtonState);
      void slotToggleSticky();

   protected:

      KDecoration::Position mousePosition(const TQPoint &) const;
      void paletteChange(const TQPalette &);
      void activeChange(bool);
      void stickyChange(bool);
      void paintEvent(TQPaintEvent *);
      void resizeEvent(TQResizeEvent *);
      void mouseDoubleClickEvent(TQMouseEvent *);
      void wheelEvent(TQWheelEvent *e);
      bool animateMinimize(bool);
      void updateButtonVisibility();
      void updateTitleBuffer();

      void createTitle();
      void resetLayout();

   private:

      TQVBoxLayout       *topLayout_;
      TQBoxLayout        *titleLayout_;
      TQSpacerItem       *titleSpacer_;

      TQPixmap           titleBuf_;
      TQPtrList<Button>  leftButtonList_;
      TQPtrList<Button>  rightButtonList_;
};

class Factory : public TQObject, public KDecorationFactory
{
   Q_OBJECT
  TQ_OBJECT

   public:
      Factory();
      ~Factory();
      virtual bool reset(unsigned long changed);
      virtual KDecoration* createDecoration(KDecorationBridge*);
      virtual bool supports( Ability ability );
};

} // End namespace

#endif

// vim:ts=2:sw=2:tw=78
