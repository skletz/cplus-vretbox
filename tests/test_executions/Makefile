# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	Makefile for vretbox (Video Retrieval Evaluation Toolbox)
#
# @author skletz
# @version 1.2, 24/08/17 remove obsolete dependencies
# @version 1.1, 19/06/17 bug fix make at all
# @version 1.0, 08/06/17
# -----------------------------------------------------------------------------
# CMD Arguments:	os=win,linux (sets the operating system, default=linux)
#									opencv=usr,opt, include (usr=/user/local/, opt=/opt/local/; default=usr)
# -----------------------------------------------------------------------------
# @TODO: Make for Windows (currently only option is considered)
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Default command line arguments
os = linux
opencv = usr

# Compiler, flags
CXX=g++
CXXFLAGS=-std=c++11 -m64

# Output directory
OUT = ../../builds
BUILD =	builds
BIN =	linux/bin
LIB =	linux/lib
EXT =	linux/ext

# Project settings
PROJECT=opencv_brisk
VERSION=1.0

SRC= $(PROJECT)/src
SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(OUT)/$(EXT)/%.o,$(SOURCES))

# Default settings for opencv installation directory
OPENCVDIR := /usr/local/
INCDIR = `pkg-config --cflags opencv`
LDLIBSOPTIONS_POST =`pkg-config --libs opencv`
LDLIBSOPTIONS_POST += -L/usr/local/share/OpenCV/3rdparty/lib

# operating system can be changed via command line argument
ifeq ($(os),win)
	BIN := win/bin
	LIB := win/lib
	EXT := win/ext
endif

# opencv installation dir can be changed via command line argument
ifeq ($(opencv),opt)
	OPENCVDIR := /opt/local/
	INCDIR = `pkg-config --cflags /opt/local/lib/pkgconfig/opencv.pc`
	LDLIBSOPTIONS_POST =`pkg-config --libs /opt/local/lib/pkgconfig/opencv.pc`
endif

# OpenCV, Boost Cplusutil, Cplulogger Directory, Header Files, Libraries
UTIL="../../libs/cplusutil"
LOGGER="../../libs/cpluslogger"
#Dynamic Features
DEFUSE="../../libs/defuse"

# contains all compiled libraries
LIBS="$(OUT)/$(LIB)"

LDLIBSOPTIONS = $(LIBS)/libcpluslogger.1.0.a
LDLIBSOPTIONS += $(LIBS)/libdefuse.2.0.a
LDLIBSOPTIONS += $(LIBS)/libcplusutil.1.0.a

LDLIBSOPTIONS += -lboost_system -lboost_filesystem -lboost_serialization -lboost_program_options -lboost_thread
#IMPORTANT: Link sequence! - OpenCV libraries have to be added at the end
LDLIBSOPTIONS += $(LDLIBSOPTIONS_POST)

INCDIR+= -I$(LOGGER)/cpluslogger/include
INCDIR+= -I$(UTIL)/cplusutil/include
INCDIR+= -I$(DEFUSE)/defuse/include

UTIL="../../libs/cplusutil"
LOGGER="../../libs/cpluslogger"
DEFUSEDIR="../../libs/defuse"

.PHONY: all

all: clean directories libraries prog

directories:
	@echo "==============================================================================" ;
	@echo "Making Directories"
	@echo "==============================================================================" ;
	mkdir -p $(OUT)/$(BIN)
	mkdir -p $(OUT)/$(LIB)
	mkdir -p $(OUT)/$(EXT)

prog: obj
	@echo "==============================================================================" ;
	@echo "Making Program"
	@echo "==============================================================================" ;

obj: $(OBJECTS)
			$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(OUT)/$(BIN)/prog$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

$(OUT)/$(EXT)/%.o: $(SRC)/%.cpp
		$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ $(INCDIR)

# make dependencies
libraries: logger util defuse

logger:
	@echo "==============================================================================" ;
	@echo "Making CPLUSLogger: " $(LOGGER)
	@echo "==============================================================================" ;
	$(MAKE) -C $(LOGGER)/ clean
	$(MAKE) -C $(LOGGER)/ all
	@echo "==============================================================================" ;
	@echo "Copying CPLUSLogger: " $(LOGGER)
	@echo "==============================================================================" ;
	cp -f $(LOGGER)/$(BUILD)/$(BIN)/* ./$(OUT)/$(BIN);
	cp -f $(LOGGER)/$(BUILD)/$(LIB)/* ./$(OUT)/$(LIB);
	cp -f $(LOGGER)/$(BUILD)/$(EXT)/* ./$(OUT)/$(EXT);

util:
	@echo "==============================================================================" ;
	@echo "Making CPLUSUtil: " $(UTIL)
	@echo "==============================================================================" ;
	$(MAKE) -C $(UTIL)/ clean
	$(MAKE) -C $(UTIL)/ all
	@echo "==============================================================================" ;
	@echo "Copying CPLUSUtil: " $(UTIL)
	@echo "==============================================================================" ;
	cp -f $(UTIL)/$(BUILD)/$(BIN)/* ./$(OUT)/$(BIN);
	cp -f $(UTIL)/$(BUILD)/$(LIB)/* ./$(OUT)/$(LIB);
	cp -f $(UTIL)/$(BUILD)/$(EXT)/* ./$(OUT)/$(EXT);

defuse:
	@echo "==============================================================================" ;
	@echo "Making DeFUSE: " $(DEFUSEDIR)
	@echo "==============================================================================" ;
	$(MAKE) -C $(DEFUSEDIR)/ os=$(os) opencv=$(opencv) clean
	$(MAKE) -C $(DEFUSEDIR)/ os=$(os) opencv=$(opencv) all
	@echo "==============================================================================" ;
	@echo "Copying DeFUSE: " $(DEFUSEDIR)
	@echo "==============================================================================" ;
	cp -f $(DEFUSEDIR)/$(BUILD)/$(BIN)/* ./$(OUT)/$(BIN);
	cp -f $(DEFUSEDIR)/$(BUILD)/$(LIB)/* ./$(OUT)/$(LIB);
	cp -f $(DEFUSEDIR)/$(BUILD)/$(EXT)/* ./$(OUT)/$(EXT);

clean:
	rm -rf $(OUT)

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "OpenCV Header Include Directory: " $(INCDIR) "\n"
	@echo "OpenCV Linking: " $(LDLIBSOPTIONS) "\n"
