# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: MPL-2.0

include ../config.mk

# librandp is statically linked with the executables in this folder, but only
# benchmark-others uses it.  The others use the C++ implementation in randp.hpp.

#export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):..

SRCS = $(wildcard benchmark-*.cpp)

CXXFLAGS += -mrdrnd -mrdseed
CXXFLAGS += -march=x86-64-v3 -maes -mvaes # x86-64-v3 implies avx, avx2

#LDFLAGS += -L..

LDLIBS += $(shell pkg-config --libs benchmark)
#LDLIBS += -l$(LIBNAME)
LDLIBS += ../$(ANAME)

# Use 1 thread in all of the tests.
export NUM_THREADS := 1

# This should be an odd number for a simpler median.
BENCHMARK_REPS = 5

DATETIME := $(shell date -u +'%Y%m%dT%H%M%S')

OUTPUT_DIR = results/$(DATETIME)

all: $(BINS)

$(BINS): ../$(ANAME)

../$(ANAME):
	$(MAKE) -C ..

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
	--benchmark_out=$(OUTPUT_DIR)/$@.txt
bash process-results.bash $(OUTPUT_DIR)/$@.txt | tee $(OUTPUT_DIR)/$@.filtered.txt
endef

mutex: benchmark-mutex | $(OUTPUT_DIR)
	$(benchmark-recipe)

others: benchmark-others | $(OUTPUT_DIR)
	$(benchmark-recipe)

pool-size: benchmark-pool-size | $(OUTPUT_DIR)
	$(benchmark-recipe)

prng-params: benchmark-prng-params | $(OUTPUT_DIR)
	$(benchmark-recipe)

reseed-countdown: benchmark-reseed-countdown | $(OUTPUT_DIR)
	$(benchmark-recipe)

$(OUTPUT_DIR):
	mkdir --verbose --parents -- $@

clean:
	@$(RM) --verbose -- $(DEPS) $(BINS)

lint:
	-clang-tidy --quiet $(SRCS) -- $(CPPFLAGS) $(CXXFLAGS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all mutex others pool-size prng-params reseed-countdown clean lint

# https://www.gnu.org/software/make/manual/html_node/Special-Targets.html#index-removing-targets-on-failure
.DELETE_ON_ERROR:

-include $(DEPS)
