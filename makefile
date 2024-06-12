# Watcom WMake for DOS

# Set the path to the Watcom compiler
CC=wcc
CFLAGS=-zq -ms -bt=dos -0 -s -wx -w4 -d0

CXX=wpp
CXXFLAGS=-zq -ms -bt=dos -0 -s -wx -w4 -d0

# Set the path to the Watcom linker
LINK=wlink

# Set the path to the Watcom librarian
LIB=wlib

TARGET=sb16.exe

SOURCES=main.c sb16.c

HEARDERS=sb16.h

all: $(TARGET)

$(TARGET): main.obj sb16.obj
	$(LINK) system dos name $@ file  { main.obj sb16.obj }

main.obj: main.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

sb16.obj: sb16.c $(HEADERS)
	$(CC) $(CFLAGS) -fo=$@ $<

clean:
	del $(OBJECTS) $(TARGET)
