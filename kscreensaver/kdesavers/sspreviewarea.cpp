//============================================================================
//
// KRotation screen saver for KDE
// Copyright (C) 2004 Georg Drenkhahn
// $Id$
//
//============================================================================

#include "sspreviewarea.h"
#include "sspreviewarea.moc"

SsPreviewArea::SsPreviewArea(QWidget* parent, const char* name)
   : QWidget(parent, name)
{
}

void SsPreviewArea::resizeEvent(QResizeEvent* e)
{
   emit resized(e);
}

