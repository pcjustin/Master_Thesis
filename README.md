DynamicStreamServer
===================

Dynamic Streaming Server via HTTP Live Streaming

/* link to remote server */
git remote add github https://github.com/pcjustin/DynamicStreamServer.git

/* local */
git add .
git commit -m "comments"

/* remote */
git pull
git pull github master
git push github master

/* automake */
autoscan
mv configure.scan configure.in

/* add the lines into configure.in
AC_CHECK_LIB([avcodec],[avcodec_alloc_frame],,AC_MSG_ERROR([You need libavcodec (avcodec_alloc_frame) to build $PACKAGE]))
AM_INIT_AUTOMAKE("myshell", "0.01")
AC_CONFIG_FILES(Makefile
        src/Makefile)

/* create Makefile.am */
AUTOMAKE_OPTIONS= foreign 
SUBDIRS = src
LDFLAGS += -L/usr/lib
bin_PROGRAMS = myshell
myshell_SOURCES = myshell.c
myshell_LDADD = -lQuantLib

aclocal
autoconf
autoheader
automake --add-missing
./configure
make

