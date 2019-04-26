#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

############### INITIALIZE EVERYTHING ######################

AC_INIT([Spectrum3d],
	[2.6.0],
	[nadaeck@hotmail.com], 
	[spectrum3d],
	[spectrum3d.sourceforge.net])
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
AC_ARG_ENABLE([gtk3],
		AS_HELP_STRING([--disable-gtk3],
		[Disable Gtk-3.0 (use Gtk-2.0 instead)]))
AC_ARG_ENABLE([gtk2],
		AS_HELP_STRING([--enable-gtk2],
		[Enable gtk2 support (instead of gtk3]))
AC_ARG_ENABLE([sdl2],
		AS_HELP_STRING([--disable-sdl2],
		[Disable SDL2 (use SDL1.2 instead)]))
AC_ARG_ENABLE([sdl],
		AS_HELP_STRING([--enable-sdl],
		[Enable SDL1.2 (instead of SDL2]))

################# CHECK FOR LIBRAIRIES ######################

BAMF_REQUIRED=0.2.53
GEIS_REQUIRED=1.0.15
jack=1
geis=1
gtk=0
gtkglext=0
gstreamer=0
sdl=1

# Check and link for GTK3
AS_IF([test "x$enable_gtk3" != "xno" && test "x$enable_gtk2" != "xyes"], [
  	PKG_CHECK_MODULES([GTK3], [gtk+-3.0], [
		gtk=3
		CFLAGS="$CFLAGS $GTK3_CFLAGS"
		LIBS="$LIBS $GTK3_LIBS"
		AC_DEFINE([GTK3], [1], [Define to 1 if you have the `GTK3' library])
		AS_IF([test "x$enable_gtkglext" == "xyes"], [
  		  	PKG_CHECK_MODULES([GTKGLEXT3], [gtkglext-3.0], [
				gtkglext=1
				CFLAGS="$CFLAGS $GTKGLEXT3_CFLAGS"
   				LIBS="$LIBS $GTKGLEXT3_LIBS"
				AC_DEFINE([GTKGLEXT3], [1], [Define to 1 if you have the `GTKGLEXT3' library])
				], [AC_MSG_ERROR([GTKGLEXT library for GTK3 (libgtkglext-3.0) is required])])
			], [])
		], [gtk=0])
  	], [])

# Check and link for GTK2
AS_IF([test "$gtk" == "0"], [
  	PKG_CHECK_MODULES([GTK2], [gtk+-2.0], [
		gtk=2
		CFLAGS="$CFLAGS $GTK2_CFLAGS"
		LIBS="$LIBS $GTK2_LIBS"
		AC_DEFINE([GTK2], [1], [Define to 1 if you have the `GTK2' library])
		AS_IF([test "x$enable_gtkglext" == "xyes"], [
  		  	PKG_CHECK_MODULES([GTKGLEXT1], [gtkglext-1.0], [
				gtkglext=1
				CFLAGS="$CFLAGS $GTKGLEXT1_CFLAGS"
   				LIBS="$LIBS $GTKGLEXT1_LIBS"
				AC_DEFINE([GTKGLEXT1], [1], [Define to 1 if you have the `GTKGLEXT1' library])
				], [AC_MSG_ERROR([GTKGLEXT library for GTK2 (libgtkglext-1.0) is required])])
		  	], [])
		], [AC_MSG_ERROR([GTK library (libgtk-2.0 or libgtk-3.0) is required])])
  	], [])

#Check and link for SDL
AS_IF([test "x$enable_sdl2" != "xno" && test "x$enable_sdl" != "xyes"], [
    AC_CHECK_LIB([SDL2], [SDL_Init], [],[])],
	[])

AS_IF([test "x$enable_sdl2" != "xno" && test "x$enable_sdl" != "xyes"], [
    AC_CHECK_LIB([SDL2], [SDL_Init], [
	sdl=2
	],[])],
	[])

AS_IF([test "$sdl" == "1"], [
    AC_CHECK_LIB([SDL], [SDL_Init], [],[AC_MSG_ERROR([SDL library (libsdl) is required for this program])])],
	[])
    
#Check and link for OpenGL
AC_CHECK_LIB([GL], [glClear], [],
	[AC_MSG_ERROR([OpenGL(GL) library is required for this program])])
AC_CHECK_LIB([GLU], [gluBeginCurve], [],
	[AC_MSG_ERROR([OpenGL(GLU) library is required for this program])])

# Check and link for GSTREAMER : version 1.0 is searched by default, then version 0.10 is searched
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-1.0], [
		gstreamer=1
		CFLAGS="$CFLAGS $GSTREAMER_CFLAGS"
		LIBS="$LIBS $GSTREAMER_LIBS"
		AC_DEFINE([GSTREAMER1], [1], [Define to 1 if you have the `Gstreamer-1.0' library])		  	
		], [
			PKG_CHECK_MODULES([GSTREAMER], [gstreamer-0.10], [
			gstreamer=0
			CFLAGS="$CFLAGS $GSTREAMER_CFLAGS"
			LIBS="$LIBS $GSTREAMER_LIBS"
			AC_DEFINE([GSTREAMER0], [1], [Define to 1 if you have the `Gstreamer-0.10' library])
			], [AC_MSG_ERROR([Gstreamer library (libgstreamer-0.10 or libgstreamer-1.0) is required])])
		])

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
	PKG_CHECK_MODULES([GEIS], [libgeis], [
		AC_DEFINE([HAVE_LIBGEIS], [1], [Define to 1 if you have the Geis library.])
		CFLAGS="$CFLAGS $GEIS_CFLAGS"
   		LIBS="$LIBS $GEIS_LIBS"
		], [geis=0])
	], [geis=0])],
	[geis=0])

# Checks for header files.		
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for library functions.
AC_CHECK_FUNCS([memset putenv strchr strerror strstr strtol])

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
AS_IF([test "$gstreamer" == "1"], 
	[AC_MSG_NOTICE([     *  Gstreamer version ...............1.0  *])],
	[AC_MSG_NOTICE([     *  Gstreamer version ..............0.10  *])])
AS_IF([test "$gtk" == "2"], 
	[AC_MSG_NOTICE([     *  GTK version .................GTK-2.0  *])],
	[AC_MSG_NOTICE([     *  GTK version .................GTK-3.0  *])])
AS_IF([test "$sdl" == "2"], 
	[AC_MSG_NOTICE([     *  SDL version.....................SDL2  *])],
	[AC_MSG_NOTICE([     *  SDL version...................SDL1.2  *])])
AS_IF([test "$jack" != "0"], 
	[AC_MSG_NOTICE([     *  JACK support.....................YES  *])],
	[AC_MSG_NOTICE([     *  JACK support......................NO  *])])
AC_MSG_NOTICE([     ******************************************])
AS_ECHO()



