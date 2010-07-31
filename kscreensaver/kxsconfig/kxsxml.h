//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 2002
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

#ifndef KXSXML_H
#define KXSXML_H

#include "kxsconfig.h"
#include <tqxml.h>
#include <tqptrstack.h>

class KXSXmlHandler;

class KXSXml
{
public:
    KXSXml( TQWidget *p );
    
    bool parse( const TQString &filename );
    const TQPtrList<KXSConfigItem> *items() const;
    TQString description() const;

private:
    TQWidget *parent;
    KXSXmlHandler *handler;
};

class KXSXmlHandler : public QXmlDefaultHandler
{
public:
    KXSXmlHandler( TQWidget *p );

    bool startDocument();
    bool startElement( const TQString&, const TQString&, const TQString& ,
	    const TQXmlAttributes& );
    bool endElement( const TQString&, const TQString&, const TQString& );
    bool characters( const TQString & );

    const TQPtrList<KXSConfigItem> *items() const { return &mConfigItemList; }
    const TQString &description() const { return desc; }

private:
    TQWidget *parent;
    KXSSelectItem *selItem;
    bool inDesc;
    TQString desc;
    TQPtrList<KXSConfigItem> mConfigItemList;
    TQPtrStack<TQWidget> mParentStack;
};

#endif // KXSXML_H

