# generic makefile
CC=g++
CFLAGS=-c -g -Wall
LDFLAGS=
LIBS=
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=banhammer.exe

SOURCES=main.cpp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
