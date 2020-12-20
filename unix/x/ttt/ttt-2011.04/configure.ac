AC_INIT([ttt], [2011.04], [http://www.newbreedsoftware.com/ttt])
AC_COPYRIGHT([(C) 2011 Cyber Switching Inc., 2003 Bill Kendrick])

AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_MACRO_DIR([m4])

AC_PREFIX_DEFAULT([/usr])

AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_FILES([Makefile])

AC_CHECK_HEADERS([arpa/inet.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([ctype.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([errno.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([fcntl.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([float.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([getopt.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([inttypes.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([math.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([pthread.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([setjmp.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([signal.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([stdarg.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([stdio.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([stdlib.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([string.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([sys/select.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([sys/stat.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([sys/types.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([sys/wait.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([syslog.h]	,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([unistd.h]	,,AC_MSG_ERROR(missing header))

AC_CHECK_HEADERS([SDL/SDL.h]		,,AC_MSG_ERROR(missing header))
AC_CHECK_HEADERS([SDL/SDL_mixer.h]	,,AC_MSG_ERROR(missing header))

AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
AC_PROG_GREP
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_SED
LT_INIT

dnl AC_CHECK_LIB([duma],		[_duma_malloc]		,,AC_MSG_ERROR(missing library))
AC_CHECK_LIB([SDL],		[SDL_Init]		,,AC_MSG_ERROR(missing library))
AC_CHECK_LIB([SDL_mixer],	[Mix_OpenAudio]		,,AC_MSG_ERROR(missing library))

AC_CHECK_PROG(SDL_CFLAGS, sdl-config, `sdl-config --cflags`)
AC_CHECK_PROG(SDL_LIBS, sdl-config, `sdl-config --libs`)

CFLAGS="${CFLAGS} ${SDL_CFLAGS}"
LIBS="${LIBS} ${SDL_LIBS}"

AC_FUNC_FORK

AC_STRUCT_TIMEZONE

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

AM_PROG_CC_C_O

AC_OUTPUT

echo "
$PACKAGE_NAME $PACKAGE_VERSION configuration:
-----------------------------
  Source code location:       ${srcdir}
  Host System Type:           ${host}
  Compiler:                   ${CC}
  Standard CFLAGS:            ${CFLAGS} ${ac_devel_default_warnings}
  Standard libraries:         ${LIBS}
  Install path (prefix):      ${prefix}

  Now type 'make' to build $PACKAGE_NAME $PACKAGE_VERSION.
"
