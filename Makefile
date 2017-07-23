# unix/mac/win32
PLATFORM = unix
SHELL = /bin/sh

# compiler
CC = gcc

# version control system
CI = ci
CO = co
RLOG = rlog

# external libraries

BLKSIZE = 4096
TARGET = micery-by-fitandbandwagon
USE_RCS = 0

ifeq ($(PLATFORM),unix)
LDFLAGS += $(shell pkgconf --libs sdl glu ) -lm
CFLAGS += -O3 -flto -Wall -ffast-math $(shell pkgconf --cflags sdl glu ) -std=gnu90 -pipe -DBLKSIZE=$(BLKSIZE)
endif

# All the objects
OBJ = 	mouse.o gl_render.o syna.o musa.o writer.o intro.o dump.o\
	wavefront.o misc.o steering.o steer_render.o

#variable de nettoyage
RM_F = rm -f
#variables d'instalation
INSTALL = install
INSTALL_DIR     = $(INSTALL) -p -d -o root -g root  -m  755
INSTALL_PROGRAM = $(INSTALL) -p    -o root -g root  -m  755
INSTALL_DATA    = $(INSTALL) -p    -o root -g root  -m  644

prefix          = /usr
exec_prefix     = $(prefix)
bindir          = $(prefix)/bin

#parallel compilation if available
ifneq (,$(filter parallel=%,$(DEB_BUILD_OPTIONS)))
 NUMJOBS = $(patsubst parallel=%,%,$(filter parallel=%,$(DEB_BUILD_OPTIONS)))
 MAKEFLAGS += -j$(NUMJOBS)
endif

export

all: $(TARGET)

$(TARGET): main.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $< $(LDFLAGS)

main.o: main.c
ifeq ($(USE_RCS),1)
	$(CI) -l $<
endif
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.c %.h
ifeq ($(USE_RCS),1)
	$(CI) -l $<
endif
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-@$(RM_F) *~ *.o *.bak $(TARGET)

install: $(TARGET)
	$(INSTALL_DIR) $(DESTDIR)$(bindir)
	-@$(RM_F) $(DESTDIR)$(bindir)/$(TARGET)
	$(INSTALL_PROGRAM) $(TARGET) $(DESTDIR)$(bindir)

