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

#ifndef __KXSCONTROL_H__
#define __KXSCONTROL_H__

#include <tqwidget.h>
#include <tqcheckbox.h>

#include "kxsitem.h"

class QLabel;
class QSlider;
class QSpinBox;
class QComboBox;
class QLineEdit;

//===========================================================================
class KXSRangeControl : public TQWidget, public KXSRangeItem
{
  Q_OBJECT
public:
  KXSRangeControl(TQWidget *parent, const TQString &name, KConfig &config);
  KXSRangeControl(TQWidget *parent, const TQString &name, const TQXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void slotValueChanged(int value);

protected:
  TQSlider *mSlider;
  TQSpinBox *mSpinBox;
};

//===========================================================================
class KXSDoubleRangeControl : public TQWidget, public KXSDoubleRangeItem
{
  Q_OBJECT
public:
  KXSDoubleRangeControl(TQWidget *parent, const TQString &name, KConfig &config);
  KXSDoubleRangeControl(TQWidget *parent, const TQString &name, const TQXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void slotValueChanged(int value);

protected:
  TQSlider *mSlider;
  double  mStep;
};

//===========================================================================
class KXSCheckBoxControl : public TQCheckBox, public KXSBoolItem
{
  Q_OBJECT
public:
  KXSCheckBoxControl(TQWidget *parent, const TQString &name, KConfig &config);
  KXSCheckBoxControl(TQWidget *parent, const TQString &name, const TQXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void slotToggled(bool);
};

//===========================================================================
class KXSDropListControl : public TQWidget, public KXSSelectItem
{
  Q_OBJECT
public:
  KXSDropListControl(TQWidget *parent, const TQString &name, KConfig &config);
  KXSDropListControl(TQWidget *parent, const TQString &name, const TQXmlAttributes &attr );

  virtual void read(KConfig &config);

  virtual void addOption( const TQXmlAttributes &attr );

signals:
  void changed();

protected slots:
  void slotActivated(int);

protected:
  TQComboBox *mCombo;
};

//===========================================================================
class KXSLineEditControl : public TQWidget, public KXSStringItem
{
  Q_OBJECT
public:
  KXSLineEditControl(TQWidget *parent, const TQString &name, KConfig &config);
  KXSLineEditControl(TQWidget *parent, const TQString &name, const TQXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void textChanged(const TQString &);

protected:
  TQLineEdit *mEdit;
};

//===========================================================================
class KXSFileControl : public TQWidget, public KXSStringItem
{
  Q_OBJECT
public:
  KXSFileControl(TQWidget *parent, const TQString &name, KConfig &config);
  KXSFileControl(TQWidget *parent, const TQString &name, const TQXmlAttributes &attr );

  virtual void read(KConfig &config);

signals:
  void changed();

protected slots:
  void textChanged(const TQString &);
  void selectFile();

protected:
  TQLineEdit *mEdit;

};

#endif

