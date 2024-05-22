import re
import os

# Directory containing the C++ source files
input_directory = 'Benchmarks'
output_directory = 'sliced_files'

# Ensure output directory exists
os.makedirs(output_directory, exist_ok=True)

# Regular expression patterns for different types of loops
for_pattern = re.compile(r'for\s*\(.*?\)\s*{')
while_pattern = re.compile(r'while\s*\(.*?\)\s*{')
do_while_pattern = re.compile(r'do\s*{')

def extract_body(source, start_pos):
    stack = []
    end_pos = start_pos
    for i in range(start_pos, len(source)):
        if source[i] == '{':
            stack.append('{')
        elif source[i] == '}':
            stack.pop()
            if not stack:
                end_pos = i
                break
    return source[start_pos:end_pos+1], end_pos + 1

def extract_loops(source_code, pattern):
    loops = []
    for match in pattern.finditer(source_code):
        start_pos = match.end() - 1
        loop_body, end_pos = extract_body(source_code, start_pos)
        loop_line_number = source_code.count('\n', 0, match.start()) + 1
        loops.append((match.start(), match.end(), loop_body, end_pos, loop_line_number))
    return loops

def process_file(file_path):
    with open(file_path, 'r') as file:
        source_code = file.read()

    # Extract loop bodies
    for_loops = extract_loops(source_code, for_pattern)
    while_loops = extract_loops(source_code, while_pattern)
    do_while_loops = extract_loops(source_code, do_while_pattern)

    # Combine all loops
    all_loops = sorted(for_loops + while_loops + do_while_loops, key=lambda x: x[0])

    # Function to delete a loop from the source code
    def delete_loop(source, start, end):
        return source[:start] + source[end:]

    # Get the base name of the file for naming output files
    base_name = os.path.splitext(os.path.basename(file_path))[0]

    sliced_files = []

    # Delete each loop one by one, save the result
    for loop in all_loops:
        modified_source = delete_loop(source_code, loop[0], loop[3])
        output_filename = os.path.join(output_directory, f'{base_name}_modified_line_{loop[4]}.cpp')
        with open(output_filename, 'w') as output_file:
            output_file.write(modified_source)
        sliced_files.append(output_filename)
    
    return sliced_files

# Iterate over all C++ files in the input directory
for root, dirs, files in os.walk(input_directory):
    for file in files:
        if file.endswith('.cpp'):
            file_path = os.path.join(root, file)
            process_file(file_path)

print('Code slicing completed.')
