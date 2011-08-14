/***************************************************************************
                          glowconfigdialog.h  -  description
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

#ifndef GLOW_CONFIG_DIALOG_H
#define GLOW_CONFIG_DIALOG_H

#include <tqvaluevector.h>
#include <tqobject.h>

class TQListView;
class TQPushButton;
class TQSignalMapper;
class TQCheckBox;
class TQComboBox;
class KConfig;
class KColorButton;

class GlowConfigDialog : public TQObject
{
	Q_OBJECT
  TQ_OBJECT

public:
	GlowConfigDialog( KConfig* conf, TQWidget* parent );
	~GlowConfigDialog();

signals:
	void changed();

public slots:
	void load( KConfig* conf );
	void save( KConfig* conf );
	void defaults();

protected slots:
	void slotTitleButtonClicked(int);
	void slotColorButtonChanged(const TQColor&);
	void slotTitlebarGradientTypeChanged(int);
	void slotResizeHandleCheckBoxChanged();
	void slotThemeListViewSelectionChanged ();

private slots:
	void slotLoadThemeList ();
	
private:
	enum ButtonType{stickyButton, helpButton, iconifyButton,
		maximizeButton, closeButton };

	KConfig *_glowConfig;

	bool _showResizeHandle;
	KPixmapEffect::GradientType _titlebarGradientType;
	TQString _theme_name;

	TQWidget *_main_group_box;
	TQGroupBox *_button_glow_color_group_box;
	TQGroupBox *_theme_group_box;

	TQListView * _theme_list_view;

	TQCheckBox *_showResizeHandleCheckBox;
	TQComboBox *_titlebarGradientTypeComboBox;

	TQPushButton *_stickyButton;
	TQPushButton *_helpButton;
	TQPushButton *_iconifyButton;
	TQPushButton *_maximizeButton;
	TQPushButton *_closeButton;
	TQSignalMapper *_titleButtonMapper;

	TQColor* _buttonConfigMap;
	TQValueVector<TQPushButton*> _titleButtonList;
	
	KColorButton *_colorButton;
};

#endif

