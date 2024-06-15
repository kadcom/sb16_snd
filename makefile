# Watcom WMake for DOS
DEBUG=1
PROC_TYPE=80486
PROTECTED_MODE=0

TARGET_PLATFORM=dos 

!if $(PROTECTED_MODE)
TARGET_PLATFORM=dos4g 
!endif


LINKER_FLAGS= 

# Set the path to the Watcom compiler
CC=wcc
CFLAGS=-ms -bt=$(TARGET_PLATFORM) -wx -w4

CXX=wpp
CXXFLAGS=-ms -bt=$(TARGET_PLATFORM) -wx -w4

!if $(PROTECTED_MODE)
CC=wcc386
CFLAGS=-bt=$(TARGET_PLATFORM) -wx -w4

CXX=wpp386
CXXFLAGS=-bt=$(TARGET_PLATFORM) -wx -w4

LINKER_FLAGS+= option heap=1024 
!endif


!if $(DEBUG)
CFLAGS+= -d2
CXXFLAGS+= -d2
LINKER_FLAGS+= debug all option map 
!else
CFLAGS+= -d0 -s -ot 
CXXFLAGS+= -d0 -s -ot
LINKER_FLAGS+= option quiet
!endif

!if $(PROTECTED_MODE)

!if $(PROC_TYPE)==80386
CFLAGS+= -3r
CXXFLAGS+= -3r
!elseif $(PROC_TYPE)==80486 
CFLAGS+= -4r
CXXFLAGS+= -4r
!elseif $(PROC_TYPE)==Pentium
CFLAGS+= -5r
CXXFLAGS+= -5r
!else 
!endif 

!else
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
!endif

# Set the path to the Watcom linker
LINK=wlink

# Set the path to the Watcom librarian
LIB=wlib

TARGET=sb16.exe

SOURCES=main.c sb16.c sb_irq.c sb_dma.c

HEARDERS=sb16.h platform.h sb_dma.h sb_irq.h

OBJECTS=main.obj sb16.obj sb_irq.obj sb_dma.obj

all: $(TARGET)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(LINK) system $(TARGET_PLATFORM) $LINKER_FLAGS name $@ file  { $(OBJECTS) }

main.obj: main.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

sb_dma.obj: sb_dma.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

sb16.obj: sb16.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

sb_irq.obj: sb_irq.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

clean: .SYMBOLIC
	del *.obj
	del *.exe
