#!/usr/bin/python

import sys

data_file = sys.argv[1]
histogram_file = sys.argv[2]

def zeros2D(x, y):
    a = []
    for i in xrange(x):
        a.append([])
        for j in xrange(y):
            a[i].append(0)
    return a
    
def arange(start, end, step):
    a = []
    i = start
    while i < end:
        a.append(i)
        i+=step
    return a

# Here, angles are defined from [-180 to 180] degrees.
# You could choose another domain. Points will be automatically
# wrapped inside that domain. This can be useful for symetrical
# side chains, for instance. Be careful to use an appropriate domain,
# otherwise the wrapping will produce meaningless data.
x_min, x_max, y_min, y_max = -180.0, 180.0, -180.0, 180.0

# Number of 2D regions in which the plot is divided.
x_resolution, y_resolution = 180, 180

def read_angles(line):
    tokens = line.split()
    x = float(tokens[0])
    y = float(tokens[1])
    while x < x_min:
        x = x_max - (x_min - x)
    while x >= x_max:
        x = x_min + (x - x_max)
    while y < y_min:
        y = y_max - (y_min - y)
    while y >= y_max:
        y = y_min + (y - y_max)
    return [x, y]

points = [read_angles(line) for line in open(data_file)]
count = len(points)
histogram = zeros2D(x_resolution, y_resolution)
x_interval_length = (x_max - x_min) / x_resolution
y_interval_length = (y_max - y_min) / y_resolution
interval_surface = x_interval_length * y_interval_length
increment = 1000.0 / count / interval_surface

for i in points:
    x = int((i[0] - x_min) / x_interval_length)
    y = int((i[1] - y_min) / y_interval_length)
    histogram[x][y] += increment

x_intervals = arange(x_min, x_max, (x_max - x_min) / x_resolution)
y_intervals = arange(y_min, y_max, (y_max - y_min) / y_resolution)

o = open(histogram_file, 'w')
for i, x in enumerate(x_intervals):
    for j, y in enumerate(y_intervals):
        o.write('%4d %4d %.3f \n' % (x, y, histogram[i][j]))
    o.write('\n')

o2 = open(data_file, 'w')
s = 0
for i in points:
    x = i[0]
    y = i[1]
    t = s % 10
    s = s+1
    if t == 0:
        o2.write('%5.1f %5.1f\n' % (x, y))
