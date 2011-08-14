//////////////////////////////////////////////////////////////////////////////
// phasestyle.cpp
// -------------------
// TQt/KDE widget style
// -------------------
// Copyright (c) 2004 David Johnson
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////
//
// Some miscellaneous notes
//
// Reimplemented scrollbar metric and drawing routines from KStyle to allow
// better placement of subcontrols. This is because the subcontrols slightly
// overlap to share part of their border.
//
// Menu and toolbars are painted with the background color by default. This
// differs from the TQt default of giving them PaletteButton backgrounds.
// Menubars have normal gradients, toolbars have reverse.
//
// Some toolbars are not part of a TQMainWindows, such as in a KDE file dialog.
// In these cases we treat the toolbar as "floating" and paint it flat.
//
//////////////////////////////////////////////////////////////////////////////

#include <kdrawutil.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>

#include <tqapplication.h>
#include <tqintdict.h>
#include <tqpainter.h>
#include <tqpointarray.h>
#include <tqsettings.h>
#include <tqstyleplugin.h>

#include <tqcheckbox.h>
#include <tqcombobox.h>
#include <tqheader.h>
#include <tqmainwindow.h>
#include <tqmenubar.h>
#include <tqpopupmenu.h>
#include <tqprogressbar.h>
#include <tqpushbutton.h>
#include <tqradiobutton.h>
#include <tqscrollbar.h>
#include <tqslider.h>
#include <tqsplitter.h>
#include <tqtabbar.h>
#include <tqtabwidget.h>
#include <tqtoolbar.h>
#include <tqtoolbox.h>
#include <tqtoolbutton.h>

#include "phasestyle.h"
#include "bitmaps.h"

static const char* TQSPLITTERHANDLE    = TQSPLITTERHANDLE_OBJECT_NAME_STRING;
static const char* TQTOOLBAREXTENSION  = "TQToolBarExtensionWidget";
static const char* KTOOLBARWIDGET     = "kde toolbar widget";

// some convenient constants
static const int ITEMFRAME       = 1; // menu stuff
static const int ITEMHMARGIN     = 3;
static const int ITEMVMARGIN     = 0;
static const int ARROWMARGIN     = 6;
static const int RIGHTBORDER     = 6;
static const int MINICONSIZE     = 16;
static const int CHECKSIZE       = 9;
static const int MAXGRADIENTSIZE = 64;

static unsigned contrast = 110;


//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction, Initialization                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PhaseStyle()
// -----------
// Constructor

PhaseStyle::PhaseStyle()
    : KStyle(FilledFrameWorkaround | AllowMenuTransparency,
             ThreeButtonScrollBar), hover_(0), hovertab_(0),
      gradients_(TQPixmap::defaultDepth() > 8), kicker_(false)
{
    TQSettings settings;
    if (gradients_) { // don't bother setting if already false
        gradients_ =
            settings.readBoolEntry("/phasestyle/Settings/gradients", true);
        contrast = 100 + settings.readNumEntry("/TQt/KDE/contrast", 5);
    }
    highlights_ =
        settings.readBoolEntry("/phasestyle/Settings/highlights", true);

    gradients = new TQMap<unsigned int, TQIntDict<GradientSet> >;

    reverse_ = TQApplication::reverseLayout();

    // create bitmaps
    uarrow = TQBitmap(6, 6, uarrow_bits, true);
    uarrow.setMask(uarrow);
    darrow = TQBitmap(6, 6, darrow_bits, true);
    darrow.setMask(darrow);
    larrow = TQBitmap(6, 6, larrow_bits, true);
    larrow.setMask(larrow);
    rarrow = TQBitmap(6, 6, rarrow_bits, true);
    rarrow.setMask(rarrow);
    bplus = TQBitmap(6, 6, bplus_bits, true);
    bplus.setMask(bplus);
    bminus = TQBitmap(6, 6, bminus_bits, true);
    bminus.setMask(bminus);
    bcheck = TQBitmap(9, 9, bcheck_bits, true);
    bcheck.setMask(bcheck);
    dexpand = TQBitmap(9, 9, dexpand_bits, true);
    dexpand.setMask(dexpand);
    rexpand = TQBitmap(9, 9, rexpand_bits, true);
    rexpand.setMask(rexpand);
    doodad_mid = TQBitmap(4, 4, doodad_mid_bits, true);
    doodad_light = TQBitmap(4, 4, doodad_light_bits, true);
}

PhaseStyle::~PhaseStyle() 
{
    delete gradients;
    gradients = 0;
}

//////////////////////////////////////////////////////////////////////////////
// Polishing                                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize application specific

void PhaseStyle::polish(TQApplication* app)
{
    if (!qstrcmp(app->argv()[0], "kicker")) kicker_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the appearance of a widget

void PhaseStyle::polish(TQWidget *widget)
{
    if (::tqqt_cast<TQMenuBar*>(widget) ||
        ::tqqt_cast<TQPopupMenu*>(widget)) {
        // anti-flicker optimization
        widget->setBackgroundMode(NoBackground);
    } else if (::tqqt_cast<TQFrame*>(widget) ||
               widget->inherits(TQTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        // needs special handling on paint events
        widget->installEventFilter(this);
    } else if (highlights_ &&
               (::tqqt_cast<TQPushButton*>(widget) ||
                ::tqqt_cast<TQComboBox*>(widget) ||
                ::tqqt_cast<TQSpinWidget*>(widget) ||
                ::tqqt_cast<TQCheckBox*>(widget) ||
                ::tqqt_cast<TQRadioButton*>(widget) ||
                ::tqqt_cast<TQSlider*>(widget) ||
                widget->inherits(TQSPLITTERHANDLE))) {
        // mouseover highlighting
        widget->installEventFilter(this);
    } else if (highlights_ && ::tqqt_cast<TQTabBar*>(widget)) {
        // highlighting needing mouse tracking
        widget->setMouseTracking(true);
        widget->installEventFilter(this);
    }

    KStyle::polish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the palette

void PhaseStyle::polish(TQPalette &pal)
{
    // clear out gradients on a color change
    gradients->clear();

    // lighten up a bit, so the look is not so "crisp"
    if (TQPixmap::defaultDepth() > 8) { // but not on low color displays
        pal.setColor(TQPalette::Disabled, TQColorGroup::Dark,
            pal.color(TQPalette::Disabled, TQColorGroup::Dark).light(contrast));
        pal.setColor(TQPalette::Active, TQColorGroup::Dark,
            pal.color(TQPalette::Active, TQColorGroup::Dark).light(contrast));
        pal.setColor(TQPalette::Inactive, TQColorGroup::Dark,
            pal.color(TQPalette::Inactive, TQColorGroup::Dark).light(contrast));
    }

    TQStyle::polish(pal);
}

//////////////////////////////////////////////////////////////////////////////
// unPolish()
// ----------
// Undo the initialization of a widget's appearance

void PhaseStyle::unPolish(TQWidget *widget)
{
    if (::tqqt_cast<TQMenuBar*>(widget) ||
        ::tqqt_cast<TQPopupMenu*>(widget)) {
        widget->setBackgroundMode(PaletteBackground);
    } else if (::tqqt_cast<TQFrame*>(widget) ||
               widget->inherits(TQTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        widget->removeEventFilter(this);
    } else if (highlights_ && // highlighting
               (::tqqt_cast<TQPushButton*>(widget) ||
                ::tqqt_cast<TQComboBox*>(widget) ||
                ::tqqt_cast<TQSpinWidget*>(widget) ||
                ::tqqt_cast<TQCheckBox*>(widget) ||
                ::tqqt_cast<TQRadioButton*>(widget) ||
                ::tqqt_cast<TQSlider*>(widget) ||
                widget->inherits(TQSPLITTERHANDLE))) {
        widget->removeEventFilter(this);
    } else if (highlights_ && ::tqqt_cast<TQTabBar*>(widget)) {
        widget->setMouseTracking(false);
        widget->removeEventFilter(this);
    }

    KStyle::unPolish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// Drawing                                                                  //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// drawPhaseGradient()
// ------------------
// Draw gradient

void PhaseStyle::drawPhaseGradient(TQPainter *painter,
                                   const TQRect &rect,
                                   TQColor color,
                                   bool horizontal,
                                   int px, int py,
                                   int pw, int ph,
                                   bool reverse) const
{
    if (!gradients_) {
        painter->fillRect(rect, color);
        return;
    }

    // px, py, pw, ph used for parent-relative pixmaps
    int size;
    if (horizontal)
        size = (pw > 0) ? pw : rect.width();
    else
        size = (ph > 0) ? ph : rect.height();

    if (size > MAXGRADIENTSIZE) { // keep it sensible
        painter->fillRect(rect, color);
    } else {
        // lazy allocation
        GradientSet *set = (*gradients)[color.rgb()][size];
        if (!set) {
            set = new GradientSet(color, size);
            (*gradients)[color.rgb()].setAutoDelete(true);
            (*gradients)[color.rgb()].insert(size, set);
        }
        painter->drawTiledPixmap(rect, *set->gradient(horizontal, reverse),
                                 TQPoint(px, py));
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseBevel()
// ----------------
// Draw the basic Phase bevel

void PhaseStyle::drawPhaseBevel(TQPainter *painter,
				int x, int y, int w, int h,
				const TQColorGroup &group,
				const TQColor &fill,
				bool sunken,
				bool horizontal,
				bool reverse) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    painter->setPen(group.dark());
    painter->drawRect(x, y, w, h);

    painter->setPen(sunken ? group.mid() : group.midlight());
    painter->drawLine(x+1, y+1, x2-2, y+1);
    painter->drawLine(x+1, y+2, x+1, y2-2);

    painter->setPen(sunken ? group.midlight() : group.mid());
    painter->drawLine(x+2, y2-1, x2-1, y2-1);
    painter->drawLine(x2-1, y+2, x2-1, y2-2);

    painter->setPen(group.button());
    painter->drawPoint(x+1, y2-1);
    painter->drawPoint(x2-1, y+1);

    if (sunken) {
        // sunken bevels don't get gradients
        painter->fillRect(x+2, y+2, w-4, h-4, fill);
    } else {
        drawPhaseGradient(painter, TQRect(x+2, y+2, w-4, h-4), fill,
                          horizontal, 0, 0, w-4, h-4, reverse);
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseButton()
// ----------------
// Draw the basic Phase button

void PhaseStyle::drawPhaseButton(TQPainter *painter,
                                 int x, int y, int w, int h,
                                 const TQColorGroup &group,
                                 const TQColor &fill,
                                 bool sunken) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    painter->setPen(group.midlight());
    painter->drawLine(x+1, y2, x2, y2);
    painter->drawLine(x2, y+1, x2, y2-1);

    painter->setPen(group.mid());
    painter->drawLine(x, y,  x2-1, y);
    painter->drawLine(x, y+1, x, y2-1);

    painter->setPen(group.button());
    painter->drawPoint(x, y2);
    painter->drawPoint(x2, y);

    drawPhaseBevel(painter, x+1, y+1, w-2, h-2, group, fill,
                   sunken, false, false);
}

//////////////////////////////////////////////////////////////////////////////
// drawPhasePanel()
// ----------------
// Draw the basic Phase panel

void PhaseStyle::drawPhasePanel(TQPainter *painter,
                                int x, int y, int w, int h,
                                const TQColorGroup &group,
                                bool sunken,
                                const TQBrush *fill) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    if (sunken) {
        painter->setPen(group.dark());
        painter->drawRect(x+1, y+1, w-2, h-2);
        painter->setPen(group.midlight());
        painter->drawLine(x+1, y2, x2, y2);
        painter->drawLine(x2, y+1, x2, y2-1);
        painter->setPen(group.mid());
        painter->drawLine(x, y, x, y2-1);
        painter->drawLine(x+1, y, x2-1, y);
        painter->setPen(group.background());
        painter->drawPoint(x, y2);
        painter->drawPoint(x2, y);
    } else {
        painter->setPen(group.dark());
        painter->drawRect(x, y, w, h);
        painter->setPen(group.midlight());
        painter->drawLine(x+1, y+1, x2-2, y+1);
        painter->drawLine(x+1, y+2, x+1, y2-2);
        painter->setPen(group.mid());
        painter->drawLine(x+2, y2-1, x2-1, y2-1);
        painter->drawLine(x2-1, y+2, x2-1, y2-2);
        painter->setPen(group.background());
        painter->drawPoint(x+1, y2-1);
        painter->drawPoint(x2-1, y+1);
    }

    if (fill) {
        painter->fillRect(x+2, y+2, w-4, h-4, fill->color());
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawPhaseTab()
// -------------
// Draw a Phase style tab

void PhaseStyle::drawPhaseTab(TQPainter *painter,
                              int x, int y, int w, int h,
                              const TQColorGroup &group,
                              const TQTabBar *bar,
                              const TQStyleOption &option,
                              SFlags flags) const
{
    const TQTabWidget *tabwidget;
    bool selected = (flags & Style_Selected);
    bool edge; // tab is at edge of bar
    const int x2 = x + w - 1;
    const int y2 = y + h - 1;

    painter->save();

    // what position is the tab?
    if ((bar->count() == 1)
	|| (bar->indexOf(option.tab()->identifier()) == 0)) {
	edge = true;
    } else {
	edge = false;
    }

    switch (TQTabBar::Shape(bar->tqshape())) {
      case TQTabBar::RoundedAbove:
      case TQTabBar::TriangularAbove: {
	  // is there a corner widget?
	  tabwidget = ::tqqt_cast<TQTabWidget*>(bar->parent());
	  if (edge && tabwidget
	      && tabwidget->cornerWidget(reverse_ ?
					 TQt::TopRight : TQt::TopLeft)) {
	      edge = false;
	  }

          if (!selected) { // shorten
              y += 2; h -= 2;
          }
          if (selected) {
              painter->setPen(TQt::NoPen);
              painter->fillRect(x+1, y+1, w-1, h-1,
                                group.brush(TQColorGroup::Background));
          } else {
              drawPhaseGradient(painter, TQRect(x+1, y+1, w-1, h-2),
                                (flags & Style_MouseOver)
                                ? group.background()
                                : TQColor(group.background().dark(contrast)),
                                false, 0, 0, 0, h*2, false);
          }

          // draw tab
          painter->setPen(group.dark());
          painter->drawLine(x, y, x, y2-2);
          painter->drawLine(x+1, y, x2, y);
          painter->drawLine(x2, y+1, x2, y2-2);

          painter->setPen(group.mid());
          painter->drawLine(x2-1, y+2, x2-1, y2-2);

          painter->setPen(group.midlight());
          painter->drawLine(x+1, y+1, x2-2, y+1);
          if ((selected) || edge) painter->drawLine(x+1, y+2, x+1, y2-2);

          // finish off bottom
          if (selected) {
              painter->setPen(group.dark());
              painter->drawPoint(x, y2-1);
              painter->drawPoint(x2, y2-1);

              painter->setPen(group.midlight());
              painter->drawPoint(x, y2);
              painter->drawLine(x+1, y2-1, x+1, y2);
              painter->drawPoint(x2, y2);

              painter->setPen(group.mid());
              painter->drawPoint(x2-1, y2-1);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2-1, x, y2);
                  painter->setPen(group.midlight());
                  painter->drawPoint(x+1, y2);
              }
          } else {
              painter->setPen(group.dark());
              painter->drawLine(x, y2-1, x2, y2-1);

              painter->setPen(group.midlight());
              painter->drawLine(x, y2, x2, y2);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2-1, x, y2);
              }
          }
          if (reverse_ && edge) {
              painter->setPen(group.dark());
              painter->drawPoint(x2, y2);
              painter->setPen(selected ? group.mid() : group.background());
              painter->drawPoint(x2-1, y2);
          }
          break;
      }

      case TQTabBar::RoundedBelow:
      case TQTabBar::TriangularBelow: {
	  // is there a corner widget?
	  tabwidget = ::tqqt_cast<TQTabWidget*>(bar->parent());
	  if (edge && tabwidget
	      && tabwidget->cornerWidget(reverse_ ?
					 TQt::BottomRight : TQt::BottomLeft)) {
	      edge = false;
	  }

          painter->setBrush((selected || (flags & Style_MouseOver))
                            ? group.background()
                            : TQColor(group.background().dark(contrast)));
          painter->setPen(TQt::NoPen);
          painter->fillRect(x+1, y+1, w-1, h-1, painter->brush());

          // draw tab
          painter->setPen(group.dark());
          painter->drawLine(x, y+1, x, y2);
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2-1);

          painter->setPen(group.mid());
          painter->drawLine(x2-1, y+1, x2-1, y2-1);
          painter->drawLine(x+2, y2-1, x2-1, y2-1);
          painter->drawPoint(x, y);
          painter->drawPoint(x2, y);

          if ((selected) || edge) {
              painter->setPen(group.midlight());
              painter->drawLine(x+1, y+1, x+1, y2-2);
          }

          // finish off top
          if (selected) {
              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawPoint(x, y);
                  painter->setPen(group.midlight());
                  painter->drawPoint(x+1, y);
              }
          } else {
              painter->setPen(group.dark());
              painter->drawLine(x, y+1, x2, y+1);

              painter->setPen(group.mid());
              painter->drawLine(x, y, x2, y);

              if (!reverse_ && edge) {
                  painter->setPen(group.dark());
                  painter->drawPoint(x, y);
              }
          }
          if (reverse_ && edge) {
              painter->setPen(group.dark());
              painter->drawPoint(x2, y);
              painter->setPen(group.mid());
              painter->drawPoint(x2-1, y);
          }
          break;
      }
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// tqdrawPrimitive()
// ---------------
// Draw the primitive element

void PhaseStyle::tqdrawPrimitive(TQ_PrimitiveElement element,
                               TQPainter *painter,
                               const TQRect &rect,
                               const TQColorGroup &group,
                               SFlags flags,
                               const TQStyleOption &option) const
{
    // common locals
    bool down      = flags & Style_Down;
    bool on        = flags & Style_On;
    bool depress   = (down || on);
    bool enabled   = flags & Style_Enabled;
    bool horiz     = flags & Style_Horizontal;
    bool mouseover = highlights_ && (flags & Style_MouseOver);
    int x, y, w, h, x2, y2, n, cx, cy;
    TQPointArray parray;
    TQWidget* widget;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch(element) {
      case PE_ButtonBevel:
      case PE_ButtonDefault:
      case PE_ButtonDropDown:
      case PE_ButtonTool:
          drawPhaseBevel(painter, x,y,w,h, group, group.button(),
                         depress, false, false);
          break;

      case PE_ButtonCommand:
              drawPhaseButton(painter, x, y, w, h, group,
                              mouseover ?
                              TQColor(group.button().light(contrast)) :
                              group.button(), depress);
          break;

      case PE_FocusRect: {
          TQPen old = painter->pen();
          painter->setPen(group.highlight().dark(contrast));

          painter->drawRect(rect);

          painter->setPen(old);
          break;
      }

      case PE_HeaderSection: {
          // covers kicker taskbar buttons and menu titles
          TQHeader* header = dynamic_cast<TQHeader*>(painter->device());
          widget =dynamic_cast<TQWidget*>(painter->device());

          if (header) {
              horiz = (header->orientation() ==Qt::Horizontal);
          } else {
              horiz = true;
          }

          if ((widget) && ((widget->inherits(TQPOPUPMENU_OBJECT_NAME_STRING)) ||
                           (widget->inherits("KPopupTitle")))) {
              // kicker/kdesktop menu titles
              drawPhaseBevel(painter, x,y,w,h,
                             group, group.background(), depress, !horiz);
          } else if (kicker_) {
              // taskbar buttons (assuming no normal headers used in kicker)
              if (depress) {
                  painter->setPen(group.dark());
                  painter->setBrush(group.brush(TQColorGroup::Mid));
                  painter->drawRect(x-1, y-1, w+1, h+1);
              }
              else {
                  drawPhaseBevel(painter, x-1, y-1, w+1, h+1,
                                 group, group.button(), false, !horiz, true);
              }
          } else {
              // other headers
              if (depress) {
                  painter->setPen(group.dark());
                  painter->setBrush(group.brush(TQColorGroup::Mid));
                  painter->drawRect(x-1, y-1, w+1, h+1);
              }
              else {
                  drawPhaseBevel(painter, x-1, y-1, w+1, h+1, group,
                                 group.background(), false, !horiz, true);
              }
          }
          break;
      }

      case PE_HeaderArrow:
          if (flags & Style_Up)
              tqdrawPrimitive(PE_ArrowUp, painter, rect, group, Style_Enabled);
          else
              tqdrawPrimitive(PE_ArrowDown, painter, rect, group, Style_Enabled);
          break;

      case PE_ScrollBarAddPage:
      case PE_ScrollBarSubPage:
          if (h) { // has a height, thus visible
              painter->fillRect(rect, group.mid());
              painter->setPen(group.dark());
              if (horiz) { // vertical
                  painter->drawLine(x, y, x2, y);
                  painter->drawLine(x, y2, x2, y2);
              } else { // horizontal
                  painter->drawLine(x, y, x, y2);
                  painter->drawLine(x2, y, x2, y2);
              }
          }
          break;

      case PE_ScrollBarAddLine:
      case PE_ScrollBarSubLine: {
          drawPhaseBevel(painter, x, y, w, h,
                         group, group.button(), down, !horiz, true);

          TQ_PrimitiveElement arrow = ((horiz) ?
                                    ((element == PE_ScrollBarAddLine) ?
                                     PE_ArrowRight : PE_ArrowLeft) :
                                    ((element == PE_ScrollBarAddLine) ?
                                     PE_ArrowDown : PE_ArrowUp));
          if (down) { // shift arrow
              switch (arrow) {
                case PE_ArrowRight: x++; break;
                case PE_ArrowLeft:  x--; break;
                case PE_ArrowDown:  y++; break;
                case PE_ArrowUp:    y--; break;
                default: break;
              }
          }

          tqdrawPrimitive(arrow, painter, TQRect(x,y,h,w), group, flags);
          break;
      }

      case PE_ScrollBarSlider:
          drawPhaseBevel(painter, x, y, w, h, group, group.button(),
                         false, !horiz, true);
          // draw doodads
          cx = x + w/2 - 2; cy = y + h/2 - 2;
          if (horiz && (w >=20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          } else if (!horiz && (h >= 20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          }
          break;

      case PE_Indicator:
          drawPhasePanel(painter, x+1, y+1, w-2, h-2, group, true, enabled ?
                         &group.tqbrush(TQColorGroup::Base) :
                         &group.tqbrush(TQColorGroup::Background));

          if (on) {
              painter->setPen(mouseover
                              ? TQColor(group.highlight().dark(contrast))
                              : group.dark());
              painter->drawRect(x+4, y+4, w-8, h-8);
              painter->fillRect(x+5, y+5, w-10, h-10,
                                group.tqbrush(TQColorGroup::Highlight));
          } else if (mouseover) {
              painter->setPen(TQColor(group.highlight().dark(contrast)));
              painter->drawRect(x+4, y+4, w-8, h-8);
          }
          break;

      case PE_IndicatorMask:
          painter->fillRect(x+1, y+1, w-2, h-2, TQt::color1);
          painter->setPen(TQt::color0);
          break;

      case PE_ExclusiveIndicator: {
          // note that this requires an even size from tqpixelMetric
          cx = (x + x2) / 2;
          cy = (y + y2) / 2;

          painter->setBrush(enabled
                            ? group.brush(TQColorGroup::Base)
                            : group.brush(TQColorGroup::Background));

          painter->setPen(group.dark());
          parray.putPoints(0, 8,
                           x+1,cy+1, x+1,cy,    cx,y+1,    cx+1,y+1,
                           x2-1,cy,  x2-1,cy+1, cx+1,y2-1, cx,y2-1);
          painter->tqdrawConvexPolygon(parray, 0, 8);

          painter->setPen(group.mid());
          parray.putPoints(0, 4, x,cy, cx,y, cx+1,y, x2,cy);
          painter->tqdrawPolyline(parray, 0, 4);
          painter->setPen(group.midlight());
          parray.putPoints(0, 4, x2,cy+1, cx+1,y2, cx,y2, x,cy+1);
          painter->tqdrawPolyline(parray, 0, 4);

          if (on) {
              painter->setBrush(group.brush(TQColorGroup::Highlight));
              painter->setPen(mouseover
                              ? TQColor(group.highlight().dark(contrast))
                              : group.dark());
              parray.putPoints(0, 8,
                               x+4,cy+1, x+4,cy,    cx,y+4,    cx+1,y+4,
                               x2-4,cy,  x2-4,cy+1, cx+1,y2-4, cx,y2-4);
              painter->tqdrawConvexPolygon(parray, 0, 8);
          } else if (mouseover) {
              painter->setPen(TQColor(group.highlight().dark(contrast)));
              parray.putPoints(0, 9,
                               x+4,cy+1, x+4,cy,    cx,y+4,    cx+1,y+4,
                               x2-4,cy,  x2-4,cy+1, cx+1,y2-4, cx,y2-4,
                               x+4,cy+1);
              painter->tqdrawPolyline(parray, 0, 9);
          }
          break;
      }

      case PE_ExclusiveIndicatorMask:
          cx = (x + x2) / 2;
          cy = (y + y2) / 2;
          painter->setBrush(TQt::color1);
          painter->setPen(TQt::color1);
          parray.putPoints(0, 8,
                           x,cy+1, x,cy,    cx,y,    cx+1,y,
                           x2,cy,  x2,cy+1, cx+1,y2, cx,y2);
          painter->tqdrawConvexPolygon(parray, 0, 8);
          painter->setPen(TQt::color0);
          break;

      case PE_DockWindowResizeHandle:
          drawPhasePanel(painter, x, y, w, h, group, false,
                         &group.tqbrush(TQColorGroup::Background));
          break;

      case PE_Splitter:
          cx = x + w/2 - 2; cy = y + h/2 - 2;
          painter->fillRect(rect,
                            (hover_ == painter->device())
                            ? TQColor(group.background().light(contrast))
                            : group.background());

          if (!horiz && (w >=20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          } else if (horiz && (h >= 20)) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          }
          break;

      case PE_Panel:
      case PE_PanelLineEdit:
      case PE_PanelTabWidget:
      case PE_TabBarBase:
          drawPhasePanel(painter, x, y, w, h, group, flags & Style_Sunken);
          break;

      case PE_PanelPopup:
      case PE_WindowFrame:
          drawPhasePanel(painter, x, y, w, h, group, false);
          break;

      case PE_GroupBoxFrame:
      case PE_PanelGroupBox:
          painter->setPen(group.dark());
          painter->drawRect(rect);
          break;

      case PE_Separator:
          painter->setPen(group.dark());
          if (w < h)
              painter->drawLine(w/2, y, w/2, y2);
          else
              painter->drawLine(x, h/2, x2, h/2);
          break;

      case PE_StatusBarSection:
          painter->setPen(group.mid());
          painter->drawLine(x, y,  x2-1, y);
          painter->drawLine(x, y+1, x, y2-1);
          painter->setPen(group.midlight());
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2-1);
          break;

      case PE_PanelMenuBar:
      case PE_PanelDockWindow:
          if (kicker_ && (w == 2)) { // kicker handle separator
              painter->setPen(group.mid());
              painter->drawLine(x, y,  x, y2);
              painter->setPen(group.midlight());
              painter->drawLine(x+1, y,  x+1, y2);
          } else if (kicker_ && (h == 2)) { // kicker handle separator
              painter->setPen(group.mid());
              painter->drawLine(x, y,  x2, y);
              painter->setPen(group.midlight());
              painter->drawLine(x, y+1,  x2, y+1);
          } else {
              --x; --y; ++w; ++h; // adjust rect so we can use bevel
              drawPhaseBevel(painter, x, y, w, h,
                             group, group.background(), false, (w < h),
                             (element==PE_PanelDockWindow) ? true : false);
          }
          break;

      case PE_DockWindowSeparator: {
          widget = dynamic_cast<TQWidget*>(painter->device());
          bool flat = true;

          if (widget && widget->parent() &&
              widget->parent()->inherits(TQTOOLBAR_OBJECT_NAME_STRING)) {
              TQToolBar *toolbar = ::tqqt_cast<TQToolBar*>(widget->parent());
              if (toolbar) {
                  // toolbar not floating or in a TQMainWindow
                  flat = flatToolbar(toolbar);
              }
          }

          if (flat)
              painter->fillRect(rect, group.background());
          else
              drawPhaseGradient(painter, rect, group.background(),
                                !(horiz), 0, 0, -1, -1, true);

          if (horiz) {
              int cx = w/2 - 1;
              painter->setPen(group.mid());
              painter->drawLine(cx, 0, cx, h-2);
              if (!flat) painter->drawLine(0, h-1, w-1, h-1);

              painter->setPen(group.midlight());
              painter->drawLine(cx+1, 0, cx+1, h-2);
          } else {
              int cy = h/2 - 1;
              painter->setPen(group.mid());
              painter->drawLine(0, cy, w-2, cy);
              if (!flat) painter->drawLine(w-1, 0, w-1, h-1);

              painter->setPen(group.midlight());
              painter->drawLine(0, cy+1, w-2, cy+1);
          }
          break;
      }

      case PE_SizeGrip: {
          int sw = TQMIN(h, w) - 1;
          y = y2 - sw;

          if (reverse_) {
              x2 = x + sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y, x2, y2);
                  painter->setPen(group.midlight());
                  painter->drawLine(x, y+1, x2-1, y2);
                  y += 3;;
                  x2 -= 3;;
              }
          } else {
              x = x2 - sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2, x2, y);
                  painter->setPen(group.midlight());
                  painter->drawLine(x+1, y2, x2, y+1);
                  x += 3;
                  y += 3;
              }
          }

          break;
      }

      case PE_CheckMark:
          painter->setPen(group.text());
          painter->drawPixmap(x+w/2-4, y+h/2-4, bcheck);
          break;

      case PE_SpinWidgetPlus:
      case PE_SpinWidgetMinus:
          if (enabled)
              painter->setPen((flags & Style_Sunken)
                              ? group.midlight() : group.dark());
          else
              painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3,
                              (element==PE_SpinWidgetPlus) ? bplus : bminus);
          break;

      case PE_SpinWidgetUp:
      case PE_ArrowUp:
          if (flags & Style_Enabled)
              painter->setPen((flags & Style_Sunken)
                              ? group.midlight() : group.dark());
          else
              painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, uarrow);
          break;

      case PE_SpinWidgetDown:
      case PE_ArrowDown:
          if (enabled) painter->setPen((flags & Style_Sunken)
                                       ? group.midlight() : group.dark());
          else painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, darrow);
          break;

      case PE_ArrowLeft:
          if (enabled) painter->setPen((flags & Style_Sunken)
                                       ? group.midlight() : group.dark());
          else painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, larrow);
          break;

      case PE_ArrowRight:
          if (enabled) painter->setPen((flags & Style_Sunken)
                                       ? group.midlight() : group.dark());
          else painter->setPen(group.mid());
          painter->drawPixmap(x+w/2-3, y+h/2-3, rarrow);
          break;

      default:
          KStyle::tqdrawPrimitive(element, painter, rect, group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawKStylePrimitive()
// ---------------------
// Draw the element

void PhaseStyle::drawKStylePrimitive(KStylePrimitive element,
                                     TQPainter *painter,
                                     const TQWidget *widget,
                                     const TQRect &rect,
                                     const TQColorGroup &group,
                                     SFlags flags,
                                     const TQStyleOption &option) const
{
    bool horiz = flags & Style_Horizontal;
    int x, y, w, h, x2, y2, n, cx, cy;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();
    cx = x + w/2;
    cy = y + h/2;

    switch (element) {
      case KPE_ToolBarHandle:
          cx-=2; cy-=2;
          drawPhaseGradient(painter, rect, group.background(),
                            !horiz, 0, 0, w-1, h-1, true);
          if (horiz) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
              painter->setPen(group.mid());
              painter->drawLine(x, y2, x2, y2);
          } else {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
              painter->setPen(group.mid());
              painter->drawLine(x2, y, x2, y2);
          }
          break;

      //case KPE_DockWindowHandle:
      case KPE_GeneralHandle:
          cx-=2; cy-=2;
          painter->fillRect(rect, group.brush(TQColorGroup::Background));

          if (horiz) {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx, cy+n,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          } else {
              for (n = -5; n <= 5; n += 5) {
                  kColorBitmaps(painter, group, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
              }
          }
          break;

      case KPE_ListViewExpander:
          painter->setPen(group.mid());
          if (flags & Style_On) {
              painter->drawPixmap(x+w/2-4, y+h/2-4, rexpand);
          } else {
              painter->drawPixmap(x+w/2-4, y+h/2-4, dexpand);
          }
          break;

      case KPE_ListViewBranch:
          painter->setPen(group.mid());
          if (flags & Style_Horizontal) {
              painter->drawLine(x, cy, x2, cy);
          } else {
              painter->drawLine(cx, y, cx, y2);
          }
          break;

      case KPE_SliderGroove: {
          const TQSlider* slider = ::tqqt_cast<const TQSlider*>(widget);
          if (slider) {
              if (slider->orientation() ==Qt::Horizontal) {
                  y = cy - 3;
                  h = 7;
              } else {
                  x = cx - 3;
                  w = 7;
              }
          }
          drawPhasePanel(painter, x, y, w, h, group, true,
                         &group.tqbrush(TQColorGroup::Mid));
          break;
      }

      case KPE_SliderHandle: {
          const TQSlider* slider = ::tqqt_cast<const TQSlider*>(widget);
          if (slider) {
              TQColor color = (widget==hover_)
                  ? TQColor(group.button().light(contrast))
                  : group.button();
              if (slider->orientation() ==Qt::Horizontal) {
                  drawPhaseBevel(painter, cx-5, y, 6, h, group, color,
                                 false, false,  false);
                  drawPhaseBevel(painter, cx, y, 6, h, group, color,
                                 false, false, false);
              } else {
                  drawPhaseBevel(painter, x, cy-5, w, 6, group, color,
                                 false, true, false);
                  drawPhaseBevel(painter, x, cy, w, 6, group, color,
                                 false, true, false);
              }
          }
          break;
      }

      default:
          KStyle::drawKStylePrimitive(element, painter, widget, rect,
                                      group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// tqdrawControl()
// -------------
// Draw the control

void PhaseStyle::tqdrawControl(TQ_ControlElement element,
                             TQPainter *painter,
                             const TQWidget *widget,
                             const TQRect &rect,
                             const TQColorGroup &group,
                             SFlags flags,
                             const TQStyleOption &option) const
{
    bool active, enabled, depress;
    int x, y, w, h, x2, y2, dx;
    TQMenuItem *mi;
    TQIconSet::Mode mode;
    TQIconSet::State state;
    TQPixmap pixmap;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch (element) {
      case CE_PushButton: {
          depress = flags & (Style_Down | Style_On);
          int bd = tqpixelMetric(PM_ButtonDefaultIndicator, widget) + 1;

          if ((flags & Style_ButtonDefault) && !depress) {
              drawPhasePanel(painter, x, y, w, h, group, true,
                             &group.tqbrush(TQColorGroup::Mid));
              drawPhaseBevel(painter, x+bd, y+bd, w-bd*2, h-bd*2, group,
                             (widget==hover_)
                             ? TQColor(group.button().light(contrast))
                             : group.button(),
                             false, false, false);
          } else {
              drawPhaseButton(painter, x, y, w, h, group,
                              (widget==hover_)
                              ? TQColor(group.button().light(contrast))
                              : group.button(), depress);
          }

          if (flags & Style_HasFocus) { // draw focus
              tqdrawPrimitive(PE_FocusRect, painter,
                            subRect(SR_PushButtonFocusRect, widget),
                            group, flags);
          }
          break;
      }

      case CE_PushButtonLabel: {
          const TQPushButton* button = ::tqqt_cast<const TQPushButton*>(widget);
          if (!button) {
              KStyle::tqdrawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          active = button->isOn() || button->isDown();

          if (active) { // shift contents
              x++; y++;
              flags |= Style_Sunken;
          }

          if (button->isMenuButton()) { // menu indicator
              int dx = tqpixelMetric(PM_MenuButtonIndicator, widget);
              tqdrawPrimitive(PE_ArrowDown, painter,
                            TQRect(x+w-dx-2, y+2, dx, h-4),
                            group, flags, option);
              w -= dx;
          }

          if (button->iconSet() && !button->iconSet()->isNull()) { // draw icon
              if (button->isEnabled()) {
                  if (button->hasFocus()) {
                      mode = TQIconSet::Active;
                  } else {
                      mode = TQIconSet::Normal;
                  }
              } else {
                  mode = TQIconSet::Disabled;
              }

              if (button->isToggleButton() && button->isOn()) {
                  state = TQIconSet::On;
              } else {
                  state = TQIconSet::Off;
              }

              pixmap = button->iconSet()->pixmap(TQIconSet::Small, mode, state);
              if (button->text().isEmpty() && !button->pixmap()) {
                  painter->drawPixmap(x+w/2 - pixmap.width()/2,
                                      y+h/2 - pixmap.height()/2, pixmap);
              } else {
                  painter->drawPixmap(x+4, y+h/2 - pixmap.height()/2, pixmap);
              }
              x += pixmap.width() + 4;
              w -= pixmap.width() + 4;
          }

          if (active || button->isDefault()) { // default button
              for(int n=0; n<2; n++) {
                  drawItem(painter, TQRect(x+n, y, w, h),
                           AlignCenter | ShowPrefix,
                           button->tqcolorGroup(),
                           button->isEnabled(),
                           button->pixmap(),
                           button->text(), -1,
                           (button->isEnabled()) ?
                           &button->tqcolorGroup().buttonText() :
                           &button->tqcolorGroup().mid());
              }
          } else { // normal button
              drawItem(painter, TQRect(x, y, w, h),
                       AlignCenter | ShowPrefix,
                       button->tqcolorGroup(),
                       button->isEnabled(),
                       button->pixmap(),
                       button->text(), -1,
                       (button->isEnabled()) ?
                       &button->tqcolorGroup().buttonText() :
                       &button->tqcolorGroup().mid());
          }
          break;
      }

      case CE_CheckBoxLabel:
      case CE_RadioButtonLabel: {
          const TQButton *b = ::tqqt_cast<const TQButton*>(widget);
          if (!b) return;

          int tqalignment = reverse_ ? AlignRight : AlignLeft;
          drawItem(painter, rect, tqalignment | AlignVCenter | ShowPrefix,
                   group, flags & Style_Enabled, b->pixmap(), b->text());

          // only draw focus if content (forms on html won't)
          if ((flags & Style_HasFocus) && ((!b->text().isNull()) || b->pixmap())) {
              tqdrawPrimitive(PE_FocusRect, painter,
                            tqvisualRect(subRect(SR_RadioButtonFocusRect,
                                               widget), widget),
                            group, flags);
          }
          break;
      }

      case CE_DockWindowEmptyArea:  {
          const TQToolBar *tb = ::tqqt_cast<const TQToolBar*>(widget);
          if (tb) {
              // toolbar not floating or in a TQMainWindow
              if (flatToolbar(tb)) {
                  if (tb->backgroundMode() == PaletteButton)
                      // force default button color to background color
                      painter->fillRect(rect, group.background());
                  else
                      painter->fillRect(rect, tb->paletteBackgroundColor());
              }
          }
          break;
      }

      case CE_MenuBarEmptyArea:
          drawPhaseGradient(painter, TQRect(x, y, w, h), group.background(),
                            (w<h), 0, 0, 0, 0, false);
          break;

      case CE_MenuBarItem: {
          const TQMenuBar *mbar = ::tqqt_cast<const TQMenuBar*>(widget);
          if (!mbar) {
              KStyle::tqdrawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          mi = option.menuItem();
          TQRect prect = mbar->rect();

          if ((flags & Style_Active) && (flags & Style_HasFocus)) {
              if (flags & Style_Down) {
                  drawPhasePanel(painter, x, y, w, h, group, true,
                                 &group.tqbrush(TQColorGroup::Background));
              } else {
                  drawPhaseBevel(painter, x, y, w, h,
                                 group, group.background(),
                                 false, false, false);
              }
          } else {
              drawPhaseGradient(painter, rect, group.background(), false, x, y,
                                prect.width()-2, prect.height()-2, false);
          }
          drawItem(painter, rect,
                   AlignCenter | AlignVCenter |
                   DontClip | ShowPrefix | SingleLine,
                   group, flags & Style_Enabled,
                   mi->pixmap(), mi->text());
          break;
      }

      case CE_PopupMenuItem: {
          const TQPopupMenu *popup = ::tqqt_cast<const TQPopupMenu*>(widget);
          if (!popup) {
              KStyle::tqdrawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }

          mi = option.menuItem();
          if (!mi) {
              painter->fillRect(rect, group.button());
              break;
          }

          int tabwidth   = option.tabWidth();
          int checkwidth = option.maxIconWidth();
          bool checkable = popup->isCheckable();
          bool etchtext  = tqstyleHint(SH_EtchDisabledText);
          active         = flags & Style_Active;
          enabled        = mi->isEnabled();
          TQRect vrect;

          if (checkable) checkwidth = TQMAX(checkwidth, 20);

          // draw background
          if (active && enabled) {
              painter->fillRect(x, y, w, h, group.highlight());
          } else if (widget->erasePixmap() &&
                     !widget->erasePixmap()->isNull()) {
              painter->drawPixmap(x, y, *widget->erasePixmap(), x, y, w, h);
          } else {
              painter->fillRect(x, y, w, h, group.background());
          }

          // draw separator
          if (mi->isSeparator()) {
              painter->setPen(group.dark());
              painter->drawLine(x+checkwidth+1, y+1, x2-checkwidth-1, y+1);
              painter->setPen(group.mid());
              painter->drawLine(x+checkwidth, y, x2-checkwidth-1, y);
              painter->drawPoint(x+checkwidth, y+1);
              painter->setPen(group.midlight());
              painter->drawLine(x+checkwidth+1, y2, x2-checkwidth, y2);
              painter->drawPoint(x2-checkwidth, y2-1);
              break;
          }

          // draw icon
          if (mi->iconSet() && !mi->isChecked()) {
              if (active)
                  mode = enabled ? TQIconSet::Active : TQIconSet::Disabled;
              else
                  mode = enabled ? TQIconSet::Normal : TQIconSet::Disabled;

              pixmap = mi->iconSet()->pixmap(TQIconSet::Small, mode);
              TQRect pmrect(0, 0, pixmap.width(), pixmap.height());
              vrect = tqvisualRect(TQRect(x, y, checkwidth, h), rect);
              pmrect.moveCenter(vrect.center());
              painter->drawPixmap(pmrect.topLeft(), pixmap);
          }

          // draw check
          if (mi->isChecked()) {
              int cx = reverse_ ? x+w - checkwidth : x;
              tqdrawPrimitive(PE_CheckMark, painter,
                            TQRect(cx + ITEMFRAME, y + ITEMFRAME,
                                  checkwidth - ITEMFRAME*2, h - ITEMFRAME*2),
                            group, Style_Default |
                            (active ? Style_Enabled : Style_On));
          }

          // draw text
          int xm = ITEMFRAME + checkwidth + ITEMHMARGIN;
          int xp = reverse_ ?
              x + tabwidth + RIGHTBORDER + ITEMHMARGIN + ITEMFRAME - 1 :
              x + xm;
          int offset = reverse_ ? -1 : 1;
          int tw = w - xm - tabwidth - ARROWMARGIN - ITEMHMARGIN * 3
              - ITEMFRAME + 1;

          painter->setPen(enabled ? (active ? group.highlightedText() :
                                     group.buttonText()) : group.mid());

          if (mi->custom()) { // draws own label
              painter->save();
              if (etchtext && !enabled && !active) {
                  painter->setPen(group.light());
                  mi->custom()->paint(painter, group, active, enabled,
                                      xp+offset, y+ITEMVMARGIN+1,
                                      tw, h-2*ITEMVMARGIN);
                  painter->setPen(group.mid());
              }
              mi->custom()->paint(painter, group, active, enabled,
                                  xp, y+ITEMVMARGIN, tw, h-2*ITEMVMARGIN);
              painter->restore();
          }
          else { // draw label
              TQString text = mi->text();

              if (!text.isNull()) {
                  int t = text.find('\t');
                  int tflags = AlignVCenter | DontClip |
                      ShowPrefix |  SingleLine |
                      (reverse_ ? AlignRight : AlignLeft);

                  if (t >= 0) {
                      int tabx = reverse_ ?
                          x + RIGHTBORDER + ITEMHMARGIN + ITEMFRAME :
                          x + w - tabwidth - RIGHTBORDER - ITEMHMARGIN
                          - ITEMFRAME;

                      // draw right label (accerator)
                      if (etchtext && !enabled) { // etched
                          painter->setPen(group.light());
                          painter->drawText(tabx+offset, y+ITEMVMARGIN+1,
                                            tabwidth, h-2*ITEMVMARGIN,
                                            tflags, text.mid(t+1));
                          painter->setPen(group.mid());
                      }
                      painter->drawText(tabx, y+ITEMVMARGIN,
                                        tabwidth, h-2*ITEMVMARGIN,
                                        tflags, text.mid(t+1));
                      text = text.left(t);
                  }

                  // draw left label
                  if (etchtext && !enabled) { // etched
                      painter->setPen(group.light());
                      painter->drawText(xp+offset, y+ITEMVMARGIN+1,
                                        tw, h-2*ITEMVMARGIN,
                                        tflags, text, t);
                      painter->setPen(group.mid());
                  }
                  painter->drawText(xp, y+ITEMVMARGIN,
                                    tw, h-2*ITEMVMARGIN,
                                    tflags, text, t);
              }
              else if (mi->pixmap()) { // pixmap as label
                  pixmap = *mi->pixmap();
                  if (pixmap.depth() == 1)
                      painter->setBackgroundMode(Qt::OpaqueMode);

                  dx = ((w - pixmap.width()) / 2) + ((w - pixmap.width()) % 2);
                  painter->drawPixmap(x+dx, y+ITEMFRAME, pixmap);

                  if (pixmap.depth() == 1)
                      painter->setBackgroundMode(Qt::TransparentMode);
              }
          }

          if (mi->popup()) { // draw submenu arrow
              TQ_PrimitiveElement arrow = reverse_ ? PE_ArrowLeft : PE_ArrowRight;
              int dim = (h-2*ITEMFRAME) / 2;
              vrect = tqvisualRect(TQRect(x + w - ARROWMARGIN - ITEMFRAME - dim,
                                       y + h / 2 - dim / 2, dim, dim), rect);
              tqdrawPrimitive(arrow, painter, vrect, group,
                            enabled ? Style_Enabled : Style_Default);
          }
          break;
      }

      case CE_TabBarTab: {
          const TQTabBar* tab = ::tqqt_cast<const TQTabBar*>(widget);
          if (tab) {
              if ((widget == hover_) && (option.tab() == hovertab_)) {
                  flags |= Style_MouseOver;
              }
              // this guy can get complicated, we we do it elsewhere
              drawPhaseTab(painter, x, y, w, h, group, tab, option,
                           flags);
          } else { // not a tabbar
              KStyle::tqdrawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          break;
      }

      case CE_ProgressBarGroove: {
          drawPhasePanel(painter, x, y, w, h, group, true,
                         &group.tqbrush(TQColorGroup::Base));
          break;
      }

      case CE_ProgressBarContents: {
          const TQProgressBar* pbar = ::tqqt_cast<const TQProgressBar*>(widget);
          if (!pbar) {
              KStyle::tqdrawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          subRect(SR_ProgressBarContents, widget).rect(&x, &y, &w, &h);

          painter->setBrush(group.brush(TQColorGroup::Highlight));
          painter->setPen(group.dark());

          if (!pbar->totalSteps()) {
              // busy indicator
              int bar = tqpixelMetric(PM_ProgressBarChunkWidth, widget) + 2;
              int progress = pbar->progress() % ((w-bar) * 2);
              if (progress > (w-bar)) progress = 2 * (w-bar) - progress;
              painter->drawRect(x+progress+1, y+1, bar-2, h-2);
          } else {
              double progress = static_cast<double>(pbar->progress()) /
                  static_cast<double>(pbar->totalSteps());
              dx = static_cast<int>(w * progress);
              if (dx < 4) break;
              if (reverse_) x += w - dx;
              painter->drawRect(x+1, y+1, dx-2, h-2);
          }
          break;
      }

      case CE_ToolBoxTab: {
          const TQToolBox *box = ::tqqt_cast<const TQToolBox*>(widget);
          if (!box) {
              KStyle::tqdrawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }

          const int rx = x2 - 20;
          const int cx = rx - h + 1;

          TQPointArray parray(6);
          parray.putPoints(0, 6,
                           x-1,y, cx,y, rx-2,y2-2, x2+1,y2-2,
                           x2+1,y2+2, x-1,y2+2);

          if (box->currentItem() && (flags & Style_Selected)) {
              painter->setPen(group.dark());
              painter->setBrush(box->currentItem()->paletteBackgroundColor());
              painter->tqdrawConvexPolygon(parray, 0, 6);
              painter->setBrush(NoBrush);
          } else {
              painter->setClipRegion(parray, TQPainter::CoordPainter);
              drawPhaseGradient(painter, rect,
                                group.background(),
                                false, 0, 0, 0, h*2, false);
              painter->setClipping(false);
              painter->tqdrawPolyline(parray, 0, 4);
          }

          parray.putPoints(0, 4, x,y+1, cx,y+1, rx-2,y2-1, x2,y2-1);
          painter->setPen(group.midlight());
          painter->tqdrawPolyline(parray, 0, 4);

          break;
      }

      default:
          KStyle::tqdrawControl(element, painter, widget, rect, group,
                              flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// tqdrawControlMask()
// -----------------
// Draw a bitmask for the element

void PhaseStyle::tqdrawControlMask(TQ_ControlElement element,
                                 TQPainter *painter,
                                 const TQWidget *widget,
                                 const TQRect &rect,
                                 const TQStyleOption &option) const
{
    switch (element) {
      case CE_PushButton:
          painter->fillRect(rect, TQt::color1);
          painter->setPen(TQt::color0);
          break;

      default:
          KStyle::tqdrawControlMask(element, painter, widget, rect, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// tqdrawComplexControl()
// --------------------
// Draw a complex control

void PhaseStyle::tqdrawComplexControl(TQ_ComplexControl control,
                                    TQPainter *painter,
                                    const TQWidget *widget,
                                    const TQRect &rect,
                                    const TQColorGroup &group,
                                    SFlags flags,
                                    SCFlags controls,
                                    SCFlags active,
                                    const TQStyleOption &option) const
{
    bool down = flags & Style_Down;
    bool on = flags & Style_On;
    bool raised = flags & Style_Raised;
    bool sunken = flags & Style_Sunken;
    TQRect subrect;
    int x, y, w, h, x2, y2;
    rect.rect(&x, &y, &w, &h);

    switch (control) {
      case CC_ComboBox: {
          const TQComboBox * combo = ::tqqt_cast<const TQComboBox*>(widget);
          if (!combo) {
              KStyle::tqdrawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          sunken = (active == SC_ComboBoxArrow);
          drawPhaseButton(painter, x, y, w, h, group,
                          (widget==hover_)
                          ? TQColor(group.button().light(contrast))
                          : group.button(), sunken);

          if (controls & SC_ComboBoxArrow) { // draw arrow box
              subrect = tqvisualRect(querySubControlMetrics(CC_ComboBox, widget,
                            SC_ComboBoxArrow), widget);

              subrect.rect(&x, &y, &w, &h);
              int slot = TQMAX(h/4, 6) + (h % 2);
              drawPhasePanel(painter, x+3, y+(h/2)-(slot/2), w-6,
                             slot, group, true,
                             sunken ? &group.tqbrush(TQColorGroup::Midlight)
                             : &group.tqbrush(TQColorGroup::Mid));
          }

          if (controls & SC_ComboBoxEditField) { // draw edit box
              if (combo->editable()) { // editable box
                  subrect = tqvisualRect(querySubControlMetrics(CC_ComboBox,
                                widget, SC_ComboBoxEditField), widget);
                  x2 = subrect.right(); y2 = subrect.bottom();
                  painter->setPen(group.dark());
                  painter->drawLine(x2+1, y, x2+1, y2);
                  painter->setPen(group.midlight());
                  painter->drawLine(x2+2, y, x2+2, y2-1);
                  painter->setPen(group.button());
                  painter->drawPoint(x2+2, y2);
              } else if (combo->hasFocus()) { // non editable box
                  subrect = tqvisualRect(subRect(SR_ComboBoxFocusRect,
                                               combo), widget);
                  tqdrawPrimitive(PE_FocusRect, painter, subrect, group,
                                Style_FocusAtBorder,
                                TQStyleOption(group.highlight()));
              }
          }

          painter->setPen(group.buttonText()); // for subsequent text
          break;
      }

      case CC_ScrollBar: {
          // always a three button scrollbar
          const TQScrollBar *sb = ::tqqt_cast<const TQScrollBar*>(widget);
          if (!sb) {
              KStyle::tqdrawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          TQRect srect;
          bool horizontal = (sb->orientation() == Qt::Horizontal);
          SFlags scrollflags = (horizontal ? Style_Horizontal : Style_Default);

          if (sb->minValue() == sb->maxValue()) scrollflags |= Style_Default;
          else scrollflags |= Style_Enabled;

          // addline
          if (controls & SC_ScrollBarAddLine) {
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarAddLine, option);
              if (srect.isValid())
                  tqdrawPrimitive(PE_ScrollBarAddLine, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarAddLine)
                                               ? Style_Down : Style_Default));
          }

          // subline (two of them)
          if (controls & SC_ScrollBarSubLine) {
              // top/left subline
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarSubLine, option);
              if (srect.isValid())
                  tqdrawPrimitive(PE_ScrollBarSubLine, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSubLine)
                                               ? Style_Down : Style_Default));
              // bottom/right subline
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarAddLine, option);
              if (srect.isValid()) {
                  if (horizontal) srect.moveBy(-srect.width()+1, 0);
                  else srect.moveBy(0, -srect.height()+1);
                  tqdrawPrimitive(PE_ScrollBarSubLine, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSubLine)
                                               ? Style_Down : Style_Default));
              }
          }

          // addpage
          if (controls & SC_ScrollBarAddPage) {
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarAddPage, option);
              if (srect.isValid()) {
                  if (horizontal) srect.addCoords(1, 0, 1, 0);
                  else srect.addCoords(0, 1, 0, 1);
                  tqdrawPrimitive(PE_ScrollBarAddPage, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarAddPage)
                                               ? Style_Down : Style_Default));
              }
          }

          // subpage
          if (controls & SC_ScrollBarSubPage) {
              srect = querySubControlMetrics(control, widget,
                                             SC_ScrollBarSubPage, option);
              if (srect.isValid()) {
                  tqdrawPrimitive(PE_ScrollBarSubPage, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSubPage)
                                               ? Style_Down : Style_Default));
              }
          }

          // slider
          if (controls & SC_ScrollBarSlider) {
              if (sb->minValue() == sb->maxValue()) {
                  // maxed out
                  srect = querySubControlMetrics(control, widget,
                                                 SC_ScrollBarGroove, option);
              } else {
                  srect = querySubControlMetrics(control, widget,
                                                 SC_ScrollBarSlider, option);
              }
              if (srect.isValid()) {
                  if (horizontal) srect.addCoords(0, 0, 1, 0);
                  else srect.addCoords(0, 0, 0, 1);
                  tqdrawPrimitive(PE_ScrollBarSlider, painter, srect, group,
                                scrollflags | ((active == SC_ScrollBarSlider)
                                               ? Style_Down : Style_Default));
                  // focus
                  if (sb->hasFocus()) {
                      srect.addCoords(2, 2, -2, -2);
                      tqdrawPrimitive(PE_FocusRect, painter, srect, group,
                                    Style_Default);
                  }
              }
          }
          break;
      }

      case CC_SpinWidget: {
          const TQSpinWidget *spin = ::tqqt_cast<const TQSpinWidget*>(widget);
          if (!spin) {
              KStyle::tqdrawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          TQ_PrimitiveElement element;

          // draw frame
          if (controls & SC_SpinWidgetFrame) {
              drawPhasePanel(painter, x, y, w, h, group, true, NULL);
          }

          // draw button field
          if (controls & SC_SpinWidgetButtonField) {
              subrect = querySubControlMetrics(CC_SpinWidget, widget,
                                               SC_SpinWidgetButtonField,
                                               option);
              if (reverse_) subrect.moveLeft(spin->upRect().left());
              drawPhaseBevel(painter, subrect.x(), subrect.y(),
                             subrect.width(), subrect.height(), group,
                             (widget==hover_)
                             ? TQColor(group.button().light(contrast))
                             : group.button(), false, false, false);
          }

          // draw up arrow
          if (controls & SC_SpinWidgetUp) {
              subrect = spin->upRect();

              sunken = (active == SC_SpinWidgetUp);
              if (spin->buttonSymbols() == TQSpinWidget::PlusMinus)
                  element = PE_SpinWidgetPlus;
              else
                  element = PE_SpinWidgetUp;

              tqdrawPrimitive(element, painter, subrect, group, flags
                            | ((active == SC_SpinWidgetUp)
                               ? Style_On | Style_Sunken : Style_Raised));
          }

          // draw down button
          if (controls & SC_SpinWidgetDown) {
              subrect = spin->downRect();

              sunken = (active == SC_SpinWidgetDown);
              if (spin->buttonSymbols() == TQSpinWidget::PlusMinus)
                  element = PE_SpinWidgetMinus;
              else
                  element = PE_SpinWidgetDown;

              tqdrawPrimitive(element, painter, subrect, group, flags
                            | ((active == SC_SpinWidgetDown)
                               ? Style_On | Style_Sunken : Style_Raised));
          }
          break;
      }

      case CC_ToolButton: {
          const TQToolButton *btn = ::tqqt_cast<const TQToolButton*>(widget);
          if (!btn) {
              KStyle::tqdrawComplexControl(control, painter, widget, rect, group,
                                         flags, controls, active, option);
              return;
          }

          TQToolBar *toolbar;
          bool horiz = true;
          bool normal = !(down || on || raised); // normal button state

          x2 = rect.right();
          y2 = rect.bottom();

          // check for TQToolBar parent
          if (btn->parent() && btn->parent()->inherits(TQTOOLBAR_OBJECT_NAME_STRING)) {
              toolbar = ::tqqt_cast<TQToolBar*>(btn->parent());
              if (toolbar) {
                  horiz = (toolbar->orientation() == Qt::Horizontal);
                  if (normal) { // draw background
                      if (flatToolbar(toolbar)) {
                          // toolbar not floating or in a TQMainWindow
                          painter->fillRect(rect, group.background());
                      } else {
                          drawPhaseGradient(painter, rect, group.background(),
                                            !horiz, 0, 0,
                                            toolbar->width()-3,
                                            toolbar->height()-3, true);
                          painter->setPen(group.mid());
                          if (horiz) {
                              painter->drawLine(x, y2, x2, y2);
                          } else {
                              painter->drawLine(x2, y, x2, y2);
                          }
                      }
                  }
              }
          }
          // check for TQToolBarExtensionWidget parent
          else if (btn->parent() &&
                   btn->parent()->inherits(TQTOOLBAREXTENSION)) {
              TQWidget *extension;
              if ((extension = ::tqqt_cast<TQWidget*>(btn->parent()))) {
                  toolbar = ::tqqt_cast<TQToolBar*>(extension->parent());
                  if (toolbar) {
                      horiz = (toolbar->orientation() == Qt::Horizontal);
                      if (normal) { // draw background
                          drawPhaseGradient(painter, rect, group.background(),
                                            !horiz, 0, 0, toolbar->width()-3,
                                            toolbar->height()-3, true);
                      }
                  }
              }
          }
          // check for background pixmap
          else if (normal && btn->parentWidget() &&
                   btn->parentWidget()->backgroundPixmap() &&
                   !btn->parentWidget()->backgroundPixmap()->isNull()) {
              TQPixmap pixmap = *(btn->parentWidget()->backgroundPixmap());
              painter->drawTiledPixmap(rect, pixmap, btn->pos());
          }
          // everything else
          else if (normal) {
              // toolbutton not on a toolbar
              painter->fillRect(rect, group.background());
          }

          // now draw active buttons
          if (down || on) {
              drawPhasePanel(painter, x, y, w, h, group, true,
                             &group.tqbrush(TQColorGroup::Button));
          } else if (raised) {
              drawPhaseBevel(painter, x, y, w, h, group, group.button(),
                             false, !horiz, true);
          }
          painter->setPen(group.text());
          break;
      }

      default:
          KStyle::tqdrawComplexControl(control, painter, widget, rect, group,
                                     flags, controls, active, option);
          break;
    }
}

//////////////////////////////////////////////////////////////////////////////
// tqdrawComplexControlMask()
// ------------------------
// Draw a bitmask for the control

void PhaseStyle::tqdrawComplexControlMask(TQ_ComplexControl control,
                                        TQPainter *painter,
                                        const TQWidget *widget,
                                        const TQRect &rect,
                                        const TQStyleOption &option) const
{
    switch (control) {
      case CC_ComboBox:
      case CC_ToolButton: {
          painter->fillRect(rect, TQt::color1);
          painter->setPen(TQt::color0);
          break;
      }

      default:
          KStyle::tqdrawComplexControlMask(control,painter,widget,rect,option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// tqpixelMetric()
// -------------
// Get the pixel metric for metric

int PhaseStyle::tqpixelMetric(PixelMetric metric, const TQWidget *widget) const
{
    // not using widget's font, so that all metrics are uniform
    int em = TQMAX(TQApplication::fontMetrics().strikeOutPos() * 3, 17);

    switch (metric) {
      case PM_DefaultFrameWidth:
          return 2;

      case PM_ButtonDefaultIndicator:   // size of default indicator
          return 2;

      case PM_ButtonMargin:             // Space betweeen frame and label
          return 3;

      case PM_TabBarTabOverlap:         // Amount of tab overlap
          return 1;

      case PM_TabBarTabHSpace:          // extra tab spacing
          return 24;

      case PM_TabBarTabVSpace:
          if (const TQTabBar *tb = ::tqqt_cast<const TQTabBar*>(widget)) {
              if (tb->tqshape() == TQTabBar::RoundedAbove) {
                  return 10;
              } else {
                  return 6;
              }
          }
          return 0;

      case PM_ExclusiveIndicatorWidth:  // radiobutton size
      case PM_ExclusiveIndicatorHeight:
      case PM_IndicatorWidth:           // checkbox size
      case PM_IndicatorHeight:
      case PM_CheckListButtonSize:	// checkbox size in qlistview items
      case PM_ScrollBarExtent:          // base width of a vertical scrollbar
          return em & 0xfffe;

      case PM_SplitterWidth:            // width of spitter
          return (em / 3) & 0xfffe;

      case PM_ScrollBarSliderMin:       // minimum length of slider
          return  em * 2;

      case PM_SliderThickness:          // slider thickness
      case PM_SliderControlThickness:
          return em;

      default:
          return KStyle::tqpixelMetric(metric, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// subRect()
// ---------
// Return subrect for the widget in logical coordinates

TQRect PhaseStyle::subRect(SubRect rect, const TQWidget *widget) const
{
    switch (rect) {
      case SR_ComboBoxFocusRect: {
          TQRect r = querySubControlMetrics(CC_ComboBox, widget,
                                           SC_ComboBoxEditField);
          r.addCoords(1, 1,-1,-1);
          return r;
      }

      default:
          return KStyle::subRect(rect, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// querySubControlMetrics()
// ------------------------
// Get metrics for subcontrols of complex controls

TQRect PhaseStyle::querySubControlMetrics(TQ_ComplexControl control,
                                         const TQWidget *widget,
                                         SubControl subcontrol,
                                         const TQStyleOption &option) const
{
    TQRect rect;

    const int fw = tqpixelMetric(PM_DefaultFrameWidth, widget);
    int w = widget->width(), h = widget->height();
    int xc;

    switch (control) {

      case CC_ComboBox: {
          xc = h; // position between edit and arrow

          switch (subcontrol) {
            case SC_ComboBoxFrame: // total combobox area
                rect = widget->rect();
                break;

            case SC_ComboBoxArrow: // the right side
                rect.setRect(w-xc, fw, xc-fw, h-(fw*2));
                break;

            case SC_ComboBoxEditField: // the left side
                rect.setRect(fw, fw, w-xc-fw-1, h-(fw*2));
                break;

            case SC_ComboBoxListBoxPopup: // the list popup box
                rect = option.rect();
                break;

            default:
                break;
          }
          break;
      }

      case CC_ScrollBar: {
          const TQScrollBar *sb = ::tqqt_cast<const TQScrollBar*>(widget);
          if (!sb) break;

          bool horizontal = (sb->orientation() == Qt::Horizontal);
          rect = KStyle::querySubControlMetrics(control, widget,
                                                subcontrol, option);

          // adjust the standard metrics so controls can "overlap"
          if (subcontrol == SC_ScrollBarGroove) {
              if (horizontal) rect.addCoords(-1, 0, 1, 0);
              else            rect.addCoords(0, -1, 0, 1);
          }
          break;
      }

      case CC_SpinWidget: {
          bool odd = widget->height() % 2;
          xc = (h * 3 / 4) + odd; // position between edit and arrows

          switch (subcontrol) {
            case SC_SpinWidgetButtonField:
                rect.setRect(w-xc, 1, xc-1, h-2);
                break;

            case SC_SpinWidgetEditField:
                rect.setRect(fw, fw, w-xc-fw, h-(fw*2));
                break;

            case SC_SpinWidgetFrame:
                rect = widget->rect();
                break;

            case SC_SpinWidgetUp:
                rect.setRect(w-xc, (h/2)-(odd ? 6 : 7), xc-1, 6);
                break;

            case SC_SpinWidgetDown:
                rect.setRect(w-xc, (h/2)+1, xc-1, odd ? 7 : 6);
                break;

            default:
                break;
          }
          break;
      }

      default:
          rect = KStyle::querySubControlMetrics(control, widget, subcontrol,
                                                option);
    }

    return rect;
}

//////////////////////////////////////////////////////////////////////////////
// tqsizeFromContents()
// ------------------
// Returns the size of widget based on the contentsize

TQSize PhaseStyle::tqsizeFromContents(ContentsType contents,
                                   const TQWidget* widget,
                                   const TQSize &contentsize,
                                   const TQStyleOption &option ) const
{
    int w = contentsize.width();
    int h = contentsize.height();

    switch (contents) {
      case CT_PushButton: {
          const TQPushButton* button = ::tqqt_cast<const TQPushButton*>(widget);
          if (!button) {
              return KStyle::tqsizeFromContents(contents, widget, contentsize,
                                              option);
          }
          int margin = tqpixelMetric(PM_ButtonMargin, widget)
              + tqpixelMetric(PM_DefaultFrameWidth, widget) + 4;

          w += margin + 6; // add room for bold font
          h += margin;

          // standard width and heights
          if (button->isDefault() || button->autoDefault()) {
              if (w < 80 && !button->pixmap()) w = 80;
          }
          if (h < 22) h = 22;
          return TQSize(w, h);
      }

      case CT_PopupMenuItem: {
          if (!widget || option.isDefault()) return contentsize;
          const TQPopupMenu *popup = ::tqqt_cast<const TQPopupMenu*>(widget);
          if (!popup) {
              return KStyle::tqsizeFromContents(contents, widget, contentsize,
                                              option);
          }
          TQMenuItem *item = option.menuItem();

          if (item->custom()) {
              w = item->custom()->tqsizeHint().width();
              h = item->custom()->tqsizeHint().height();
              if (!item->custom()->fullSpan())
                  h += ITEMVMARGIN*2 + ITEMFRAME*2;
          } else if (item->widget()) { // a menu item that is a widget
              w = contentsize.width();
              h = contentsize.height();
          } else if (item->isSeparator()) {
              w = h = 3;
          } else {
              if (item->pixmap()) {
                  h = TQMAX(h, item->pixmap()->height() + ITEMFRAME*2);
              } else {
                  h = TQMAX(h, MINICONSIZE + ITEMFRAME*2);
                  h = TQMAX(h, popup->fontMetrics().height()
                           + ITEMVMARGIN*2 + ITEMFRAME*2);
              }
              if (item->iconSet())
                  h = TQMAX(h, item->iconSet()->
                           pixmap(TQIconSet::Small, TQIconSet::Normal).height()
                           + ITEMFRAME*2);
          }

          if (!item->text().isNull() && item->text().find('\t') >= 0)
              w += 12;
          else if (item->popup())
              w += 2 * ARROWMARGIN;

          if (option.maxIconWidth() || popup->isCheckable()) {
              w += TQMAX(option.maxIconWidth(),
                        TQIconSet::iconSize(TQIconSet::Small).width())
                  + ITEMHMARGIN*2;
          }
          w += RIGHTBORDER;
          return TQSize(w, h);
      }

      default:
          return KStyle::tqsizeFromContents(contents, widget, contentsize,
                                          option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// flatToolbar()
// -------------
// Is the toolbar "flat"

bool PhaseStyle::flatToolbar(const TQToolBar *toolbar) const
{
    if (!toolbar) return true; // not on a toolbar
    if (!toolbar->isMovingEnabled()) return true; // immobile toolbars are flat
    if (!toolbar->area()) return true; // not docked
    if (toolbar->place() == TQDockWindow::OutsideDock) return true; // ditto
    if (!toolbar->mainWindow()) return true; // not in a main window
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Grab events we are interested in. Most of this routine is to handle the
// exceptions to the normal styling rules.

bool PhaseStyle::eventFilter(TQObject *object, TQEvent *event)
{
    if (KStyle::eventFilter(object, event)) return true;
    if (!object->isWidgetType()) return false;

    bool horiz;
    int x, y, w, h;
    TQFrame *frame;
    TQToolBar *toolbar;
    TQWidget *widget;

    // painting events
    if (event->type() == TQEvent::Paint) {
        // make sure we do the most specific stuff first

        // KDE Toolbar Widget
        // patch by Daniel Brownlees <dbrownlees@paradise.net.nz>
        if (object->parent() && !qstrcmp(object->name(), KTOOLBARWIDGET)) {
            if (0 == (widget = ::tqqt_cast<TQWidget*>(object))) return false;
            TQWidget *parent = ::tqqt_cast<TQWidget*>(object->parent());
            int px = widget->x(), py = widget->y();
            // find the toolbar
            while (parent && parent->parent()
                   && !::tqqt_cast<TQToolBar*>(parent)) {
                px += parent->x();
                py += parent->y();
                parent = ::tqqt_cast<TQWidget*>(parent->parent());
            }
            if (!parent) return false;
            TQT_TQRECT_OBJECT(widget->rect()).rect(&x, &y, &w, &h);
            TQRect prect = parent->rect();

            toolbar = ::tqqt_cast<TQToolBar*>(parent);
            horiz = (toolbar) ? (toolbar->orientation() == Qt::Horizontal)
                : (prect.height() < prect.width());
            TQPainter painter(widget);
            if (flatToolbar(toolbar)) {
                painter.fillRect(widget->rect(),
                                  parent->tqcolorGroup().background());
            } else {
                drawPhaseGradient(&painter, widget->rect(),
                                  parent->tqcolorGroup().background(),
                                  !horiz, px, py,
                                  prect.width(), prect.height(), true);
                if (horiz && (h==prect.height()-2)) {
                    painter.setPen(parent->tqcolorGroup().mid());
                    painter.drawLine(x, h-1, w-1, h-1);
                } else if (!horiz && (w==prect.width()-2)) {
                    painter.setPen(parent->tqcolorGroup().mid());
                    painter.drawLine(w-1, y, w-1, h-1);
                }
            }
        }

        // TQToolBarExtensionWidget
        else if (object && object->isWidgetType() && object->parent() &&
                 (toolbar = ::tqqt_cast<TQToolBar*>(object->parent()))) {
            if (0 == (widget = ::tqqt_cast<TQWidget*>(object))) return false;
            horiz = (toolbar->orientation() == Qt::Horizontal);
            TQPainter painter(widget);
            TQT_TQRECT_OBJECT(widget->rect()).rect(&x, &y, &w, &h);
            // draw the extension
            drawPhaseGradient(&painter, widget->rect(),
                              toolbar->tqcolorGroup().background(),
                              !horiz, x, y, w-1, h-1, true);
            if (horiz) {
                painter.setPen(toolbar->tqcolorGroup().dark());
                painter.drawLine(w-1, 0, w-1, h-1);
                painter.setPen(toolbar->tqcolorGroup().mid());
                painter.drawLine(w-2, 0, w-2, h-2);
                painter.drawLine(x, h-1, w-2, h-1);
                painter.drawLine(x, y, x, h-2);
                painter.setPen(toolbar->tqcolorGroup().midlight());
                painter.drawLine(x+1, y, x+1, h-2);
            } else {
                painter.setPen(toolbar->tqcolorGroup().dark());
                painter.drawLine(0, h-1, w-1, h-1);
                painter.setPen(toolbar->tqcolorGroup().mid());
                painter.drawLine(0, h-2, w-2, h-2);
                painter.drawLine(w-1, y, w-1, h-2);
                painter.drawLine(x, y, w-2, y);
                painter.setPen(toolbar->tqcolorGroup().midlight());
                painter.drawLine(x, y+1, w-2, y+1);
            }
        }

        // TQFrame lines (do this guy last)
        else if (0 != (frame = ::tqqt_cast<TQFrame*>(object))) {
            TQFrame::Shape tqshape = frame->frameShape();
            switch (tqshape) {
              case TQFrame::HLine:
              case TQFrame::VLine: {
                  // NOTE: assuming lines have no content
                  TQPainter painter(frame);
                  TQT_TQRECT_OBJECT(frame->rect()).rect(&x, &y, &w, &h);
                  painter.setPen(frame->tqcolorGroup().dark());
                  if (tqshape == TQFrame::HLine) {
                      painter.drawLine(0, h/2, w, h/2);
                  } else if (tqshape == TQFrame::VLine) {
                      painter.drawLine(w/2, 0, w/2, h);
                  }
                  return true;
              }
              default:
                  break;
            }
        }

    } else if (highlights_) { // "mouseover" events
        if (::tqqt_cast<TQPushButton*>(object) ||
            ::tqqt_cast<TQComboBox*>(object) ||
            ::tqqt_cast<TQSpinWidget*>(object) ||
            ::tqqt_cast<TQCheckBox*>(object) ||
            ::tqqt_cast<TQRadioButton*>(object) ||
            ::tqqt_cast<TQSlider*>(object) ||
            object->inherits(TQSPLITTERHANDLE)) {
            if (event->type() == TQEvent::Enter) {
                if (0 != (widget = ::tqqt_cast<TQWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = widget;
                    widget->tqrepaint(false);
                }
            } else if (event->type() == TQEvent::Leave) {
                if (0 != (widget = ::tqqt_cast<TQWidget*>(object))) {
                    hover_ = 0;
                    widget->tqrepaint(false);
                }
            }
        } else if (::tqqt_cast<TQTabBar*>(object)) { // special case for qtabbar
            if (event->type() == TQEvent::Enter) {
                if (0 != (widget = ::tqqt_cast<TQWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = widget;
                    hovertab_ = 0;;
                    widget->tqrepaint(false);
                }
            } else if (event->type() == TQEvent::Leave) {
                if (0 != (widget = ::tqqt_cast<TQWidget*>(object))) {
                    hover_ = 0;
                    hovertab_ = 0;;
                    widget->tqrepaint(false);
                }
            } else if (event->type() == TQEvent::MouseMove) {
                TQTabBar *tabbar;
                if (0 != (tabbar = ::tqqt_cast<TQTabBar*>(object))) {
                    TQMouseEvent *me;
                    if (0 != (me = dynamic_cast<TQMouseEvent*>(event))) {
                        TQTab *tab = tabbar->selectTab(me->pos());
                        if (hovertab_ != tab) {
                            hovertab_ = tab;
                            tabbar->tqrepaint(false);
                        }
                    }
                }
            }
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// GradientSet                                                              //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// GradientSet()
// -------------
// Constructor

GradientSet::GradientSet(const TQColor &color, int size)
    : color_(color), size_(size)
{
    for (int n=0; n<GradientTypeCount; ++n)  set[n] = 0;
}

//////////////////////////////////////////////////////////////////////////////
// ~GradientSet()
// --------------
// Destructor

GradientSet::~GradientSet()
{
    for (int n=0; n<GradientTypeCount; ++n) if (set[n]) delete set[n];
}

//////////////////////////////////////////////////////////////////////////////
// gradient()
// ----------
// Return the appropriate gradient pixmap

KPixmap* GradientSet::gradient(bool horizontal, bool reverse)
{
    GradientType type;

    if (horizontal) {
        type = (reverse) ? HorizontalReverse : Horizontal;
    } else {
        type = (reverse) ? VerticalReverse : Vertical;
    }

    // lazy allocate
    if (!set[type]) {
        set[type] = new KPixmap();
        switch (type) {
          case Horizontal:
              set[type]->resize(size_, 16);
              KPixmapEffect::gradient(*set[type],
                                      color_.light(contrast),
                                      color_.dark(contrast),
                                      KPixmapEffect::HorizontalGradient);
              break;

          case HorizontalReverse:
              set[type]->resize(size_, 16);
              KPixmapEffect::gradient(*set[type],
                                      color_.dark(contrast),
                                      color_.light(contrast),
                                      KPixmapEffect::HorizontalGradient);
              break;

          case Vertical:
              set[type]->resize(16, size_);
              KPixmapEffect::gradient(*set[type],
                                      color_.light(contrast),
                                      color_.dark(contrast),
                                      KPixmapEffect::VerticalGradient);
              break;

          case VerticalReverse:
              set[type]->resize(16, size_);
              KPixmapEffect::gradient(*set[type],
                                      color_.dark(contrast),
                                      color_.light(contrast),
                                      KPixmapEffect::VerticalGradient);
              break;

          default:
              break;
        }
    }
    return (set[type]);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

class PhaseStylePlugin : public TQStylePlugin
{
 public:
    PhaseStylePlugin();
    TQStringList keys() const;
    TQStyle *create(const TQString &key);
};

PhaseStylePlugin::PhaseStylePlugin() : TQStylePlugin() { ; }

TQStringList PhaseStylePlugin::keys() const
{
    return TQStringList() << "Phase";
}

TQStyle* PhaseStylePlugin::create(const TQString& key)
{
    if (key.lower() == "phase")
        return new PhaseStyle();
    return 0;
}

KDE_Q_EXPORT_PLUGIN(PhaseStylePlugin)

#include "phasestyle.moc"
