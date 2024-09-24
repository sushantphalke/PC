import random

# Configuration
num_lines = 10000  # Number of lines to generate
line_length = 50    # Length of each line
pattern = "pattern we are looking for"
pattern_frequency = 0.1  # Probability of including the pattern in a line

# Generate random text
def generate_line():
    if random.random() < pattern_frequency:
        return f"This line contains the {pattern}."
    else:
        return ''.join(random.choices('abcdefghijklmnopqrstuvwxyz ', k=line_length))

# Write to the file
with open('input_large.txt', 'w') as f:
    for _ in range(num_lines):
        f.write(generate_line() + "\n")

print(f"Generated 'input_large.txt' with {num_lines} lines.")
