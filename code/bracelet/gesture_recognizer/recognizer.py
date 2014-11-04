import onedollar
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# read gesture data from file
# each line must contain x, y, z values
# separated by spaces
def file2gesture(fname):
	f = open(fname, "r")
	data = f.readlines()
	gesture = []
	for line in data:
		coords = line.split(' ')
		p = onedollar.Point(float(coords[0]), float(coords[1]), float(coords[2].strip('\n')))
		gesture.append(p)
	f.close()
	return gesture


# first, process the sample gestures
circle = file2gesture("sample_data/circle1.txt")
line = file2gesture("sample_data/line1.txt")
arc = file2gesture("sample_data/arc1.txt")
templates = [circle, arc, line]
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
for t in templates:
	xs = []
	ys = []
	zs = []
	for i in range(len(t)):
		xs.append(t[i].x)
		ys.append(t[i].y)
		zs.append(t[i].z)
	ax.plot(xs, ys, zs)
	t = onedollar.resample(t, 64) #magic number for the start
	t = onedollar.rotate_to_zero(t)
	t = onedollar.scale_to_square(t, 100) #magic number from 3$ paper
	# now plot dem templates
	xs = []
	ys = []
	zs = []
	for i in range(len(t)):
		xs.append(t[i].x)
		ys.append(t[i].y)
		zs.append(t[i].z)
	#ax.plot(xs, ys, zs)
plt.show()

# now do the same for the "unknown" gestures
gestures = []
gestures.append(file2gesture("sample_data/arc2.txt"))
gestures.append(file2gesture("sample_data/arc3.txt"))
gestures.append(file2gesture("sample_data/circle2.txt"))
gestures.append(file2gesture("sample_data/line2.txt"))
for g in gestures:
	g = onedollar.resample(g, 64)
	g = onedollar.rotate_to_zero(g)
	g = onedollar.scale_to_square(t, 100)
	result = onedollar.recognize(g, templates, 100)
	print(result[1])
	print(templates.index(result[0]))
