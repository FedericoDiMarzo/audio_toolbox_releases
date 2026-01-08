#!/bin/bash

# Getting the sources paths
script_dir=$(realpath "$(dirname "$0")")
files=$("$script_dir/list_all_sources.sh")

# Applying the format
for file in $files; do
  echo "Formatting $file"
  clang-format -style=file -i "$file"
done
