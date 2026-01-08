#!/bin/bash

# Getting the sources paths
script_dir=$(realpath "$(dirname "$0")")
root_dir=$(realpath "$script_dir/..")
files=$(find "$root_dir" -name "*.h" -o -name "*.cpp" -o -name "*.hpp" |
  grep -v "audio_toolbox/libs/" |
  grep -v "audio_toolbox/build/")

# Checking the format
wrong_format=0
for file in $files; do
  echo "Checking $file"
  output=$(clang-format -style=file "$file")
  original=$(cat "$file")
  if [ "$output" != "$original" ]; then
    echo "Wrong format in $file"
    wrong_format=1
  fi
done


[[ $wrong_format -eq 0 ]] && echo "Format check passed" || exit 1