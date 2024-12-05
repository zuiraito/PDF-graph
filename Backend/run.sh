#!/bin/bash

# Set the paths
input_dir="../Data/"
output_dir="JSONs/"
ignore_words="ignore_words"

# Loop through all PDF files in the input directory
for input_file in "$input_dir"/*.pdf; do
    # Extract the file name without the extension
    filename=$(basename -- "$input_file")
    filename_noext="${filename%.*}"
    
    # Set the output file path
    output_file="$output_dir/$filename_noext.json"
    
    # Check if the output file already exists
    if [ -f "$output_file" ]; then
        echo "Output file '$output_file' already exists. Skipping..."
    else
        # Execute the script.sh with the correct arguments
        echo "Processing '$input_file'..."
        ./pdf_titles "$input_file" "$ignore_words" "$output_file" 
    fi
done

./combine_jsons.sh > combined.json
./move_to_frontend.sh
