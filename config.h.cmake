#define VERSION "@VERSION@"

#cmakedefine HAVE_NUMERIC_LIMITS 1

/* Defines where xscreensaver stores it's config files */
#define XSCREENSAVER_CONFIG_DIR "${XSCREENSAVER_CONFIG_DIR}"

/* Defines where xscreensaver stores its graphic hacks */
#define XSCREENSAVER_HACKS_DIR "${XSCREENSAVER_DIR}"

/* Defines if you have GL (Mesa, OpenGL, ...) */
#cmakedefine HAVE_GL 1

/* Defines if you have GL/xmesa.h header file. */
#cmakedefine HAVE_GL_XMESA_H 1

/* Defines if you have GL/glut.h header file. */
#cmakedefine HAVE_GL_XMESA_H 1

/* Defines if you have X11/dirent.h header file. */
#cmakedefine HAVE_DIRENT_H 1

/* Defines if you have sys/ndir.h header file. */
#cmakedefine HAVE_SYS_NDIR_H 1

/* Defines if you have sys/dir.h header file. */
#cmakedefine HAVE_SYS_DIR_H 1

/* Defines if you have ndir.h header file. */
#cmakedefine HAVE_NDIR_H 1

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H 1

