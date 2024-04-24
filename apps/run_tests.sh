#!/bin/bash

# Directory containing your .x files
DIRECTORY='./'

# Loop through all .x files in the directory
for xfile in $DIRECTORY*.x; do
    # Skip specific files
    if [[ "$xfile" != *"/sem_prime.x" && "$xfile" != *"/sem_buffer.x" ]]; then
        echo "Running $xfile..."
        # Make sure the file is executable
        chmod +x "$xfile"
        # Execute the file
        ./"$xfile"
    else
        echo "Skipping $xfile..."
    fi
done
