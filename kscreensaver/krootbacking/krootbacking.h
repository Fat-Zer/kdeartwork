/***************************************************************************
 *   Copyright (C) 2011 by Timothy Pearson <kb9vqf@pearsoncomputing.net>   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/


#ifndef KROOTBACKING_H
#define KROOTBACKING_H

#include <tqobject.h>
#include <tqcolor.h>
#include <kdelibs_export.h>

#ifndef Q_WS_QWS //FIXME

class TQRect;
class TQWidget;
class TQTimer;
class KSharedPixmap;
class KRootBackingData;

/**
 * Gets the current full shared desktop pixmap and feeds it to xscreensaver
 *
 * @author Timothy Pearson <kb9vqf@pearsoncomputing.net>
 */
class KRootBacking: public TQObject
{
    Q_OBJECT

public:
    /**
     * Constructs a KRootBacking.
     */
    KRootBacking();

    /**
     * Destructs the object.
     */
    virtual ~KRootBacking();

    /**
     * Checks if pseudo-transparency is available.
     * @return @p true if transparency is available, @p false otherwise.
     */
    bool isAvailable() const;

    /**
     * Returns true if the KRootBacking is active.
     */
    bool isActive() const { return m_bActive; }

    /**
     * Returns the number of the current desktop.
     */
    int currentDesktop() const;

#ifndef KDE_NO_COMPAT
    /**
     * Deprecated, use isAvailable() instead.
     * @deprecated
     */
    KDE_DEPRECATED bool checkAvailable(bool) { return isAvailable(); }
#endif

    /** @since 3.2
     * @return the fade color.
     */
    const TQColor &color() const { return m_FadeColor; }

    /** @since 3.2
     * @return the color opacity.
     */
    double opacity() const { return m_Fade; }

public slots:
    /**
     * Starts background handling.
     */
    virtual void start();

    /**
     * Stops background handling.
     */
    virtual void stop();

    /**
     * Sets the fade effect.
     *
     * This effect will fade the background to the
     * specified color.
     * @param opacity A value between 0 and 1, indicating the opacity
     * of the color. A value of 0 will not change the image, a value of 1
     * will use the fade color unchanged.
     * @param color The color to fade to.
     */
    void setFadeEffect(double opacity, const TQColor &color);

    /**
     * Repaints the widget background. Normally, you shouldn't need this
     * as it is handled automatically.
     *
     * @param force Force a tqrepaint, even if the contents did not change.
     */
    void tqrepaint( bool force );

    /**
     * Repaints the widget background. Normally, you shouldn't need this
     * as it is handled automatically. This is equivalent to calling
     * tqrepaint( false ).
     */
    void tqrepaint();

    /**
     * Asks KDesktop to export the desktop background as a KSharedPixmap.
     * This method uses DCOP to call KBackgroundIface/setExport(int).
     */
    void enableExports();

    /**
     * Returns the name of the shared pixmap (only needed for low level access)
     */
    static TQString pixmapName(int desk);
signals:
    /**
     * Emitted when the background needs updating and custom painting
     * (see setCustomPainting(bool) ) is enabled.
     *
     * @param pm A pixmap containing the new background.
     */
    void backgroundUpdated( const TQPixmap &pm );

protected:
    /**
     * Called when the pixmap has been updated. The default implementation
     * applies the fade effect, then sets the target's background, or emits
     * backgroundUpdated(const TQPixmap &) depending on the painting mode.
     */
    virtual void updateBackground( KSharedPixmap * );

private slots:
    void slotBackgroundChanged(int);
    void slotDone(bool);

private:
    bool m_bActive, m_bInit;
    int m_Desk;
    int m_timeout;

    double m_Fade;
    TQColor m_FadeColor;

    TQRect m_Rect;
    TQTimer *m_pTimer;
    KSharedPixmap *m_pPixmap;
    KRootBackingData *d;

    void init();
};

#endif // ! Q_WS_QWS

#endif // KROOTBACKING_H