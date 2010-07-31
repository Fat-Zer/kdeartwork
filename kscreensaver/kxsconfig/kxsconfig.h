//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef __KXSCONFIG_H__
#define __KXSCONFIG_H__

#include <kdialogbase.h>
#include "kxsitem.h"

class KProcess;
class QLabel;

class KXSConfigDialog : public KDialogBase
{
  Q_OBJECT
public:
  KXSConfigDialog(const TQString &file, const TQString &name);
  ~KXSConfigDialog();

  bool create();
  TQString command();

protected slots:
  void slotPreviewExited(KProcess *);
  void slotNewPreview();
  void slotChanged();
  virtual void slotOk();
  virtual void slotCancel();

protected:
  TQString   mFilename;
  TQString   mExeName;
  TQString   mConfigFile;
  KProcess  *mPreviewProc;
  TQWidget   *mPreview;
  TQTimer    *mPreviewTimer;
  TQPtrList<KXSConfigItem> mConfigItemList;
  bool      mKilled;
};

#endif
