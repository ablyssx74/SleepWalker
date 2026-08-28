# Optimized Haiku Build Script
SHELL := /bin/bash

PACKAGE_DIR := build/package
NAME = SleepWalker
VERSION = 1.0.1


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
HAIKU_LIBS = -lbe

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
	rc -o $(NAME).rsrc $(NAME).rdef 
	xres -o $(NAME) $(NAME).rsrc  
	mimeset -f $(NAME)
	cp $(NAME) $(PACKAGE_DIR)/apps/$(NAME)
	cp $(NAME)_64px.png $(PACKAGE_DIR)/data/$(NAME)/
	ln -s ../apps/$(NAME) $(PACKAGE_DIR)/bin/$(NAME)
	ln -s ../../../../apps/$(NAME) $(PACKAGE_DIR)/data/deskbar/menu/Applications/$(NAME)
	package create -C $(PACKAGE_DIR) $(NAME)-$(VERSION)-1-$(ARCH).hpkg


clean:
	@rm -f $(NAME) $(NAME).rsrc *.hpkg
	@rm -rf build
	@echo "Cleanup complete."



