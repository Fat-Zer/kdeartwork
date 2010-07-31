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

#include <tqlabel.h>
#include <tqslider.h>
#include <tqlayout.h>
#include <tqcombobox.h>
#include <tqlineedit.h>
#include <tqspinbox.h>
#include <tqpushbutton.h>
#include <tqxml.h>
#include <klocale.h>
#include <kfiledialog.h>
#include "kxscontrol.h"

//===========================================================================
KXSRangeControl::KXSRangeControl(TQWidget *parent, const TQString &name,
                                  KConfig &config)
  : TQWidget(parent), KXSRangeItem(name, config), mSlider(0), mSpinBox(0)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(mLabel, this);
  l->add(label);
  mSlider = new TQSlider(mMinimum, mMaximum, 10, mValue, Qt::Horizontal, this);
  connect(mSlider, TQT_SIGNAL(valueChanged(int)), TQT_SLOT(slotValueChanged(int)));
  l->add(mSlider);
}

KXSRangeControl::KXSRangeControl(TQWidget *parent, const TQString &name,
                                  const TQXmlAttributes &attr )
  : TQWidget(parent), KXSRangeItem(name, attr), mSlider(0), mSpinBox(0)
{
    if (attr.value("type") == "spinbutton" ) {
	TQHBoxLayout *hb = new TQHBoxLayout(this);
	if (!mLabel.isEmpty()) {
            TQLabel *l = new TQLabel(i18n(mLabel.utf8()), this);
	    hb->add(l);
	}
	mSpinBox = new TQSpinBox(mMinimum, mMaximum, 1, this);
	mSpinBox->setValue(mValue);
	connect(mSpinBox, TQT_SIGNAL(valueChanged(int)), TQT_SLOT(slotValueChanged(int)));
	hb->add(mSpinBox);
    } else {
	TQString lowLabel = attr.value("_low-label");
	TQString highLabel = attr.value("_high-label");
	TQVBoxLayout *vb = new TQVBoxLayout(this);
	if (!mLabel.isEmpty()) {
            TQLabel *l = new TQLabel(i18n(mLabel.utf8()), this);
	    vb->add(l);
	}
	TQHBoxLayout *hb = new TQHBoxLayout(vb);
	if (!lowLabel.isEmpty()) {
            TQLabel *l = new TQLabel(i18n(lowLabel.utf8()), this);
	    hb->addWidget(l);
	}
	mSlider = new TQSlider(mMinimum, mMaximum, 10, mValue, Qt::Horizontal, this);
	connect(mSlider, TQT_SIGNAL(valueChanged(int)), TQT_SLOT(slotValueChanged(int)));
	hb->add(mSlider);
	if (!highLabel.isEmpty()){
            TQLabel *l = new TQLabel(i18n(highLabel.utf8()), this);
	    hb->addWidget(l);
	}
    }
}

void KXSRangeControl::slotValueChanged(int value)
{
  mValue = value;
  emit changed();
}

void KXSRangeControl::read(KConfig &config)
{
    KXSRangeItem::read(config);
    if ( mSpinBox )
	mSpinBox->setValue(mValue);
    else
	mSlider->setValue(mValue);
}

//===========================================================================
KXSDoubleRangeControl::KXSDoubleRangeControl(TQWidget *parent,
                                  const TQString &name, KConfig &config)
  : TQWidget(parent), KXSDoubleRangeItem(name, config)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(mLabel, this);
  l->add(label);

  int value = int((mValue - mMinimum) * 100 / (mMaximum - mMinimum));

  mSlider = new TQSlider(0, 100, 10, value, Qt::Horizontal, this);
  connect(mSlider, TQT_SIGNAL(valueChanged(int)), TQT_SLOT(slotValueChanged(int)));
  l->add(mSlider);
}

KXSDoubleRangeControl::KXSDoubleRangeControl(TQWidget *parent,
                                  const TQString &name, const TQXmlAttributes &attr)
  : TQWidget(parent), KXSDoubleRangeItem(name, attr)
{
    TQString lowLabel = attr.value("_low-label");
    TQString highLabel = attr.value("_high-label");
    TQVBoxLayout *vb = new TQVBoxLayout(this);
    if (!mLabel.isEmpty()) {
        TQLabel *l = new TQLabel(i18n(mLabel.utf8()), this);
	vb->add(l);
    }
    TQHBoxLayout *hb = new TQHBoxLayout(vb);
    if (!lowLabel.isEmpty()) {
        TQLabel *l = new TQLabel(i18n(lowLabel.utf8()), this);
	hb->addWidget(l);
    }
    int value = int((mValue - mMinimum) * 100 / (mMaximum - mMinimum));
    mSlider = new TQSlider(0, 100, 10, value, Qt::Horizontal, this);
    connect(mSlider, TQT_SIGNAL(valueChanged(int)), TQT_SLOT(slotValueChanged(int)));
    hb->add(mSlider);
    if (!highLabel.isEmpty()){
        TQLabel *l = new TQLabel(i18n(highLabel.utf8()), this);
	hb->addWidget(l);
    }
}

void KXSDoubleRangeControl::slotValueChanged(int value)
{
  mValue = mMinimum + value * (mMaximum - mMinimum) / 100.0;
  emit changed();
}

void KXSDoubleRangeControl::read(KConfig &config)
{
    KXSDoubleRangeItem::read(config);
    mSlider->setValue((int)((mValue - mMinimum) * 100.0 /
                            (mMaximum - mMinimum) + 0.5));
}

//===========================================================================
KXSCheckBoxControl::KXSCheckBoxControl(TQWidget *parent, const TQString &name,
                                      KConfig &config)
  : TQCheckBox(parent), KXSBoolItem(name, config)
{
  setText(mLabel);
  setChecked(mValue);
  connect(this, TQT_SIGNAL(toggled(bool)), TQT_SLOT(slotToggled(bool)));
}

KXSCheckBoxControl::KXSCheckBoxControl(TQWidget *parent, const TQString &name,
                                      const TQXmlAttributes &attr)
  : TQCheckBox(parent), KXSBoolItem(name, attr)
{
  setText(i18n(mLabel.utf8()));
  setChecked(mValue);
  connect(this, TQT_SIGNAL(toggled(bool)), TQT_SLOT(slotToggled(bool)));
}

void KXSCheckBoxControl::slotToggled(bool state)
{
  mValue = state;
  emit changed();
}

void KXSCheckBoxControl::read(KConfig &config)
{
    KXSBoolItem::read(config);
    setChecked(mValue);
}

//===========================================================================
KXSDropListControl::KXSDropListControl(TQWidget *parent, const TQString &name,
                                      KConfig &config)
  : TQWidget(parent), KXSSelectItem(name, config)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(mLabel, this);
  l->add(label);
  mCombo = new TQComboBox(this);
  for(uint i=0; i < mOptions.count(); i++)
      mCombo->insertItem( i18n(mOptions[i].utf8()) );
  mCombo->setCurrentItem(mValue);
  connect(mCombo, TQT_SIGNAL(activated(int)), TQT_SLOT(slotActivated(int)));
  l->add(mCombo);
}

KXSDropListControl::KXSDropListControl(TQWidget *parent, const TQString &name,
                                      const TQXmlAttributes &attr)
  : TQWidget(parent), KXSSelectItem(name, attr)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(i18n(mLabel.utf8()), this);
  l->add(label);
  mCombo = new TQComboBox(this);
  connect(mCombo, TQT_SIGNAL(activated(int)), TQT_SLOT(slotActivated(int)));
  l->add(mCombo);
}

void KXSDropListControl::addOption(const TQXmlAttributes &attr)
{
    KXSSelectItem::addOption( attr );
    mCombo->insertItem( i18n(mOptions[mOptions.count()-1].utf8()) );
    if ( (unsigned)mValue == mOptions.count()-1 )
	mCombo->setCurrentItem(mOptions.count()-1);
}

void KXSDropListControl::slotActivated(int indx)
{
  mValue = indx;
  emit changed();
}

void KXSDropListControl::read(KConfig &config)
{
    KXSSelectItem::read(config);
    mCombo->setCurrentItem(mValue);
}

//===========================================================================
KXSLineEditControl::KXSLineEditControl(TQWidget *parent, const TQString &name,
                                  KConfig &config)
  : TQWidget(parent), KXSStringItem(name, config)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(mLabel, this);
  l->add(label);
  mEdit = new TQLineEdit(this);
  connect(mEdit, TQT_SIGNAL(textChanged(const TQString &)), TQT_SLOT(textChanged(const TQString &)));
  l->add(mEdit);
}

KXSLineEditControl::KXSLineEditControl(TQWidget *parent, const TQString &name,
                                  const TQXmlAttributes &attr )
  : TQWidget(parent), KXSStringItem(name, attr)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(i18n(mLabel.utf8()), this);
  l->add(label);
  mEdit = new TQLineEdit(this);
  connect(mEdit, TQT_SIGNAL(textChanged(const TQString &)), TQT_SLOT(textChanged(const TQString &)));
  l->add(mEdit);
}

void KXSLineEditControl::textChanged( const TQString &text )
{
  mValue = text;
  emit changed();
}

void KXSLineEditControl::read(KConfig &config)
{
    KXSStringItem::read(config);
    mEdit->setText(mValue);
}

//===========================================================================
KXSFileControl::KXSFileControl(TQWidget *parent, const TQString &name,
                                  KConfig &config)
  : TQWidget(parent), KXSStringItem(name, config)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(mLabel, this);
  l->add(label);
  mEdit = new TQLineEdit(this);
  connect(mEdit, TQT_SIGNAL(textChanged(const TQString &)), TQT_SLOT(textChanged(const TQString &)));
  l->add(mEdit);
}

KXSFileControl::KXSFileControl(TQWidget *parent, const TQString &name,
                                  const TQXmlAttributes &attr )
  : TQWidget(parent), KXSStringItem(name, attr)
{
  TQVBoxLayout *l = new TQVBoxLayout(this);
  TQLabel *label = new TQLabel(i18n(mLabel.utf8()), this);
  l->add(label);
  TQHBoxLayout *hb = new TQHBoxLayout(l);
  mEdit = new TQLineEdit(this);
  connect(mEdit, TQT_SIGNAL(textChanged(const TQString &)), TQT_SLOT(textChanged(const TQString &)));
  hb->add(mEdit);
  TQPushButton *pb = new TQPushButton( "...", this );
  connect( pb, TQT_SIGNAL(clicked()), this, TQT_SLOT(selectFile()) );
  hb->addWidget(pb);
}

void KXSFileControl::textChanged( const TQString &text )
{
  mValue = text;
  emit changed();
}

void KXSFileControl::selectFile()
{
    TQString f = KFileDialog::getOpenFileName();
    if ( !f.isEmpty() ) {
	mValue = f;
	mEdit->setText(mValue);
	emit changed();
    }
}

void KXSFileControl::read(KConfig &config)
{
    KXSStringItem::read(config);
    mEdit->setText(mValue);
}

#include "kxscontrol.moc"
