#!/bin/bash
set -euo pipefail

DIRECTORY=${1:-../include}

# Function to format files using clang-format
format_files() {
    local dir=$1
    find "$dir" -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print0 | while IFS= read -r -d '' file; do
        clang-format -i "$file"
        echo "Formatted $file"
    done
}

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "clang-format could not be found. Please install it and try again."
    exit 1
fi

# Run the format_files function on the specified directory
format_files "$DIRECTORY"
