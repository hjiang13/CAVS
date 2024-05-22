import os
import json

# Directory containing the sliced C++ files
input_directory = 'sliced_files'
output_file = 'sliced_files.jsonl'
mapping_file = 'label_mapping.json'

# Initialize an empty list to store all file data
file_data = []
label_mapping = {}
label_counter = 0.0

# Iterate over all C++ files in the input directory
for root, dirs, files in os.walk(input_directory):
    for file in files:
        if file.endswith('.cpp'):
            file_path = os.path.join(root, file)
            with open(file_path, 'r') as f:
                source_code = f.read()
            
            # Assign a numerical label
            label_mapping[label_counter] = file
            file_entry = {
                "code": source_code,
                "label": label_counter
            }
            file_data.append(file_entry)
            label_counter += 1

# Write the file data to a JSONL file
with open(output_file, 'w') as jsonl_file:
    for entry in file_data:
        jsonl_file.write(json.dumps(entry) + '\n')

# Save the label mapping to a JSON file
with open(mapping_file, 'w') as json_file:
    json.dump(label_mapping, json_file, indent=4)

print(f'Organized code files saved to {output_file}')
print(f'Label mapping saved to {mapping_file}')