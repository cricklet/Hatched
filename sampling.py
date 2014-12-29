import random, math

r = lambda: random.random() * 2 - 1
m = lambda x: math.sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2])
n = lambda x: (x[0] / m(x), x[1] / m(x), x[2] / m(x))

a = lambda x: math.atan2(x[2], math.sqrt(x[0]*x[0] + x[1]*x[1]))

for i in range(24):
    x = (0,0,0)

    while a(x) < 0.2:
        x = (r(), r(), random.random())
        x = n(x)
        scale = float(i + 1) / float(32)
        x = (x[0] * scale, x[1] * scale, x[2] * scale)

    print 'vec3(%f, %f, %f), ' % x

