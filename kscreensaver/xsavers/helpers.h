#ifndef __HELPERS__H__
#define __HELPERS__H__

#include <tqwidget.h>
#include <kconfig.h>

void min_width(TQWidget *);
void fixed_width(TQWidget *);
void min_height(TQWidget *);
void fixed_height(TQWidget *);
void min_size(TQWidget *);
void fixed_size(TQWidget *);

/*
 * Use this to get a KConfig object that uses a reasonable config filename.
 * KGlobal::config() will use the klockrc config file.
 *
 * Caller must delete the object when finished.
 */
KConfig *klock_config();

#endif

