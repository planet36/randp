# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

export LC_ALL := C

# libarp is statically linked with the executables in this folder.

#export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):..

LIBNAME := arp
ANAME := lib$(LIBNAME).a

SRCS := $(wildcard benchmark-*.cpp)
DEPS = $(addsuffix .d,$(basename $(SRCS)))
OBJS = $(addsuffix .o,$(basename $(SRCS)))
BINS = $(basename $(SRCS))

CPPFLAGS = -MMD -MP
# _DEFAULT_SOURCE needed for getentropy, arc4random
CPPFLAGS += -D_DEFAULT_SOURCE

CXXFLAGS = -std=c++23
CXXFLAGS += -pipe -Wall -Wextra -Wpedantic -Wfatal-errors
CXXFLAGS += -O2 -flto=auto -maes
CXXFLAGS += -mrdrnd -mrdseed

#LDFLAGS += -L..

LDLIBS += `pkg-config --libs benchmark`
#LDLIBS += -l$(LIBNAME)
LDLIBS += ../$(ANAME)

# Use N-1 threads in the benchmark
export NUM_THREADS := $(shell nproc --ignore 1)

# Should be an odd number for simpler median
BENCHMARK_REPS := 11

DATETIME := $(shell date -u +'%Y%m%dT%H%M%S')

OUTPUT_DIR := results

all: $(BINS)

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
	$(benchmark-recipe)

num-blocks: benchmark-num-blocks | $(OUTPUT_DIR)
	$(benchmark-recipe)

others: benchmark-others | $(OUTPUT_DIR)
	$(benchmark-recipe)

reseed-countdown: benchmark-reseed-countdown | $(OUTPUT_DIR)
	$(benchmark-recipe)

$(OUTPUT_DIR):
	mkdir --verbose --parents -- $@

clean:
	@$(RM) --verbose -- $(DEPS) $(BINS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all mutex num-blocks others reseed-countdown clean

-include $(DEPS)
