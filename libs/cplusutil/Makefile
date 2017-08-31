# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	Makefile for cplusutil (c++ collection)
# @author skletz
# @version 1.1, 08/06/17 change out directory
# @version 1.0 01/05/17
# -----------------------------------------------------------------------------
# CMD Arguments:	os=win,linux (sets the operating system, default=linux)
# -----------------------------------------------------------------------------
# @TODO: Make for Windows (currently the option is only considered)
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Default command line arguments
os = linux

PROJECT=cplusutil
VERSION=1.0

CXX=g++
CXXFLAGS=-std=c++11 -m64

# Output directory
BUILD =	builds
BIN =	linux/bin
LIB =	linux/lib
EXT =	linux/ext

SRC= $(PROJECT)/src
TARGETSHARED = $(PROJECT).so.$(VERSION)
TARGETSTATIC = $(PROJECT).$(VERSION).a

SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(BUILD)/$(EXT)/%.o,$(SOURCES))

LDLIBSOPTIONS=-L/usr/local/lib -lboost_filesystem -lboost_system

# operating system can be changed via command line argument
ifeq ($(os),win)
	BIN := win/bin
	LIB := win/lib
	EXT := win/ext
endif

.PHONY: all

all: directories demo shared static

directories:
	mkdir -p $(BUILD)/$(BIN)
	mkdir -p $(BUILD)/$(LIB)
	mkdir -p $(BUILD)/$(EXT)

demo: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILD)/$(BIN)/demo$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

shared: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -Wall -shared -Wl,-soname, $(BUILD)/$(EXT)/*.o -o $(BUILD)/$(LIB)/lib$(TARGETSHARED)

$(BUILD)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

static: $(OBJECTS)
	ar -rv $(BUILD)/$(LIB)/lib$(TARGETSTATIC) $(OBJECTS)


clean:
	rm -rf $(BUILD)
