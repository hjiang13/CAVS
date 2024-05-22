import os
import json

# Directory containing the sliced C++ files
input_directory = 'sliced_files'
output_file = 'sliced_files.jsonl'

# Function to read the content of a file
def read_file(file_path):
    with open(file_path, 'r') as file:
        return file.read()

# Initialize an empty list to store all file data
file_data = []

# Iterate over all C++ files in the input directory
for root, dirs, files in os.walk(input_directory):
    for file in files:
        if file.endswith('.cpp'):
            file_path = os.path.join(root, file)
            source_code = read_file(file_path)
            file_entry = {
                "code": source_code,
                "label": file
            }
            file_data.append(file_entry)

# Write the file data to a JSONL file
with open(output_file, 'w') as jsonl_file:
    for entry in file_data:
        jsonl_file.write(json.dumps(entry) + '\n')

print(f'Organized code files saved to {output_file}')