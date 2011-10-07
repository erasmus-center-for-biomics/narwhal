# compile options
CC=g++
CFLAGS=-c -g -Wall-O4
LDFLAGS=-g -L/usr/lib64 -static
SOURCES=src/main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/qseq2fastq

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
