
# compile options
CC=g++
CFLAGS=-c -g -Wall -I/home/cl15/rutger/Prog/bamtools/include -O4
LDFLAGS=-g -L/home/cl15/rutger/Prog/bamtools/lib -L/usr/lib64 -lbamtools -lz -static
SOURCES=src/main.cpp src/ReadCnt.cpp src/BaseCnt.cpp src/CigarCnt.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/bamstats

# dist options
PACKAGE="bamstats"
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
