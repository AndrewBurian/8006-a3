# generic makefile
CC=g++
CFLAGS=-c -g -Wall -std=c++11
LDFLAGS=
LIBS=
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=banhammer.exe

SOURCES=main.cpp filter.cpp action.cpp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
