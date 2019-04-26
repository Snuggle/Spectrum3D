#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

############### INITIALIZE EVERYTHING ######################

AC_INIT([Spectrum3d],
	[2.0.1],
	[nadaeck@hotmail.com], 
	[spectrum3d])
AC_CONFIG_SRCDIR([Makefile.am])

AM_INIT_AUTOMAKE([-Wall])

AC_PROG_CC
AM_PROG_CC_C_O

AC_CONFIG_HEADERS([src/config.h])

################ LOOK FOR 'DISABLE' OPTIONS #####################

AC_ARG_ENABLE([jack],
		AS_HELP_STRING([--disable-jack],
		[Disable Jack support]))
AC_ARG_ENABLE([geis],
		AS_HELP_STRING([--disable-geis],
		[Disable multitouch support]))

################# CHECK FOR LIBRAIRIES ######################

BAMF_REQUIRED=0.2.53
GEIS_REQUIRED=1.0.15
jack=1
geis=1

# Check and link for GTK+-2.0
PKG_CHECK_MODULES([GTK], [gtk+-2.0], [
				CFLAGS="$CFLAGS $GTK_CFLAGS"
   				LIBS="$LIBS $GTK_LIBS"
				], [AC_MSG_ERROR([GTK (gtk+-2.0) library (libgtk) is required])])

# Check and link for GSTREAMER-0.10
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-0.10], [
				CFLAGS="$CFLAGS $GSTREAMER_CFLAGS"
   				LIBS="$LIBS $GSTREAMER_LIBS"
				], [AC_MSG_ERROR([GSTREAMER (gstreamer-0.10) library (libgstreamer) is required])])

#Check and link for SDL and OpenGL
AC_CHECK_LIB([SDL], [SDL_Init], [],
	[AC_MSG_ERROR([SDL library (libsdl) is required for this program])])
AC_CHECK_LIB([GL], [glClear], [],
	[AC_MSG_ERROR([OpenGL(GL) library is required for this program])])
AC_CHECK_LIB([GLU], [gluBeginCurve], [],
	[AC_MSG_ERROR([OpenGL(GLU) library is required for this program])])

# Check and link for JACK (if jacklib is present and if this option is not disabled)
AS_IF([test "x$enable_jack" != "xno"], [
  AC_CHECK_LIB([jack], [jack_client_open], [], [jack=0])],
	[jack=0])

# Check and link for MULTITOUCH (if libbamf and libutouch-geis are present and if this option is not disabled)
# Check first for LIBBAMF; if it is there, check for LIBUTOUCH-GEIS version 2; if it is there, check if LIBUTOUCH-GEIS version is > 2.0.11
AS_IF([test "x$enable_geis" != "xno"], [
  PKG_CHECK_MODULES([BAMF], [libbamf >= $BAMF_REQUIRED], [
	CFLAGS="$CFLAGS $BAMF_CFLAGS"
  	LIBS="$LIBS $BAMF_LIBS"
	PKG_CHECK_MODULES([GEIS], [libutouch-geis > GEIS_REQUIRED], [
		AC_DEFINE([HAVE_LIBUTOUCH_GEIS], [1], [Define to 1 if you have the `utouch-geis' library version 2 (advanced API) (-lutouch-geis).])
		CFLAGS="$CFLAGS $GEIS_CFLAGS"
   		LIBS="$LIBS $GEIS_LIBS"
		PKG_CHECK_MODULES([GEIS_12], [libutouch-geis > 2.0.11], [
			AC_DEFINE([GEIS_12], [1], [Define to 1 if you have the `utouch-geis' library > 2.0.11.])
			], [geis12=0])
		], [geis=0])
	], [geis=0])],
	[geis=0])

# Checks for header files.		
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for library functions.
AC_CHECK_FUNCS([strchr strstr strtol])

AC_CONFIG_FILES([Makefile
		src/Makefile
		data/Makefile
		data/spectrum3d.desktop
		doc/Makefile])

AC_OUTPUT

mkdir -p $HOME/.$PACKAGE


################### PRINT CONFIG MESSAGE ######################

AS_ECHO()
AC_MSG_NOTICE([     ******************************************])
AS_IF([test "$jack" != "0"], 
	[AC_MSG_NOTICE([     *  JACK support.....................YES  *])],
	[AC_MSG_NOTICE([     *  JACK support.....................NO   *])])
AS_IF([test "$geis" != "0"], 
	[AC_MSG_NOTICE([     *  MULTITOUCH support...............YES  *])],
	[AC_MSG_NOTICE([     *  MULTITOUCH support...............NO   *])])
AC_MSG_NOTICE([     ******************************************])
AS_ECHO()



