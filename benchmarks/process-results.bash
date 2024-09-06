# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

# shellcheck disable=SC2043

export LC_ALL=C

SCRIPT_NAME="$(basename -- "${BASH_SOURCE[0]}")"
#SCRIPT_DIR="$(dirname -- "${BASH_SOURCE[0]}")"

function program_exists
{
    # https://pubs.opengroup.org/onlinepubs/9699919799/utilities/command.html
    command -v "${@}" > /dev/null
}

function print_usage
{
    cat <<EOT 1>&2
Usage: bash $SCRIPT_NAME BENCH_RESULTS_FILE

"BENCH_RESULTS_FILE" is the product of one of the benchmarks make targets.
EOT
}

for PROGRAM in grep sed awk sort column
do
    if ! program_exists "$PROGRAM"
    then
        printf 'Error: %q not found\n' "$PROGRAM" 1>&2
        #printf 'Try "%q -h" for more information.\n' "$SCRIPT_NAME" 1>&2
        print_usage
        exit 1
    fi
done

if (($# < 1))
then
    print_usage
    exit 1
fi

declare -r BENCH_RESULTS_FILE="$1"

if [[ ! -f "$BENCH_RESULTS_FILE" ]]
then
    printf 'Error: %q does not exist\n' "$BENCH_RESULTS_FILE" 1>&2
    print_usage
    exit 1
fi

# {{{ System and library version info
uname -srm
grep '^model name' /proc/cpuinfo | sort -u | awk -F': ' '{print $2}'
ldd --version | head -n 1
gcc --version | head -n 1
find /usr/lib -maxdepth 1 -type f -name 'libbenchmark.so*' -printf '%P\n'
find .. -maxdepth 1 -type f -name 'librandp.so*' -printf '%P\n'
echo
# }}}

echo "# Median time per call"
echo
for I in 8 16 32 64
do
    PATTERN="^rand_uint:${I}b:"
    if grep -q "$PATTERN" "$BENCH_RESULTS_FILE"
    then
        grep "$PATTERN" "$BENCH_RESULTS_FILE" | grep median | sed -r -e 's|(/threads:[0-9]+)?_median||' | awk '{print $1, $4, $5}' | sort -n -k 2 | column --table --table-right 2 || exit
        echo
    fi
done

echo "# Median time per call"
echo
for I in 32
do
    PATTERN="^rand_lt-one:${I}b:"
    if grep -q "$PATTERN" "$BENCH_RESULTS_FILE"
    then
        grep "$PATTERN" "$BENCH_RESULTS_FILE" | grep median | sed -r -e 's|(/threads:[0-9]+)?_median||' | awk '{print $1, $4, $5}' | sort -n -k 2 | column --table --table-right 2 || exit
        echo
    fi
done

echo "# Median calls per second"
echo
for I in 32
do
    PATTERN="^rand_lt-many:${I}b:"
    if grep -q "$PATTERN" "$BENCH_RESULTS_FILE"
    then
        grep "$PATTERN" "$BENCH_RESULTS_FILE" | grep median | sed -r -e 's|(/threads:[0-9]+)?_median||' | awk '{print $1, $7}' | sed -E -e 's/items_per_second=//' | sort -h -k 2 -r | column --table || exit
        echo
    fi
done

echo "# Median bytes per second copied to buffer (of size bytes)"
echo
for I in 1 2 4 8 16 32 64 128 256
do
    PATTERN="^rand_buf:${I}B:"
    if grep -q "$PATTERN" "$BENCH_RESULTS_FILE"
    then
        grep "$PATTERN" "$BENCH_RESULTS_FILE" | grep median | sed -r -e 's|(/threads:[0-9]+)?_median||' | awk '{print $1, $7}' | sed -E -e 's/bytes_per_second=//' | sort -h -k 2 -r | column --table || exit
        echo
    fi
done

echo "# Median bytes per second copied to buffer (of size pages)"
echo
for I in 1 2 3 4 5 6 7 8
do
    PATTERN="^rand_buf:${I}pg:"
    if grep -q "$PATTERN" "$BENCH_RESULTS_FILE"
    then
        grep "$PATTERN" "$BENCH_RESULTS_FILE" | grep median | sed -r -e 's|(/threads:[0-9]+)?_median||' | awk '{print $1, $7}' | sed -E -e 's/bytes_per_second=//' | sort -h -k 2 -r | column --table || exit
        echo
    fi
done

echo "# Median time to generate 4 GiB"
echo
PATTERN="^rand_buf_4GiB:"
if grep -q "$PATTERN" "$BENCH_RESULTS_FILE"
then
	grep "$PATTERN" "$BENCH_RESULTS_FILE" | grep median | sed -r -e 's|(/threads:[0-9]+)?_median||' | awk '{print $1, $4, $5}' | sort -n -k 2 | column --table --table-right 2 || exit
	echo
fi
