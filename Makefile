# Optimized Haiku Build Script
SHELL := /bin/bash

PACKAGE_DIR := build/package
NAME = SleepWalker
# The one place a release changes is the VERSION file: the program (and its update check), the package and the
# resource all take it from there.
VERSION := $(shell sed -n 's/.*\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*/\1/p' VERSION | head -1)
V_MAJOR := $(word 1,$(subst ., ,$(VERSION)))
V_MIDDLE := $(word 2,$(subst ., ,$(VERSION)))
V_MINOR := $(word 3,$(subst ., ,$(VERSION)))
DEFINES += -DAPP_LOCAL_VERSION=\"$(VERSION)\"


UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M), BePC)
CXX = g++-x86 
CC = gcc-x86
MAKE := setarch x86 $(MAKE)
ARCH = x86_gcc2
SIMD_FLAGS := -O2
is32bit = _x86
INCLUDE = -L/boot/system/lib/x86 
else
CXX = g++
CC = gcc
ARCH = x86_64
SIMD_FLAGS := -O3
INCLUDE = -L/boot/system/lib
endif


BUILD_FLAGS = $(SIMD_FLAGS) 
LD_OPTIMIZE = -Wl,--gc-sections


EXTRA_LIBS = 
HAIKU_LIBS = -lbe -lcurl

ifeq ($(CHK_GL), ON)
    DEFINES += -DCHK_GL
endif

.PHONY: build clean

all: build

build: 
	@echo "--------- Building $(NAME) $(ARCH) ---------"

	$(CXX) -o $(NAME) $(DEFINES) $(INCLUDE) $(BUILD_FLAGS) $(HAIKU_LIBS) $(LD_OPTIMIZE) $(NAME).cpp


package: all
	@[ -n "$(PACKAGE_DIR)" ] || { echo "PACKAGE_DIR is undefined"; exit 1; }
	rm -rf "./$(PACKAGE_DIR)"
	mkdir -p $(PACKAGE_DIR)
	sed -e 's/$$(NAME)/$(NAME)/g' -e 's/$$(VERSION)/$(VERSION)/g' -e 's/$$(ARCH)/$(ARCH)/'  -e 's/$$(is32bit)/$(is32bit)/g' -e 's/$$(YEAR)/$(shell date +%Y)/' $(NAME).tpl > $(PACKAGE_DIR)/.PackageInfo
	mkdir -p $(PACKAGE_DIR)/apps
	mkdir -p $(PACKAGE_DIR)/data/$(NAME)
	mkdir -p $(PACKAGE_DIR)/bin
	mkdir -p $(PACKAGE_DIR)/data/deskbar/menu/Applications
	sed -e 's/@V_MAJOR@/$(V_MAJOR)/' -e 's/@V_MIDDLE@/$(V_MIDDLE)/' -e 's/@V_MINOR@/$(V_MINOR)/' -e 's/@VERSION@/$(VERSION)/' $(NAME).rdef > $(NAME).version.rdef
	rc -o $(NAME).rsrc $(NAME).version.rdef
	xres -o $(NAME) $(NAME).rsrc  
	mimeset -f $(NAME)
	cp $(NAME) $(PACKAGE_DIR)/apps/$(NAME)
	cp $(NAME)_64px.png $(PACKAGE_DIR)/data/$(NAME)/
	ln -s ../apps/$(NAME) $(PACKAGE_DIR)/bin/$(NAME)
	ln -s ../../../../apps/$(NAME) $(PACKAGE_DIR)/data/deskbar/menu/Applications/$(NAME)
	package create -C $(PACKAGE_DIR) $(NAME)-$(VERSION)-1-$(ARCH).hpkg


clean:
	@rm -f $(NAME) $(NAME).rsrc *.version.rdef *.hpkg
	@rm -rf build
	@echo "Cleanup complete."



