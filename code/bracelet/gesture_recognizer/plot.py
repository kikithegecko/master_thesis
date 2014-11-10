import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

#f = open("c:\\Users\\kb\\Documents\\master_thesis\\code\\bracelet\\accel_test\\sample_gesture3.txt","r")

for j in range(3):
	fig = plt.figure()
	ax = fig.add_subplot(111, projection='3d')
	for i in range(10):
		f = open(str(j*10+i+1) + ".txt", "r")
		lines = f.readlines()
	
		xs = []
		ys = []
		zs = []
	
		for l in lines:
			coords = l.split(' ')
			xs.append(float(coords[0]))
			ys.append(float(coords[1]))
			zs.append(float(coords[2].strip('\n')))
		
		#ax.scatter(xs, ys, zs)
		ax.plot(xs, ys, zs)
		
	plt.show()
	
