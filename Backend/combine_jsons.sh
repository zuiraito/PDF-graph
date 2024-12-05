#!/bin/bash

# Define the directory (change to your desired directory if necessary)
directory="JSONs/"

# Loop through each file in the directory
cat JSON_head_and_tail/head
echo
for file in "$directory"/*.json; do
  # Check if it's a regular file
  if [[ -f "$file" ]]; then
    sed '1,2d' "$file" | sed '$d' | sed '$d' | sed '$d'
    echo "	},"
  fi
done
cat JSON_head_and_tail/tail
echo
