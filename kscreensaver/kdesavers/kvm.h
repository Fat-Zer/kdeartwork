//-----------------------------------------------------------------------------
//
// kvm screensaver
//

#ifndef __KVM_H__
#define __KVM_H__

#include <tqtimer.h>
#include <tqptrlist.h>

#include <kdialogbase.h>
#include <kscreensaver.h>

extern "C" {
#include "vm.h"
#include "vm_random.h"
}

#define	THREAD_MAX_STACK_SIZE	10
#define	MAX_THREADS_NUM		20

#define	MAX_REFRESH_TIMEOUT	40

typedef struct {
  TQWidget *w;
  int grid_width, grid_height;
  int grid_margin_x;
  int grid_margin_y;
  int char_width, char_height;
  bool insert_top_p, insert_bottom_p;
  int density;
  struct tvm_pool*	pool;
  char*	modified;
  int	show_threads;

  TQPixmap images;
  int image_width, image_height;
  int nglyphs;

} m_state;


class kVmSaver : public KScreenSaver
{
	Q_OBJECT
  TQ_OBJECT
public:
	kVmSaver( WId id );
	virtual ~kVmSaver();

	void setSpeed( int spd );
	void setRefreshTimeout( const int refreshTimeout );

protected:
	void blank();
	void readSettings();
        int getRandom( const int max_value );
        void modifyArea( const int op );

protected slots:
	void slotTimeout();

protected:
	TQTimer      timer;
	int         colorContext;

	int         speed;
	m_state*    pool_state;
        int	refreshStep;
        int	refreshTimeout;
};


class kVmSetup : public KDialogBase
{
	Q_OBJECT
  TQ_OBJECT
public:
	kVmSetup( TQWidget *parent = NULL, const char *name = NULL );
    ~kVmSetup();
protected:
	void readSettings();

private slots:
	void slotSpeed( int );
	void slotRefreshTimeout( int num );
	void slotOk();
	void slotHelp();

private:
	TQWidget *preview;
	kVmSaver *saver;

	int speed;
	int refreshTimeout;
};


#endif

