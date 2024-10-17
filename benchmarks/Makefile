# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

include ../config.mk

# librandp is statically linked with one of the executables in this folder.

#export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):..

SRCS := $(wildcard benchmark-*.cpp)

CXXFLAGS += -mrdrnd -mrdseed

#LDFLAGS += -L..

LDLIBS += `pkg-config --libs benchmark`
#LDLIBS += -l$(LIBNAME)
LDLIBS += ../$(ANAME)

# Should be an odd number for simpler median
BENCHMARK_REPS := 3

DATETIME := $(shell date -u +'%Y%m%dT%H%M%S')

OUTPUT_DIR := results

all: $(BINS)

$(BINS): ../$(ANAME)

# The built-in recipe for the implicit rule uses $^ instead of $<
%: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

# https://www.gnu.org/software/make/manual/html_node/Canned-Recipes.html
define benchmark-recipe =
./$< \
	--benchmark_enable_random_interleaving=true \
	--benchmark_repetitions=$(BENCHMARK_REPS) \
	--benchmark_report_aggregates_only=true \
	--benchmark_out_format=console \
	--benchmark_out=$(OUTPUT_DIR)/$@.$(DATETIME).txt
bash process-results.bash $(OUTPUT_DIR)/$@.$(DATETIME).txt | tee $(OUTPUT_DIR)/$@.$(DATETIME).filtered.txt
endef

mutex: benchmark-mutex | $(OUTPUT_DIR)
	NUM_THREADS=$(shell nproc --ignore 1) $(benchmark-recipe)

num-blocks: benchmark-num-blocks | $(OUTPUT_DIR)
	NUM_THREADS=$(shell nproc --ignore 1) $(benchmark-recipe)

others: benchmark-others | $(OUTPUT_DIR)
	NUM_THREADS=1 $(benchmark-recipe)

prng-params: benchmark-prng-params | $(OUTPUT_DIR)
	NUM_THREADS=$(shell nproc --ignore 1) $(benchmark-recipe)

reseed-countdown: benchmark-reseed-countdown | $(OUTPUT_DIR)
	NUM_THREADS=$(shell nproc --ignore 1) $(benchmark-recipe)

$(OUTPUT_DIR):
	mkdir --verbose --parents -- $@

clean:
	@$(RM) --verbose -- $(DEPS) $(BINS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all mutex num-blocks others prng-params reseed-countdown clean

-include $(DEPS)
