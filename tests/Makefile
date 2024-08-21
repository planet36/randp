# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

include ../config.mk

# libarp is dynamically linked with the executables in this folder.

export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):..

SRCS := $(wildcard test-*.c)

LDFLAGS += -L..

LDLIBS += -l$(LIBNAME)

DATETIME := $(shell date -u +'%Y%m%dT%H%M%S')

OUTPUT_DIR := results

all: $(BINS)

$(BINS): ../$(SONAME_0)

# The built-in recipe for the implicit rule uses $^ instead of $<
%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

fork: test-fork
	./$<
	./$< arc4random
	@test $(shell ./$<            | awk '{print $3}' | sort | uniq | wc --lines) -eq 2
	@test $(shell ./$< arc4random | awk '{print $3}' | sort | uniq | wc --lines) -eq 2

generate: test-generate
	./$<
	./$< arc4random

profile: test-profile | $(OUTPUT_DIR)
	valgrind --tool=callgrind \
		--callgrind-out-file=$(OUTPUT_DIR)/callgrind.out.$(DATETIME) ./$<
	@if [[ -n "$$DISPLAY" ]]; then kcachegrind $(OUTPUT_DIR)/callgrind.out.$(DATETIME); fi

run-PractRand: test-dump | $(OUTPUT_DIR)
	@echo "WARNING: Each test takes about 60 minutes on my fast (in AD 2024) computer"
	./$<            | RNG_test stdin -tf 2 -te 1 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.arp.$(DATETIME).txt
	./$< arc4random | RNG_test stdin -tf 2 -te 1 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.arc4random.$(DATETIME).txt

run-rngtest: test-dump | $(OUTPUT_DIR)
	./$<            | rngtest -c 500000 |& tee $(OUTPUT_DIR)/rngtest.arp.$(DATETIME).txt
	./$< arc4random | rngtest -c 500000 |& tee $(OUTPUT_DIR)/rngtest.arc4random.$(DATETIME).txt

$(OUTPUT_DIR):
	mkdir --verbose --parents -- $@

clean:
	@$(RM) --verbose -- $(DEPS) $(BINS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all fork generate profile run-PractRand run-rngtest clean

-include $(DEPS)
