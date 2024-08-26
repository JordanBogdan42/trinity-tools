
# Makefile for trinity-tools
#
#
# 
#By: Oscar F Romero Matamala
#Modified by: Jordan Bogdan


BIN_DIR := .
SRC_DIR := $(BIN_DIR)/src
INC_DIR := $(BIN_DIR)/include
OBJ_DIR := $(BIN_DIR)/obj
EXE := $(BIN_DIR)/test
DICT_DIR := ${EXACT_DIR}/dict

ARCH := $(shell uname)

# linux flags
ifeq ($(ARCH),Linux)
CXX           = g++ 
CXXFLAGS      =  -g -O3 -Wall -ggdb3 -fPIC -fno-strict-aliasing  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE_SOURCE -D_LARGEFILE64_SOURCE
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -shared
endif

# Apple OS X flags
ifeq ($(ARCH),Darwin)
CXX           = g++ 
CXXFLAGS      = -g -O3 -Wall -fPIC  -fno-strict-aliasing
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

OutPutOpt     = -o

PREFIX		= /usr/local
# Root

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTLDFLAGS  := $(shell root-config --ldflags)
ROOTLIBDIR   := $(shell root-config --libdir)

CLLFLAGS += $(ROOTLDFLAGS)

CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTGLIBS)
LIBS         += -lMinuit -L./
LIBS		+= -L$(DICT_DIR) -L$(INC_DIR)


INCLUDEFLAGS  = -I$(INC_DIR) -I$(DICT_DIR)/include -I./
CXXFLAGS     += $(INCLUDEFLAGS)

ALLFLAGS = $(CXXFLAGS) $(CPPFLAGS) -Wall

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CP  := cp

#Recipe

.PHONY: all
all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR) 
	$(LD) $(OBJ) $(CXXFLAGS) $(LIBS) $(LDFLAGS) $(OutPutOpt) $@ -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -lExACT
	@echo "$@ Compilation done"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(LD) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/$(OBJ_DIR):
		mkdir -p $@

-include $(OBJ:.o=.d)

.SUFFIXES: .o

clean:
	-rm -rv $(OBJ_DIR)
	-rm -rv $(EXE)
