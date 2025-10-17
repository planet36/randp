# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

export LC_ALL := C

VERSION := $(shell git describe --tags --abbrev=0 2> /dev/null)
ifeq ($(VERSION),)
VERSION := X.Y
endif
VERSION_SPLIT := $(subst ., , $(VERSION))
VERSION_MAJOR := $(word 1, $(VERSION_SPLIT))
VERSION_MINOR := $(word 2, $(VERSION_SPLIT))

LIBNAME := randp
ANAME := lib$(LIBNAME).a
# "versionless" soname
SONAME_0 := lib$(LIBNAME).so
# soname
SONAME_1 := $(SONAME_0).$(VERSION_MAJOR)
# "realname"
SONAME_2 := $(SONAME_1).$(VERSION_MINOR)
SINGLE_HEADER := $(LIBNAME)-single.h

# paths
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
INCDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib
MANDIR ?= $(PREFIX)/share/man

DEPS = $(addsuffix .d,$(basename $(SRCS)))
OBJS = $(addsuffix .o,$(basename $(SRCS)))
BINS = $(basename $(SRCS))

# clang and clang++ not supported
CC = gcc
CXX = g++

CPPFLAGS = -MMD -MP
# _DEFAULT_SOURCE needed for getentropy, arc4random
CPPFLAGS += -D_DEFAULT_SOURCE

CFLAGS = -std=c23
CFLAGS += -pipe -Wall -Wextra -Wpedantic -Wfatal-errors
CFLAGS += -O3 -flto=auto -maes

CXXFLAGS = -std=c++20
CXXFLAGS += -pipe -Wall -Wextra -Wpedantic -Wfatal-errors
CXXFLAGS += -O3 -flto=auto -maes

#LDFLAGS +=

#LDLIBS +=

# default = rv
ARFLAGS = rscv
