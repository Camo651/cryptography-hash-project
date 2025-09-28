import sys
import re


file = sys.argv[1]

lines = []
with open(file, 'r') as f:
    data = f.read()
    lines = data.split('\n')

stack = []
heatmap = {}
stack_heatmap = {}
# Each line can begin with "start" or "end". It then has a function name with each nested component separated by dots.
# ex: start SHA256_Final.loop2.if1
onlyalphanumRegex = r'[^a-zA-Z0-9_. ]'
for i, line in enumerate(lines):
    line = re.sub(onlyalphanumRegex, '', line)
    if not line:
        continue
    parts = line.split(' ')
    if len(parts) < 2:
        print(f"Error: Malformed line at {i+1}: '{line}'")
        sys.exit(1)
        continue
    command = parts[0].replace('\x00', '')
    func = parts[1].replace('\x00', '')
    heatmap[func] = heatmap.get(func, 0) + 1
    if command == 'start':
        stack.append(func)
    elif command == 'end':
        fullstack = '/'.join(stack)
        stack_heatmap[fullstack] = stack_heatmap.get(fullstack, 0) + 1
        stack_top = stack.pop() if stack else None
        if stack_top != func:
            print(f"Error: Mismatched end at line {i+1}: expected {stack_top}, got {func}")
            sys.exit(1)
            



# print heatmap
total = sum(heatmap.values())
heatmap = dict(sorted(heatmap.items(), key=lambda item: item[1], reverse=True))
for func, count in heatmap.items():
    print(f"{func}: {count/total:.2%}")


print("\nStack heatmap:")
total = sum(stack_heatmap.values())
stack_heatmap = dict(sorted(stack_heatmap.items(), key=lambda item: item[1], reverse=True))
for func, count in stack_heatmap.items():
    print(f"{func}: {count/total:.2%}")