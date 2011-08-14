/***************************************************************************
                          glowconfigdialog.cpp  -  description
                             -------------------
    begin                : Thu Sep 12 2001
    copyright            : (C) 2001 by Henning Burchardt
    email                : h_burchardt@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <tqbitmap.h>
#include <tqbuttongroup.h>
#include <tqcheckbox.h>
#include <tqcombobox.h>
#include <tqdir.h>
#include <tqfileinfo.h>
#include <tqgroupbox.h>
#include <tqlabel.h>
#include <tqlayout.h>
#include <tqlistview.h>
#include <tqpushbutton.h>
#include <tqsignalmapper.h>
#include <tqstringlist.h>
#include <kconfig.h>
#include <kcolorbutton.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kstandarddirs.h>
#include "bitmaps.h"
#include "glowconfigdialog.h"
#include "../resources.h"

#define NUMBER_OF_BUTTONS 5

extern "C"
{
	KDE_EXPORT TQObject* allocate_config( KConfig* conf, TQWidget* parent )
	{
		return(new GlowConfigDialog(conf, parent));
	}
}

GlowConfigDialog::GlowConfigDialog( KConfig * conf, TQWidget * parent )
	: TQObject(parent)
{
	_glowConfig = new KConfig("kwinglowrc");
	KGlobal::locale()->insertCatalogue("kwin_glow_config");

	_main_group_box = new TQWidget(parent);
	TQVBoxLayout *main_group_boxLayout = new TQVBoxLayout(_main_group_box);
	main_group_boxLayout->tqsetAlignment(TQt::AlignTop | TQt::AlignLeft);
	main_group_boxLayout->setSpacing(6);

	//-------------------------------------------------------------------------
	// themes

	_theme_list_view = new TQListView (_main_group_box, "theme_list_view");
	_theme_list_view->addColumn (i18n("Theme"));
	_theme_list_view->addColumn (i18n("Button Size"));
        _theme_list_view->setAllColumnsShowFocus(true);
        _theme_list_view->setResizeMode(TQListView::AllColumns);

	main_group_boxLayout->addWidget (_theme_list_view);
	TQObject::connect (_theme_list_view, TQT_SIGNAL(selectionChanged()),
			this, TQT_SLOT(slotThemeListViewSelectionChanged()));
	slotLoadThemeList();

	_button_glow_color_group_box = new TQGroupBox(
		0, Qt::Horizontal, i18n("Button Glow Colors"), _main_group_box);
	TQHBoxLayout *colorHBoxLayout =
		new TQHBoxLayout(_button_glow_color_group_box->tqlayout());

	// create buttons
	TQSize buttonSize(BITMAP_SIZE, BITMAP_SIZE);
	TQPixmap pm(buttonSize);
	pm.fill(TQt::black);

	_stickyButton = new TQPushButton(_button_glow_color_group_box);
	pm.setMask(TQBitmap(buttonSize, stickyoff_bits, true));
	_stickyButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_stickyButton);
	_titleButtonList.push_back(_stickyButton);

	_helpButton = new TQPushButton(_button_glow_color_group_box);
	pm.setMask(TQBitmap(buttonSize, help_bits, true));
	_helpButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_helpButton);
	_titleButtonList.push_back(_helpButton);

	_iconifyButton = new TQPushButton(_button_glow_color_group_box);
	pm.setMask(TQBitmap(buttonSize, minimize_bits, true));
	_iconifyButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_iconifyButton);
	_titleButtonList.push_back(_iconifyButton);

	_maximizeButton = new TQPushButton(_button_glow_color_group_box);
	pm.setMask(TQBitmap(buttonSize, maximizeoff_bits, true));
	_maximizeButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_maximizeButton);
	_titleButtonList.push_back(_maximizeButton);

	_closeButton = new TQPushButton(_button_glow_color_group_box);
	pm.setMask(TQBitmap(buttonSize, close_bits, true));
	_closeButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_closeButton);
	_titleButtonList.push_back(_closeButton);

	// create signal mapper
	_titleButtonMapper = new TQSignalMapper(this);
	for( uint i=0; i<_titleButtonList.size(); i++ ) {
		_titleButtonMapper->setMapping(TQT_TQOBJECT(_titleButtonList[i]), i);
		connect(_titleButtonList[i], TQT_SIGNAL(clicked()),_titleButtonMapper, TQT_SLOT(map()));
	}
	connect(_titleButtonMapper, TQT_SIGNAL(mapped(int)),this, TQT_SLOT(slotTitleButtonClicked(int)));

	_colorButton = new KColorButton(_button_glow_color_group_box);
	_colorButton->setEnabled(false);
	connect(_colorButton, TQT_SIGNAL(changed(const TQColor&)),
		this, TQT_SLOT(slotColorButtonChanged(const TQColor&)));

	colorHBoxLayout->addItem(new TQSpacerItem(
		200, 20, TQSizePolicy::Expanding, TQSizePolicy::Minimum));
	colorHBoxLayout->addWidget(_colorButton);

	main_group_boxLayout->addWidget(_button_glow_color_group_box);

	TQHBoxLayout *titlebarGradientTypeLayout = new TQHBoxLayout();
	_titlebarGradientTypeComboBox = new TQComboBox(_main_group_box);

    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, TQString::tqfromLatin1("WM") );
    TQColor activeBackground = c->readColorEntry("activeBackground");
    TQColor activeBlend = c->readColorEntry("activeBlend");

	// If the colors are equal, change one to get a gradient effect
	if (activeBackground==activeBlend) {
		activeBackground = activeBackground.dark();
	}
	for (int i=0; i< KPixmapEffect::EllipticGradient; i++ ) {
		KPixmap gradPixmap(TQSize(196,20));
		KPixmapEffect::gradient(gradPixmap, activeBackground,
								activeBlend, (KPixmapEffect::GradientType) i);

		_titlebarGradientTypeComboBox->insertItem(gradPixmap, i);
	}

	connect(_titlebarGradientTypeComboBox, TQT_SIGNAL(activated(int)),
		this, TQT_SLOT(slotTitlebarGradientTypeChanged(int)));
	titlebarGradientTypeLayout->addWidget(
		new TQLabel(i18n("Titlebar gradient:"), _main_group_box));
	titlebarGradientTypeLayout->addWidget(_titlebarGradientTypeComboBox, 0, TQt::AlignLeft);
        titlebarGradientTypeLayout->addStretch(10);
	main_group_boxLayout->addLayout(titlebarGradientTypeLayout);


	_showResizeHandleCheckBox = new TQCheckBox(
			i18n("Show resize handle"),	_main_group_box);
	connect(_showResizeHandleCheckBox, TQT_SIGNAL(clicked()),
		this, TQT_SLOT(slotResizeHandleCheckBoxChanged()));
	main_group_boxLayout->addWidget(_showResizeHandleCheckBox);

	// load config and update user interface
	load(conf);

	_main_group_box->show();
}

GlowConfigDialog::~GlowConfigDialog()
{
	delete _main_group_box;
	delete _glowConfig;
	delete[] _buttonConfigMap;
}

void GlowConfigDialog::load( KConfig* /* conf */ )
{
	TQColor color;
	const TQColor defaultCloseButtonColor(DEFAULT_CLOSE_BUTTON_COLOR);
	const TQColor defaultMaximizeButtonColor(DEFAULT_MAXIMIZE_BUTTON_COLOR);
	const TQColor defaultIconifyButtonColor(DEFAULT_ICONIFY_BUTTON_COLOR);
	const TQColor defaultHelpButtonColor(DEFAULT_HELP_BUTTON_COLOR);
	const TQColor defaultStickyButtonColor(DEFAULT_STICKY_BUTTON_COLOR);

	_glowConfig->setGroup("General");

	_buttonConfigMap = new TQColor[NUMBER_OF_BUTTONS];
	color = _glowConfig->readColorEntry("stickyButtonGlowColor",
			&defaultStickyButtonColor);
	_buttonConfigMap[stickyButton] = color;

	color = _glowConfig->readColorEntry("helpButtonGlowColor",
			&defaultHelpButtonColor);
	_buttonConfigMap[helpButton] = color;

	color = _glowConfig->readColorEntry("iconifyButtonGlowColor",
			&defaultIconifyButtonColor);
	_buttonConfigMap[iconifyButton] = color;

	color = _glowConfig->readColorEntry("maximizeButtonGlowColor",
			&defaultMaximizeButtonColor);
	_buttonConfigMap[maximizeButton] = color;

	color = _glowConfig->readColorEntry("closeButtonGlowColor",
			&defaultCloseButtonColor);
	_buttonConfigMap[closeButton] = color;

	_showResizeHandle = _glowConfig->readBoolEntry("showResizeHandle", true);
	_titlebarGradientType = static_cast<KPixmapEffect::GradientType>
							(_glowConfig->readNumEntry("titlebarGradientType",
							KPixmapEffect::DiagonalGradient));

	_showResizeHandleCheckBox->setChecked(_showResizeHandle);
	_titlebarGradientTypeComboBox->setCurrentItem(_titlebarGradientType);

	_theme_name = _glowConfig->readEntry ("themeName", "default");
	_theme_list_view->setSelected (
			_theme_list_view->findItem (_theme_name, 0), true);
        slotTitleButtonClicked(0);
}

void GlowConfigDialog::save( KConfig* /* conf */ )
{
	_glowConfig->setGroup("General");

	_glowConfig->writeEntry("stickyButtonGlowColor", _buttonConfigMap[stickyButton]);
	_glowConfig->writeEntry("helpButtonGlowColor", _buttonConfigMap[helpButton]);
	_glowConfig->writeEntry("iconifyButtonGlowColor", _buttonConfigMap[iconifyButton]);
	_glowConfig->writeEntry("maximizeButtonGlowColor", _buttonConfigMap[maximizeButton]);
	_glowConfig->writeEntry("closeButtonGlowColor", _buttonConfigMap[closeButton]);

	_glowConfig->writeEntry("showResizeHandle", _showResizeHandle);
	_glowConfig->writeEntry("titlebarGradientType", _titlebarGradientType);

	_glowConfig->writeEntry ("themeName", _theme_name);

	_glowConfig->sync();
}

void GlowConfigDialog::defaults()
{
	const TQColor defaultCloseButtonColor = DEFAULT_CLOSE_BUTTON_COLOR;
	const TQColor defaultMaximizeButtonColor(DEFAULT_MAXIMIZE_BUTTON_COLOR);
	const TQColor defaultIconifyButtonColor(DEFAULT_ICONIFY_BUTTON_COLOR);
	const TQColor defaultHelpButtonColor(DEFAULT_HELP_BUTTON_COLOR);
	const TQColor defaultStickyButtonColor(DEFAULT_STICKY_BUTTON_COLOR);

	_buttonConfigMap[stickyButton] = defaultStickyButtonColor;
	_buttonConfigMap[helpButton] = defaultHelpButtonColor;
	_buttonConfigMap[iconifyButton] = defaultIconifyButtonColor;
	_buttonConfigMap[maximizeButton] = defaultMaximizeButtonColor;
	_buttonConfigMap[closeButton] = defaultCloseButtonColor;

	_showResizeHandle = true;
	_titlebarGradientType = KPixmapEffect::DiagonalGradient;

	_showResizeHandleCheckBox->setChecked(_showResizeHandle);
	_titlebarGradientTypeComboBox->setCurrentItem(_titlebarGradientType);

	_theme_list_view->setSelected (
			_theme_list_view->findItem("default", 0), true);
}

void GlowConfigDialog::slotLoadThemeList ()
{
	TQStringList dir_list=KGlobal::dirs()->findDirs("data", "kwin/glow-themes");

	TQStringList::ConstIterator it;

	_theme_list_view->clear();
	new TQListViewItem (_theme_list_view, "default", "17x17");

	for (it=dir_list.begin(); it!=dir_list.end(); ++it)
	{
		TQDir dir (*it, TQString("*"), TQDir::Unsorted,
				TQDir::Dirs | TQDir::Readable);
		if (dir.exists())
		{
			TQFileInfoListIterator it2(*dir.entryInfoList());
			TQFileInfo * finfo;

			while ((finfo=it2.current()))
			{
				if (finfo->fileName() == "." || finfo->fileName() == "..") {
					++it2;
					continue;
				}

				if (! _theme_list_view->findItem (finfo->fileName(), 0))
				{
					KConfig conf (dir.path() + "/" + finfo->fileName() + "/" +
							finfo->fileName() + ".theme");
					TQSize button_size = conf.readSizeEntry (
						"buttonSize", new TQSize (-1, -1));
					if (button_size.width() == -1)
					{
						++it2;
						continue;
					}
					TQString size_string = TQString("") +
						TQString::number(button_size.width()) +
						"x" + TQString::number(button_size.height());
					new TQListViewItem (_theme_list_view,
							finfo->fileName(), size_string);
				}

				++it2;
			}
		}
	}
}

void GlowConfigDialog::slotTitlebarGradientTypeChanged(int index)
{
	_titlebarGradientType = static_cast<KPixmapEffect::GradientType>(index);
	emit changed();
}

void GlowConfigDialog::slotResizeHandleCheckBoxChanged()
{
	_showResizeHandle = _showResizeHandleCheckBox->isChecked();
	emit changed();
}

void GlowConfigDialog::slotTitleButtonClicked(int index)
{
	for( int i=0; i< ((int) _titleButtonList.size()); i++ ) {
		_titleButtonList[i]->setDown(i==index);
	}
	_colorButton->setEnabled(true);
	_colorButton->setColor(_buttonConfigMap[index]);
}

void GlowConfigDialog::slotColorButtonChanged(const TQColor& glowColor)
{
	if( _stickyButton->isDown() ) {
		_buttonConfigMap[stickyButton] = glowColor;
	} else if( _helpButton->isDown() ) {
		_buttonConfigMap[helpButton] = glowColor;
	} else if( _iconifyButton->isDown() ) {
		_buttonConfigMap[iconifyButton] = glowColor;
	} else if( _maximizeButton->isDown() ) {
		_buttonConfigMap[maximizeButton] = glowColor;
	} else {
		_buttonConfigMap[closeButton] = glowColor;
	}
	emit changed();
}

void GlowConfigDialog::slotThemeListViewSelectionChanged ()
{
	if( _theme_list_view->selectedItem() != 0 ) {
		_theme_name = _theme_list_view->selectedItem()->text (0);

		emit changed();
	}
}

#include "glowconfigdialog.moc"
