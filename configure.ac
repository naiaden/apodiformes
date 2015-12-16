AC_PREREQ([2.67])
AC_INIT([apodiformes], [1.0], [l.onrust@let.ru.nl])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

#AC_DEFINE_DIR([SYSCONF_PATH], sysconfdir, sysconfdir)

if test x"${CXXFLAGS+set}" = xset; then
  # the user set CXXFLAGS; don't override it.
  cxx_flags_were_set=true
else
  cxx_flags_were_set=false
fi


# Checks for programs.
#AC_PROG_CXX( [g++] )
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([ext])

if $cxx_flags_were_set; then
  CXXFLAGS=$CXXFLAGS
fi

# use libtool
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# when running tests, use CXX
# Checks for libraries.

if test $prefix = "NONE"; then
   prefix="$ac_default_prefix"
fi



PKG_PROG_PKG_CONFIG


# ugly hack when PKG_CONFIG_PATH isn't defined.
# couldn't get it to work otherwise
if test "x$PKG_CONFIG_PATH" = x; then
     export PKG_CONFIG_PATH=""
fi

AC_ARG_WITH(colibricore,
       [  --with-colibricore=DIR       use colibri-core installed in <DIR>; 
               note that you can install colibri-core in a non-default directory with
               ./configure --prefix=<DIR> in the colibri-core installation directory],
       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$withval/lib/pkgconfig"],
       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$prefix/lib/pkgconfig"])
  AC_MSG_NOTICE( [pkg-config search path: $PKG_CONFIG_PATH] )
PKG_CHECK_MODULES([colibricore], [colibri-core >= 0.5.3] )


AC_CHECK_LIB(zlib, zlib)

DIR_GLOG=/usr/local/glog-0.3.1
DIR_GLOG_LIB_DEFAULT=$DIR_GLOG/lib
DIR_GLOG_INC_DEFAULT=$DIR_GLOG/include

#LDFLAGS='-lpthread'
#INCLUDES='-I$(top_srcdir)/src -I$(top_srcdir)/lib/gtest/include'

dnl **************************************************************************
dnl Check for presence of Google C++ Logging Library (glog)
dnl **************************************************************************

AC_ARG_WITH(glog-includes,
			AC_HELP_STRING([--with-glog-includes=DIR],
						   [Specify location of Google C++ Logging Library headers]),
						   [INCLUDES="$INCLUDES -I$withval"],
						   [INCLUDES="$INCLUDES -I${DIR_GLOG_INC_DEFAULT}"])

AC_ARG_WITH(glog-libs,
			AC_HELP_STRING([--with-glog-libs=DIR],
						   [Specify location of Google C++ Logging Library libs]),
						   [LIB_GLOG_PATH="$withval"],
						   [LIB_GLOG_PATH="${DIR_GLOG_LIB_DEFAULT}"])

LIBS="$LIBS -L${LIB_GLOG_PATH} -lglog"

CXXFLAGS="$CXXFLAGS $colibricore_CFLAGS"
LIBS="$LIBS $colibricore_LIBS -lz -lbz2"



# Checks for library functions.

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  include/Makefile
])
AC_OUTPUT

