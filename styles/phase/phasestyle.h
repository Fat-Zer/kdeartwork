//////////////////////////////////////////////////////////////////////////////
// phasestyle.h
// -------------------
// Qt/KDE widget style
// -------------------
// Copyright (c) 2004 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef PHASESTYLE_H
#define PHASESTYLE_H

#include <kstyle.h>
#include <tqcolor.h>

class KPixmap;

class GradientSet
{
public:
    enum GradientType {
        Horizontal,
        Vertical,
        HorizontalReverse,
        VerticalReverse,
        GradientTypeCount
    };

    GradientSet(const TQColor &color, int size);
    ~GradientSet();

    KPixmap* gradient(bool horizontal, bool reverse);

private:
    KPixmap *set[GradientTypeCount];
    TQColor color_;
    int size_;
};

class PhaseStyle : public KStyle
{
    Q_OBJECT
public:
    PhaseStyle();
    virtual ~PhaseStyle();

    void polish(TQApplication* app);
    void polish(TQWidget *widget);
    void polish(TQPalette &pal);
    void unPolish(TQWidget *widget);

    void drawPrimitive(PrimitiveElement element,
            TQPainter *painter,
            const TQRect &rect,
            const TQColorGroup &group,
            SFlags flags = Style_Default,
            const TQStyleOption &option = TQStyleOption::Default) const;

    void drawKStylePrimitive(KStylePrimitive element,
            TQPainter *painter,
            const TQWidget *widget,
            const TQRect &rect,
            const TQColorGroup &group,
            SFlags flags = Style_Default,
            const TQStyleOption &option = TQStyleOption::Default) const;

    void drawControl(ControlElement element,
            TQPainter *painter,
            const TQWidget *widget,
            const TQRect &rect,
            const TQColorGroup &group,
            SFlags flags = Style_Default,
            const TQStyleOption &option = TQStyleOption::Default) const;

    void drawControlMask(ControlElement element,
            TQPainter *painter,
            const TQWidget *widget,
            const TQRect &rect,
            const TQStyleOption &option = TQStyleOption::Default) const;

    void drawComplexControl(ComplexControl control,
            TQPainter *painter,
            const TQWidget *widget,
            const TQRect &rect,
            const TQColorGroup &group,
            SFlags flags = Style_Default,
            SCFlags controls = SC_All,
            SCFlags active = SC_None,
            const TQStyleOption &option = TQStyleOption::Default) const;

    void drawComplexControlMask(ComplexControl control,
            TQPainter *painter,
            const TQWidget *widget,
            const TQRect &rect,
            const TQStyleOption &option = TQStyleOption::Default) const;

    int pixelMetric(PixelMetric metric,
            const TQWidget *widget = 0) const;

    TQRect subRect(SubRect rect, const TQWidget *widget) const;

    TQRect querySubControlMetrics(ComplexControl control,
            const TQWidget *widget,
	    SubControl subcontrol,
            const TQStyleOption &option = TQStyleOption::Default) const;

    TQSize sizeFromContents(ContentsType contents,
            const TQWidget *widget,
            const TQSize &contentsize,
            const TQStyleOption& option = TQStyleOption::Default) const;

private:
    PhaseStyle(const PhaseStyle &);
    PhaseStyle& operator=(const PhaseStyle &);

    void drawPhaseBevel(TQPainter *painter,
            int x, int y, int w, int h,
            const TQColorGroup &group,
	    const TQColor &fill,
            bool sunken=false,
            bool horizontal=true,
            bool reverse=false) const;

    void drawPhaseButton(TQPainter *painter,
            int x, int y, int w, int h,
            const TQColorGroup &group,
	    const TQColor &fill,
            bool sunken=false) const;

    void drawPhasePanel(TQPainter *painter,
            int x, int y, int w, int h,
            const TQColorGroup &group,
            bool sunken=false,
            const TQBrush *fill=NULL) const;

    void drawPhaseTab(TQPainter *painter,
            int x, int y, int w, int h,
            const TQColorGroup &group,
            const TQTabBar *bar,
	    const TQStyleOption &option,
            const SFlags flags) const;

    void drawPhaseGradient(TQPainter *painter,
            const TQRect &rect,
            TQColor color,
            bool horizontal,
            int px=0, int py=0,
            int pw=-1, int ph=-1,
            bool reverse=false) const;

    bool flatToolbar(const TQToolBar *toolbar) const;

    bool eventFilter(TQObject *object, TQEvent *event);

private:
    TQWidget *hover_;
    TQTab *hovertab_;
    TQMap<unsigned int, TQIntDict<GradientSet> > * gradients;
    bool gradients_;
    bool highlights_;
    bool reverse_;
    bool kicker_;
};

#endif // PHASESTYLE_H
