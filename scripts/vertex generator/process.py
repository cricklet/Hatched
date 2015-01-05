uvs_file = open('uvs.txt')
posnorms_file = open('posnorms.txt')

uvs = []
for line in uvs_file:
    if len(line.strip()) < 1: continue

    uv = line.strip().strip(',').split(',')[-2:]
    uv = [x.replace('f','').strip() for x in uv]
    uv = [float(x) for x in uv]
    uv = tuple(uv)
    
    uvs.append(uv)

positions = []
norms = []
for line in posnorms_file:
    if len(line.strip()) < 1: continue

    pos = line.strip().strip(',').replace('f','').split(',')[0:3]
    pos = [float(x) for x in pos]
    pos = tuple(pos)
    
    norm = line.strip().strip(',').replace('f','').split(',')[3:6]
    norm = [float(x) for x in norm]
    norm = tuple(norm)

    positions.append(pos)
    norms.append(norm)
    
print len(positions)
print len(norms)
print len(uvs)

everything = []

for i in range(0,len(uvs)):
    e = positions[i] + norms[i] + uvs[i]
    e = ["{0:.1f}".format(x) for x in e]
    e = tuple(e)
    everything.append(e)

for e in everything:
    print "%s,%s,%s, %s,%s,%s, %s,%s," % e
