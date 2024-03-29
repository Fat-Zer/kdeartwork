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

#include "HelpButton.h"

namespace RiscOS
{

/* XPM */
static const char * const help_xpm[] = {
"12 12 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
" .  ++++  . ",
"  .+....+.  ",
"  +......+  ",
"..+......+..",
"  +......+  ",
"  +......+  ",
"  .+....+.  ",
" .  ++ +  . ",
"    + ++    ",
"    ++ +    ",
"    + ++    ",
"     ++     "};

HelpButton::HelpButton(TQWidget *parent)
  : Button(parent, i18n("Help"))
{
   setPixmap(TQPixmap((const char **)help_xpm));
}

void HelpButton::mouseReleaseEvent(TQMouseEvent *e)
{
   Button::mouseReleaseEvent(e);

   if (!rect().contains(e->pos()))
      return;

   emit(help());
}

} // End namespace;

// vim:ts=2:sw=2:tw=78
#include "HelpButton.moc"
