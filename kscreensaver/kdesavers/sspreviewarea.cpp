//============================================================================
//
// KRotation screen saver for KDE
// Copyright (C) 2004 Georg Drenkhahn
// $Id$
//
//============================================================================

#include "sspreviewarea.h"
#include "sspreviewarea.moc"

SsPreviewArea::SsPreviewArea(TQWidget* parent, const char* name)
   : TQWidget(parent, name)
{
}

void SsPreviewArea::resizeEvent(TQResizeEvent* e)
{
   emit resized(e);
}

