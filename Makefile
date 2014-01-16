HEADERS = estimators.h

SOURCES = estimators.cpp \
	main.cpp

OBJECTS= $(SOURCES:.cpp=.o)

BINS = arfer

all: $(OBJECTS) $(BINS)

CXX = g++
CXXFLAGS = -O3 -D_FILE_OFFSET_BITS=64
INCLUDES = -I$(BAMTOOLS_ROOT)/include -Ivcflib/src -Ivcflib
LDFLAGS =
LIBS = -L./ -Lvcflib/tabixpp/ -ltabix -lz -lm 

VCFLIB = vcflib/tabixpp/tabix.o \
	vcflib/tabixpp/bgzf.o \
	vcflib/smithwaterman/SmithWatermanGotoh.o \
	vcflib/smithwaterman/disorder.c \
	vcflib/smithwaterman/LeftAlign.o \
	vcflib/smithwaterman/Repeats.o \
	vcflib/smithwaterman/IndelAllele.o \
	vcflib/src/split.o \
	vcflib/src/Variant.o

# profiling

profiling:
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -g" all

gprof:
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -pg" all

# libraries

# builds bamtools static lib, and copies into root

$(VCFLIB):
	cd vcflib && $(MAKE)

# arfer build

%.o: %.cpp
	$(CXX) -c $(*F).cpp -o $@ $(INCLUDES) $(LDFLAGS) $(CXXFLAGS)

$(BINS): $(BIN_SOURCES) $(OBJECTS) $(SOURCES) $(HEADERS) $(VCFLIB)
	$(CXX) -o $@ $(INCLUDES) $(OBJECTS) $(VCFLIB) $(LDFLAGS) $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(BINS) $(OBJECTS)
	cd vcflib && $(MAKE) clean

clean-arfer:
	rm -f $(BINS) $(OBJECTS)

.PHONY: clean all
