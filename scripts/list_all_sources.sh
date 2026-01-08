#!/bin/bash

# Getting the sources paths
script_dir=$(realpath "$(dirname "$0")")
root_dir=$(realpath "$script_dir/..")
files=$(find "$root_dir/src" "$root_dir/tests" "$root_dir/tools" "$root_dir/inc" \
          -name "*.h" -o -name "*.cpp" -o -name "*.hpp" | \
          grep -v "audio_toolbox/build/")

# Printing the files
echo "$files"
