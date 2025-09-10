# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

include ../config.mk

# librandp is dynamically linked with the executables (except test-single) in
# this folder.

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

# This executable does not link with librandp.
test-single: test-single.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

fork: test-fork
	./$<
	./$< arc4random
	./$< mrand48
	@test $(shell ./$<            | awk '{print $$3}' | sort | uniq | wc --lines) -eq 2
	@test $(shell ./$< arc4random | awk '{print $$3}' | sort | uniq | wc --lines) -eq 2
	-@test $(shell ./$< mrand48    | awk '{print $$3}' | sort | uniq | wc --lines) -eq 2

generate: test-generate
	./$<
	./$< arc4random
	./$< mrand48
	openssl rand -hex 60 | tr '[:lower:]' '[:upper:]'

profile: test-profile | $(OUTPUT_DIR)
	valgrind --tool=callgrind --enable-debuginfod=no \
		--callgrind-out-file=$(OUTPUT_DIR)/callgrind.out.$(DATETIME) ./$<

profile-kgui: profile
	kcachegrind $(OUTPUT_DIR)/callgrind.out.$(DATETIME)

profile-qgui: profile
	qcachegrind $(OUTPUT_DIR)/callgrind.out.$(DATETIME)

# Takes about 3 hours
run-PractRand: test-dump | $(OUTPUT_DIR)
	@echo "WARNING: Each test takes about 60 minutes on my fast (in AD 2024) computer"
	./$<             | RNG_test stdin -tf 2 -te 0 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.randp.$(DATETIME).txt
	./$< arc4random  | RNG_test stdin -tf 2 -te 0 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.arc4random.$(DATETIME).txt
	./$< mrand48     | RNG_test stdin -tf 2 -te 0 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.mrand48.$(DATETIME).txt
	openssl rand max | RNG_test stdin -tf 2 -te 0 -tlmin 256MB -tlmax 512GB -multithreaded | tee $(OUTPUT_DIR)/RNG_test.openssl-rand.$(DATETIME).txt

# Takes about 5 minutes
run-rngtest: test-dump | $(OUTPUT_DIR)
	./$<             | rngtest -c 1000000 |& tee $(OUTPUT_DIR)/rngtest.randp.$(DATETIME).txt
	./$< arc4random  | rngtest -c 1000000 |& tee $(OUTPUT_DIR)/rngtest.arc4random.$(DATETIME).txt
	./$< mrand48     | rngtest -c 1000000 |& tee $(OUTPUT_DIR)/rngtest.mrand48.$(DATETIME).txt
	openssl rand max | rngtest -c 1000000 |& tee $(OUTPUT_DIR)/rngtest.openssl-rand.$(DATETIME).txt

single: test-single
	./$<

$(OUTPUT_DIR):
	mkdir --verbose --parents -- $@

clean:
	@$(RM) --verbose -- $(DEPS) $(BINS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all fork generate profile profile-kgui profile-qgui run-PractRand run-rngtest single clean

-include $(DEPS)
