#include "helpers.h"

#include <kapplication.h>

void min_width(TQWidget *w) {
  w->setMinimumWidth(w->sizeHint().width());
}

void fixed_width(TQWidget *w) {
  w->setFixedWidth(w->sizeHint().width());
}

void min_height(TQWidget *w) {
  w->setMinimumHeight(w->sizeHint().height());
}

void fixed_height(TQWidget *w) {
  w->setFixedHeight(w->sizeHint().height());
}

void min_size(TQWidget *w) {
  w->setMinimumSize(w->sizeHint());
}

void fixed_size(TQWidget *w) {
  w->setFixedSize(w->sizeHint());
}

KConfig *klock_config()
{
    TQString name( kapp->argv()[0] );
    int slash = name.findRev( '/' );
    if ( slash )
	name = name.mid( slash+1 );

    return new KConfig( name + "rc" );
}

