OBJECTS = main.o bufferList.o

TOPDIR:=$(shell pwd)

CFLAGS=-Wall -O2

TARGET=Intel
CLIBS=-L/usr/lib -lSDL -lSDL_image -lSDL_ttf -lSDL_gfx -lavcodec -lavformat -lswscale -lavutil
CFLAGS+=-I$(TOPDIR)/headers -I/usr/include/SDL -I/usr/include/libxml2 -DDEBUG=0 -D__STDC_CONSTANT_MACROS
LIB_NAME=GraphAPI.lib

CXX=$(CROSS_COMPILE)g++
CC=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
LD=$(CC)
OBJCOPY=$(CROSS_COMPILE)objcopy
REMOVE=rm

APPLICATION_NAME=demoPlayer
TARGET=linux

all:$(OBJECTS)
	$(CC) $(INCL) $(CFLAGS) $(OBJECTS) -o $(APPLICATION_NAME) $(LIB_NAME) $(CLIBS)
	@echo Compiled $(APPLICATION_NAME) for $(TARGET)

.PHONY : all

clean:
	rm -f *.o $(APPLICATION_NAME) 

.PHONY : clean
