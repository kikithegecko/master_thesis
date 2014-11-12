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
corner1 = file2gesture("templates/corner_1.txt")
corner2 = file2gesture("templates/corner_2.txt")
corner3 = file2gesture("templates/corner_3.txt")
arc1 = file2gesture("templates/arc_1.txt")
arc2 = file2gesture("templates/arc_2.txt")
arc3 = file2gesture("templates/arc_3.txt")
circle1 = file2gesture("templates/circle_1.txt")
circle2 = file2gesture("templates/circle_2.txt")
circle3 = file2gesture("templates/circle_3.txt")

templates = [corner1, corner2, corner3, arc1, arc2, arc3, circle1, circle2, circle3]

for t in range(len(templates)):
	fig = plt.figure()
	ax = fig.add_subplot(111, projection='3d')
	xs = []
	ys = []
	zs = []
	for i in range(len(templates[t])):
		xs.append(templates[t][i].x)
		ys.append(templates[t][i].y)
		zs.append(templates[t][i].z)
	#ax.plot(xs, ys, zs)
	templates[t] = onedollar.resample(templates[t], 64) #magic number for the start
	templates[t] = onedollar.rotate_to_zero(templates[t])
	templates[t] = onedollar.scale_to_square(templates[t], 100) #magic number from 3$ paper
	templates[t] = onedollar.translate_to_origin(templates[t])
	#print("Template length: " + str(len(templates[t])))
	# now plot dem templates
	xs = []
	ys = []
	zs = []
	for i in range(len(templates[t])):
		xs.append(templates[t][i].x)
		ys.append(templates[t][i].y)
		zs.append(templates[t][i].z)
	#ax.plot(xs, ys, zs)
	#plt.show()

# now do the same for the "unknown" gestures
gestures = []
for i in range(30):
	gestures.append(file2gesture("sample_data/" + str(i+1) + ".txt"))
for g in gestures:
	xs = []
	ys = []
	zs = []
	for i in range(len(g)):
		xs.append(g[i].x)
		ys.append(g[i].y)
		zs.append(g[i].z)
	#ax.plot(xs, ys, zs)
	g = onedollar.resample(g, 64)
	g = onedollar.rotate_to_zero(g)
	g = onedollar.scale_to_square(g, 100)
	g = onedollar.translate_to_origin(g)
	#print("Gesture length: " + str(len(g)))
	result = onedollar.recognize(g, templates, 100)
	#print("score: " + str(result[1]))
	match = templates.index(result[0])
	#print("match: " + str(match))
	if match < 4:
		print("corner")
	elif match < 7:
		print("arc")
	else:
		print("circle")
#plt.show()
