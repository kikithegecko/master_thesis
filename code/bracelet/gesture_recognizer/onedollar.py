# A 1$ Gesture Recognizer
import math

class Point:
	def __init__(self):
		self.x = 0
		self.y = 0
		#self.z = 0

class Bounding_Box(points):
	def __init__(self):
		min_x = float("inf")
		min_y = float("inf")
		#min_z = float("inf")
		max_x = float("-inf")
		max_y = float("-inf")
		#max_z = float("-inf")
		for p in points:
			if p.x < min_x:
				min_x = p.x
			elif p.x > max_x:
				max_x = p.x
			if p.y < min_y:
				min_y = p.y
			elif p.y > max_y:
				max_y = p.y
			#if p.z < min_z:
				#min_z = p.z
			#elif p.z > max_z:
				#max_z = p.z
		self.width = abs(max_x - min_x)
		self.height = abs(max_y - min_y)
		#self.depth = abs(max_z - min_z)

def centroid(points):
	c = Point()
	for p in points:
		c.x += p.x
		c.y += p.y
		#c.z += p.z
	c.x /= len(points)
	c.y /= len(points)
	#c.z /= len(points)

# resample a points path into n evenly spaced points
def resample(points, n):
	increment = path_length(points) / (n - 1)
	D = 0
	newpoints = points[:1]
	for i in range(1, len(points[1:])):
		dist = distance(points[i-1], points[i])
		if(D + dist) > increment:
			q = Point()
			q.x = points[i-1].x + ((increment - D) / dist) * (points[i].x - points[i-1].x)
			q.y = points[i-1].y + ((increment - D) / dist) * (points[i].y - points[i-1].y)
			#q.z = points[i-1].z + ((increment - D) / dist) * (points[i].z - points[i-1].z) #TODO
			newpoints.append(q)
			points.insert(i, q)
			D = 0
		else:
			D = D + dist
	return newpoints
	
def path_length(A):
	dist = 0
	for i in range(1, len(A)):
		dist += distance(A[i-1], A[i])
	return dist
	
def distance(p, q):
	return sqrt((q.x - p.x)**2 + (q.y - p.y)**2)
	#return sqrt((q.x - p.x)**2 + (q.y - p.y)**2 + (q.z - p.z)**2)
	
# rotate points so that their indicative angle is at 0 degrees
def rotate_to_zero(points):
	c = centroid(points)
	theta = math.atan(c.y - points[0].y, c.x - points[0].x) # for -pi <= theta <= pi
	newpoints = rotate_by(points, -theta)
	return newpoints
	
def rotate_by(points, theta):
	c = centroid(points)
	for p in points:
		q = Point()
		q.x = (p.x - c.x) * cos(thetha) - (p.y - c.y) * sin(theta) + c.x
		q.y = (p.x - c.x) * sin(thetha) - (p.y - c.y) * cos(theta) + c.y
		append(newpoints, q)
	return newpoints
	
# scale points so that the resulting bounding box will be of size*size dimension;
# then translate points to the origin. bounding_box returns a rectangle according to
# (min.x, min.x), (max.x, max.y).
# for gestures serving as templates, steps 1-3 should be carried out once on 
# the raw input points. For candidates, steps 1-4 should be used just after 
# the candidate is articulated.
def scale_to_square(points, size):
	B = bounding_box(points)
	newpoints = []
	for p in points:
		q = Point()
		q.x = p.x * (size / B.width)
		q.y = p.y * (size / B.height)
		#q.z = p.z * (size / B.depth)
		newpoints.append(q)
	return newpoints
	
def translate_to_origin(points):
	c = centroid(points)
	newpoints = []
	for p in points:
		q = Point()
		q.x = p.x - c.x
		q.y = p.y - c.y
		#q.z = p.z - c.z
		newpoints.append(q)
	return newpoints
	
# match points against a set of templates. The size variable in recognize 
# refers to the size passed to scale_to_square. the symbol phi equals 0.5*(-1 + sqrt(5)). 
# We use thetha = +- 45 degrees and theta_delta = 2 degrees in recognize. 
# Due to using resample, we can assume that A abd B in path_distance contain 
# the same number of points, i.e. |A| = |B|.
def recognize(points, templates):
	b = float("inf")
	for t in templates:
		d = distance_at_best_angle(points, t, -theta, theta, theta_delta) #TODO theta stuff
		if d < b:
			b = d
			t_best = t
	score = 1 - b / 0.5 * sqrt(size*size + size*size)
	return (T, score)

def distance_at_best_angle(points, thehta_a, thehta_b, theta_delta):
	phi = 0.5 * (-1 + sqrt(5))
	x1 = phi * thehta_a + (1 - phi) * theta_b
	f1 = distance_at_angle(points, T, x1)
	x2 = (1 - phi) * theta_a + phi * theta_b
	f2 = distance_at_angle(points, T, x2)
	while abs(theta_b - theta_a) > theta_delta:
		if f1 < f2:
			theta_b = x2
			x2 = x1
			f2 = f1
			x1 = phi * theta_a + (1 - phi) * theta_b
			f1 = distance_at_angle(points, T, x1)
		else:
			theta_a = x1
			x2 = x1
			f2 = f1
			x1 = phi * theta_a + (1 - phi) * theta_b
			f1 = distance_at_angle(points, T, x2)
	return min(f1, f2)
	
def distance_at_angle(points, T, theta):
	newpoints = rotate_by(points, theta)
	d = path_distance(newpoints, T_points)
	return d
	
def path_distance(A, B):
	d = 0
	for i in range(len(A)):
		d = d + distance(A[i], B[i])
	return d / len(A)
