#
#  Version 1.0
#
PROJECT=cplusutil
VERSION=1.0

OS=Linux
CXX=g++
CXXFLAGS=-std=c++11 -Wall

TARGET=build
BIN=bin
LIB=lib
EXT=ext

SRC= $(PROJECT)/src
TARGETSHARED = $(PROJECT).1.0.dll
TARGETSTATIC = $(PROJECT).$(VERSION).lib

SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(TARGET)/$(EXT)/%.o,$(SOURCES))

LDLIBSOPTIONS= -L"$(Boost_DIR)\stage\lib\vc140\x64" -lboost_filesystem -lboost_system

# all:
# 	# @echo $(SOURCES) "\n"
# 	# @echo "Object files:"
# 	# @echo $(OBJECTS)
#
# all: clean directories demo staticlib
all: directories demo
	#shared static

directories:
	echo Boost Dir: $(LDLIBSOPTIONS)
	mkdir -p $(TARGET)/$(BIN)
	mkdir -p $(TARGET)/$(LIB)
	mkdir -p $(TARGET)/$(EXT)

demo: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)/$(BIN)/demo$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

shared: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDLIBSOPTIONS) -shared, $(TARGET)/$(EXT)/*.o -o $(TARGET)/$(LIB)/lib$(TARGETSHARED)

$(TARGET)/$(EXT)/%.o: $(SRC)/%.cpp
	   $(CXX) $(CXXFLAGS) -c $< -o $@ -I"$(Boost_DIR)"

static: $(OBJECTS)
	ar -rv $(TARGET)/$(LIB)/lib$(TARGETSTATIC) $(OBJECTS)


clean:
	rm -rf build
