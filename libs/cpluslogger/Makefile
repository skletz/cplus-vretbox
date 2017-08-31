# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	Makefile for cpluslogger (c++ header only logger)
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

PROJECT=cpluslogger
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

demo: Logger.o demo.o
	$(CXX) $(CXXFLAGS) $(BUILD)/$(EXT)/demo.o $(BUILD)/$(EXT)/Logger.o -o $(BUILD)/$(BIN)/demo$(PROJECT).$(VERSION)

Logger.o: $(SRC)/Logger.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $(SRC)/Logger.cpp -o $(BUILD)/$(EXT)/Logger.o

demo.o: $(SRC)/demo.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $(SRC)/demo.cpp -o $(BUILD)/$(EXT)/demo.o

static: $(BUILD)/$(EXT)/Logger.o
	ar -rv $(BUILD)/$(LIB)/lib$(TARGETSTATIC) $(BUILD)/$(EXT)/Logger.o

shared: Logger.o
	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -Wall -shared -Wl,-soname, $(BUILD)/$(EXT)/Logger.o -o $(BUILD)/$(LIB)/lib$(TARGETSHARED)

clean:
	rm -rf $(BUILD)
