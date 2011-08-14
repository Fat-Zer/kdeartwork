#include "helpers.h"

#include <kapplication.h>

void min_width(TQWidget *w) {
  w->setMinimumWidth(w->tqsizeHint().width());
}

void fixed_width(TQWidget *w) {
  w->setFixedWidth(w->tqsizeHint().width());
}

void min_height(TQWidget *w) {
  w->setMinimumHeight(w->tqsizeHint().height());
}

void fixed_height(TQWidget *w) {
  w->setFixedHeight(w->tqsizeHint().height());
}

void min_size(TQWidget *w) {
  w->setMinimumSize(w->tqsizeHint());
}

void fixed_size(TQWidget *w) {
  w->setFixedSize(w->tqsizeHint());
}

KConfig *klock_config()
{
    TQString name( kapp->argv()[0] );
    int slash = name.findRev( '/' );
    if ( slash )
	name = name.mid( slash+1 );

    return new KConfig( name + "rc" );
}

