# compile options
CC=g++
CFLAGS=-c -g -Wall
LDFLAGS=-g -L/usr/lib64 -static
SOURCES=src/main.c src/fastq.c src/linkedlist.c src/demultiplex.c src/divide.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/demultiplex

# dist options
PACKAGE="Narwhal"
VERSION=` date +"%Y.%m.%d"`
ARCHIVE=$(PACKAGE)-$(VERSION)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)
	rm -f build/*.o
	rm -f src/*.o
	rm -f build/*.so
	rm -f src/*.so

dist:
	tar -zcf $(ARCHIVE).tar .
