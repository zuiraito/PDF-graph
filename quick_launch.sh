#!/bin/bash

# Change to the directory where the script is located
cd "$(dirname "$0")" || exit

# (cd Backend && ./clear_jsons.sh)
(cd Backend && ./run.sh)
(cd Frontend && ./run.sh)
