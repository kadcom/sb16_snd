# Watcom WMake for DOS
DEBUG=1
PROC_TYPE=8086

# Set the path to the Watcom compiler
CC=wcc
CFLAGS=-ms -bt=dos -0 -wx -w4

CXX=wpp
CXXFLAGS=-ms -bt=dos -0 -wx -w4

LINKER_FLAGS= 

!if $(DEBUG)
CFLAGS+= -d2
CXXFLAGS+= -d2
LINKER_FLAGS+= debug all option map 
!else
CFLAGS+= -d0 -s
CXXFLAGS+= -d0 -s
LINKER_FLAGS+= option quiet
!endif

!if $(PROC_TYPE)==8086
CFLAGS+= -0 
CXXFLAGS+= -0
!elseif $(PROC_TYPE)==80286
CFLAGS+= -2
CXXFLAGS+= -2
!elseif $(PROC_TYPE)==80386
CFLAGS+= -3
CXXFLAGS+= -3
!elseif $(PROC_TYPE)==80486
CFLAGS+= -4
CXXFLAGS+= -4
!elseif $(PROC_TYPE)==Pentium
CFLAGS+= -5
CXXFLAGS+= -5
!else 
CFLAGS+= -0
CXXFLAGS+= -0
!endif

# Set the path to the Watcom linker
LINK=wlink

# Set the path to the Watcom librarian
LIB=wlib

TARGET=sb16.exe

SOURCES=main.c sb16.c

HEARDERS=sb16.h

all: $(TARGET)

$(TARGET): main.obj sb16.obj
	$(LINK) system dos $LINKER_FLAGS name $@ file  { main.obj sb16.obj }

main.obj: main.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

sb16.obj: sb16.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

clean: .SYMBOLIC
	del *.obj
	del *.exe
