# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

export LC_ALL := C

# libarp is dynamically linked with the executables in this folder.

export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):..

LIBNAME := arp

C_SRCS := $(wildcard *.c)
DEPS := $(C_SRCS:.c=.d)

BINS = test-dump test-fork test-generate

CPPFLAGS = -MMD -MP
CPPFLAGS += -I..
# _DEFAULT_SOURCE needed for getentropy, arc4random
CPPFLAGS += -D_DEFAULT_SOURCE

CFLAGS = -std=c23
CFLAGS += -pipe -Wall -Wextra -Wpedantic -Wfatal-errors
CFLAGS += -O2 -flto=auto -maes

LDFLAGS += -L..

LDLIBS += -l$(LIBNAME)

DATETIME := $(shell date -u +'%Y%m%dT%H%M%S')

OUTPUT_DIR := results

all: $(BINS)

# The built-in recipe for the implicit rule uses $^ instead of $<
%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

dump: test-dump | $(OUTPUT_DIR)
	@echo "WARNING: Each test takes about 60 minutes on my fast (in AD 2024) computer"
	@./$<            | RNG_test stdin -tf 2 -te 1 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.arp.$(DATETIME).txt
	@./$< arc4random | RNG_test stdin -tf 2 -te 1 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.arc4random.$(DATETIME).txt

fork: test-fork
	@./$<
	@./$< arc4random
	@test $(shell ./$<            | awk '{print $3}' | sort | uniq | wc --lines) -eq 2
	@test $(shell ./$< arc4random | awk '{print $3}' | sort | uniq | wc --lines) -eq 2

generate: test-generate
	@./$<
	@./$< arc4random

$(OUTPUT_DIR):
	mkdir --verbose --parents -- $@

clean:
	@$(RM) --verbose -- $(DEPS) $(BINS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all dump fork generate clean

-include $(DEPS)
