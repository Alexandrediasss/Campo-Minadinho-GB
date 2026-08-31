import re

with open('sprites.h', 'r') as f:
    content = f.read()

def swap_bytes(match, name):
    hex_list = match.group(1).replace(' ', '').replace('\n', '').split(',')
    hex_list = [h for h in hex_list if h]
    
    new_hex = []
    for i in range(0, len(hex_list), 2):
        if i+1 < len(hex_list):
            new_hex.append(hex_list[i+1])
            new_hex.append(hex_list[i])
        else:
            new_hex.append(hex_list[i])
            
    formatted = ', '.join(new_hex)
    return f"unsigned char {name}[] = {{{formatted}}};"

# Find big_block_1 to big_block_8 and swap bytes
for i in range(1, 9):
    name = f"big_block_{i}"
    pattern = rf"unsigned char {name}\[\] = \{{([^}}]+)\}};"
    content = re.sub(pattern, lambda m, n=name: swap_bytes(m, n), content)

with open('sprites.h', 'w') as f:
    f.write(content)
