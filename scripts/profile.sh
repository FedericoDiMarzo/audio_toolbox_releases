#!/bin/bash

# Check if at least one argument is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <executable> [args...]"
    exit 1
fi

# Assign the first argument to a variable and shift the arguments
executable=$1
shift

# Remove existing callgrind output files
rm callgrind.out.*

# Run valgrind with callgrind and time the execution
time valgrind 	--tool=callgrind \
		--dump-instr=yes \
		--instr-atstart=no \
		$executable "$@"
