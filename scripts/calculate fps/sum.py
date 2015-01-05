total = 0
num = 0
for line in file('numbers.txt'):
    total += float(line)
    num += 1

avg =  total / num

print 1 / avg
