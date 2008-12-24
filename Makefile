VERSION = 0.1.1

CC         = gcc
CXX        = g++
BINDIR     = /usr/bin
LJS_LIBDIR = /usr/lib/lulzjs
CFLAGS     = -DXP_UNIX -D__LJS_LIBRARY_PATH__="\"${LJS_LIBDIR}/\"" -D__LJS_VERSION__="\"${VERSION}\""
LDFLAGS    = -ljs

ifdef DEBUG
CFLAGS += -DDEBUG -g
endif

CORE         = src/core/main.o src/core/Core.o src/core/Misc.o src/core/Preprocessor.o
CORE_CFLAGS  = ${CFLAGS}
CORE_LDFLAGS = ${LDFLAGS} -ldl

LIB_SYSTEM         = src/lib/System/System.o src/lib/System/IO/IO.o src/lib/System/IO/File.o
LIB_SYSTEM_CFLAGS  = ${CFLAGS}
LIB_SYSTEM_LDFLAGS = ${LDFLAGS}

all: core libcore libsystem

debug:
	CFLAGS="${CFLAGS} -DDEBUG"
	$(call all)

core: $(CORE)
	${CC} ${CORE_LDFLAGS} ${CORE_CFLAGS} ${CORE} -o ljs

core_install:
	cp ljs ${BINDIR}/

libcore:

libcore_install:
	mkdir -p ${LJS_LIBDIR}/Core
	cp -rf src/core/Core/*.js ${LJS_LIBDIR}/Core/
	
libsystem: $(LIB_SYSTEM)

$(LIB_SYSTEM): $(LIB_SYSTEM:.o=.c)
	${CC} ${LIB_SYSTEM_LDFLAGS} ${LIB_SYSTEM_CFLAGS} -fPIC -c $*.c -o $*.lo
	${CC} -shared -Wl,-soname,`basename $*`.so -o $*.o $*.lo -lc

libsystem_install:
	mkdir -p ${LJS_LIBDIR}/System
	mkdir -p ${LJS_LIBDIR}/System/IO
	cp src/lib/System/init.js    ${LJS_LIBDIR}/System/init.js
	cp src/lib/System/System.o   ${LJS_LIBDIR}/System/System.so
	cp src/lib/System/IO/IO.o    ${LJS_LIBDIR}/System/IO/IO.so
	cp src/lib/System/IO/File.o  ${LJS_LIBDIR}/System/IO/File.so
	cp src/lib/System/IO/File.js ${LJS_LIBDIR}/System/IO/File.js
	cp src/lib/System/Console.js ${LJS_LIBDIR}/System/Console.js

libsystem_uninstall:

install: all core_install libcore_install libsystem_install
	chmod -R a+rx ${LJS_LIBDIR}
	chmod a+rx ${BINDIR}/ljs

uninstall:
	rm -f ${BINDIR}/ljs
	rm -rf ${LJS_LIBDIR}
	

clean:
	rm -f ljs;
	rm -f src/core/*.o;
	rm -f src/lib/System/*.*o; rm -f src/lib/System/IO/*.*o

