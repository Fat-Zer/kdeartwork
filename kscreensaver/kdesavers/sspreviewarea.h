//============================================================================
//
// KRotation screen saver for KDE
// Copyright (C) 2004 Georg Drenkhahn
// $Id$
//
//============================================================================

#ifndef SSPREVIEWAREA_H
#define SSPREVIEWAREA_H

#include <tqwidget.h>

/** @brief Reimplementation of TQWidget emitting a signal if resized.
 *
 * This class is equalt to TQWidget except for the fact that the signal resized()
 * is emitted if the widget gets resized.  By this signaling mechanism it is
 * possible to resize the embedded GL area object within the screen saver setup
 * dialog.
 *
 * In the constructor of the dialog widget (KPendulumSetup::KPendulumSetup(),
 * KRotationSetup::KRotationSetup()) the signal SsPreviewArea::resized() is
 * connected with a slot of the screensaver class
 * (KPendulumSaver::resizeGlArea(), KRotationSaver::resizeGlArea()).  This slot
 * function calls the reimplemented TQGLWidget::resizeGL() method of the GL
 * widgets (PendulumGLWidget::resizeGL(), RotationGLWidget::resizeGL()) which
 * really resizes the GL scenery. */
class SsPreviewArea : public TQWidget
{
   Q_OBJECT
  TQ_OBJECT

  public:
   /** @brief Constructor for SsPreviewArea
    * @param parent Pointer tp parent widget, forwarded to the TQWidget
    * constructor
    * @param name Pointer to widget name, forwarded to the TQWidget constructor
    *
    * The constructor just calls TQWidget::TQWidget() with the given arguments.
    */
   SsPreviewArea(TQWidget* parent = NULL, const char* name = NULL);

  protected:
   /** @brief Called if widget gets resized.
    * @param e Pointer to the corresponding TQResizeEvent object containing the
    * resize information
    *
    * Reimplemented event handler from TQWidget.  Only the signal resized() is
    * emitted. */
   virtual void resizeEvent(TQResizeEvent* e);

  signals:
   /** @brief Signal which is emitted in the resizeEvent() method.
    * @param e Pointer to the corresponding TQResizeEvent object */
   void resized(TQResizeEvent* e);
};

#endif
