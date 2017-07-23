# unix/mac/win32
PLATFORM = unix

# compiler
CC = gcc

# version control system
CI = ci
CO = co
RLOG = rlog

# external libraries

BLKSIZE = 4096
TARGET = micery
USE_RCS = 0

ifeq ($(PLATFORM),unix)
LIBS = `sdl-config --libs` -lGLU -lGL -lm
COPT = -Wall -ffast-math -O2 -Wall `sdl-config --cflags`\
-DBLKSIZE=$(BLKSIZE)
endif

ifeq ($(PLATFORM),mac)
BLKSIZE = 1024
LIBS = `sdl-config --static-libs` -lm
COPT = -ffast-math -O2 -Wall `sdl-config --cflags` -DMAC\
-I/System/Library/Frameworks/AGL.framework/Headers\
-DBLKSIZE=$(BLKSIZE)
endif

ifeq ($(PLATFORM),win32)
LIBS = -mwindows -lSDL -lglu32 -lopengl32 -lm
COPT = -ffast-math -O2 -Wall -DBLKSIZE=$(BLKSIZE) -DNO_STRSEP
TARGET = micery.exe
endif

# All the objects
OBJ = 	mouse.o gl_render.o syna.o musa.o writer.o intro.o dump.o\
	wavefront.o misc.o steering.o steer_render.o

$(TARGET): main.o $(OBJ)
	$(CC) $(COPT) -o $@ $(OBJ) $< $(LIBS)
	strip $@

main.o: main.c
ifeq ($(USE_RCS),1)
	$(CI) -l $<
endif
	$(CC) $(COPT) -o $@ -c $<

%.o: %.c %.h
ifeq ($(USE_RCS),1)
	$(CI) -l $<
endif
	$(CC) $(COPT) -o $@ -c $<

clean:
	rm *~ *.o *.bak $(TARGET)
