# A 1$ Gesture Recognizer
import math

class Point:
	def __init__(self, x=0, y=0, z=0):
		self.x = x
		self.y = y
		self.z = z
	def __str__(self):
		return str(self.x) + " " + str(self.y) + " " + str(self.z)

# helper function for getting a bounding box around a gesture
class Bounding_Box:
	def __init__(self, points):
		min_x = float("inf")
		min_y = float("inf")
		min_z = float("inf")
		max_x = float("-inf")
		max_y = float("-inf")
		max_z = float("-inf")
		for p in points:
			if p.x < min_x:
				min_x = p.x
			elif p.x > max_x:
				max_x = p.x
			if p.y < min_y:
				min_y = p.y
			elif p.y > max_y:
				max_y = p.y
			if p.z < min_z:
				min_z = p.z
			elif p.z > max_z:
				max_z = p.z
		self.width = abs(max_x - min_x)
		self.height = abs(max_y - min_y)
		self.depth = abs(max_z - min_z)

# helper function for calculating the centroid
def centroid(points):
	c = Point()
	for p in points:
		c.x += p.x
		c.y += p.y
		c.z += p.z
	c.x /= len(points)
	c.y /= len(points)
	c.z /= len(points)
	return c
	
# helper function for calculating the cross product
# between two vectors/points
def cross_product(p, q):
	r = Point()
	r.x = p.y * q.z - p.z * q.y
	r.y = p.z * q.x - p.x * q.z
	r.z = p.x * q.y - p.y * q.x
	return r
		

# STEP 1:
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
			q.z = points[i-1].z + ((increment - D) / dist) * (points[i].z - points[i-1].z) #TODO
			newpoints.append(q)
			points.insert(i, q)
			D = 0
		else:
			D = D + dist
	return newpoints

# helper function for step 1
def path_length(A):
	dist = 0
	for i in range(1, len(A)):
		dist += distance(A[i-1], A[i])
	return dist
	
# helper function for step 1:
# calculate eucledian distance
def distance(p, q):
	#return sqrt((q.x - p.x)**2 + (q.y - p.y)**2)
	return math.sqrt((q.x - p.x)**2 + (q.y - p.y)**2 + (q.z - p.z)**2)
	
# STEP 2:
# rotate points so that their indicative angle is at 0 degrees
def rotate_to_zero(points):
	c = centroid(points)
	#theta = math.atan(c.y - points[0].y, c.x - points[0].x) # for -pi <= theta <= pi TODO check this?
	scalarprod = points[0].x * c.x + points[0].y * c.y + points[0].z * c.z
	theta = math.acos(scalarprod / (math.sqrt(points[0].x**2 + points[0].y**2 + points[0].z**2) * math.sqrt(c.x**2 + c.y**2 + c.z**2)))
	v_axis = cross_product(points[0], c)
	#newpoints = rotate_by(points, -theta)
	newpoints = rotate_rodrigues(points, v_axis, theta)
	return newpoints

# helper function for step 2	
def rotate_by(points, theta):
	c = centroid(points)
	newpoints = []
	for p in points:
		q = Point()
		q.x = (p.x - c.x) * cos(thetha) - (p.y - c.y) * sin(theta) + c.x
		q.y = (p.x - c.x) * sin(thetha) - (p.y - c.y) * cos(theta) + c.y
		newpoints.append(q)
	return newpoints
	
# helper function for step 2:
# vector rotation in axis-angle representation
# using Rodrigues' rotation formula
def rotate_rodrigues(points, vector, theta):
	c = centroid(points)
	newpoints = []
	for p in points:
		q = Point()
		cp = cross_product(p, vector)
		sp = p.x * vector.x + p.y * vector.y + p.z * vector.z
		q.x = math.cos(theta) * p.x + math.sin(theta) * cp.x + (1 - math.cos(theta)) * sp * vector.x
		q.y = math.cos(theta) * p.y + math.sin(theta) * cp.y + (1 - math.cos(theta)) * sp * vector.y
		q.z = math.cos(theta) * p.z + math.sin(theta) * cp.z + (1 - math.cos(theta)) * sp * vector.z
		newpoints.append(q)
	return newpoints
		
	
# scale points so that the resulting bounding box will be of size*size dimension;
# then translate points to the origin. bounding_box returns a rectangle according to
# (min.x, min.x), (max.x, max.y).
# for gestures serving as templates, steps 1-3 should be carried out once on 
# the raw input points. For candidates, steps 1-4 should be used just after 
# the candidate is articulated.
def scale_to_square(points, size): # in 3$ paper, size=100
	B = Bounding_Box(points)
	newpoints = []
	for p in points:
		q = Point()
		q.x = p.x * (size / B.width)
		q.y = p.y * (size / B.height)
		q.z = p.z * (size / B.depth)
		newpoints.append(q)
	return newpoints
	
def translate_to_origin(points):
	c = centroid(points)
	newpoints = []
	for p in points:
		q = Point()
		q.x = p.x - c.x
		q.y = p.y - c.y
		q.z = p.z - c.z
		newpoints.append(q)
	return newpoints

# STEP 4:	
# match points against a set of templates. The size variable in recognize 
# refers to the size passed to scale_to_square. the symbol phi equals 0.5*(-1 + sqrt(5)). 
# Due to using resample, we can assume that A abd B in path_distance contain 
# the same number of points, i.e. |A| = |B|.
#TODO
def recognize(points, templates, rescale_size):
	#'constants'
	#theta_min = -45
	theta_min = -180
	#theta_max = 45
	theta_max = 180
	theta_delta = 2
	
	best = float("inf")
	for t in templates:
		dist = distance_at_best_angle(points, t, theta_min, theta_max, theta_delta)
		if dist < best:
			best = dist
			t_best = t
	#score = 1 - best / 0.5 * sqrt(rescale_size**2 + rescale_size**2)
	score = 1 - best / (0.5 * math.sqrt(3 * rescale_size**2))
	return (t_best, score)

def distance_at_best_angle(points, template, theta_min, theta_max, theta_delta):
	phi = 0.5 * (-1 + math.sqrt(5))
	alpha_min = theta_min
	alpha_max = theta_max
	beta_min = theta_min
	beta_max = theta_max
	gamma_min = theta_min
	gamma_max = theta_max
	
	x1 = phi * alpha_min + (1 - phi) * alpha_max
	x2 = (1 - phi) * alpha_min + phi * alpha_max
	y1 = phi * beta_min + (1 - phi) * beta_max
	y2 = (1 - phi) * beta_min + phi * beta_max
	z1 = phi * gamma_min + (1 - phi) * gamma_max
	z2 = (1 - phi) * gamma_min + phi * gamma_max
	
	f1 = distance_at_angle(points, template, x1, y1, z1)
	f2 = distance_at_angle(points, template, x1, y1, z2)
	f3 = distance_at_angle(points, template, x1, y2, z1)
	f4 = distance_at_angle(points, template, x1, y2, z2)
	f5 = distance_at_angle(points, template, x2, y1, z1)
	f6 = distance_at_angle(points, template, x2, y1, z2)
	f7 = distance_at_angle(points, template, x2, y2, z1)
	f8 = distance_at_angle(points, template, x2, y2, z2)

	while (abs(alpha_max - alpha_min) > theta_delta) and (abs(beta_max - beta_min) > theta_delta) and (abs(gamma_max - gamma_min) > theta_delta):	
		min_f = min(f1, f2, f3, f4, f5, f6, f7, f8)
		
		if min_f == f1: #x1, y1, z1
			alpha_max = x2
			x2 = x1
			x1 = phi * alpha_min + (1 - phi) * alpha_max
			beta_max = y2
			y2 = y1
			y1 = phi * beta_min + (1 - phi) * beta_max
			gamma_max = z1
			z2 = z1
			z1 = phi * gamma_min + (1 - phi) * gamma_max
			f8 = f1
			f1 = distance_at_angle(points, template, x1, y1, z1)
			f2 = distance_at_angle(points, template, x1, y1, z2)
			f3 = distance_at_angle(points, template, x1, y2, z1)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f5 = distance_at_angle(points, template, x2, y1, z1)
			f6 = distance_at_angle(points, template, x2, y1, z2)
			f7 = distance_at_angle(points, template, x2, y2, z1)
		elif min_f == f2: #x1, y1, z2
			alpha_max = x2
			x2 = x1
			x1 = phi * alpha_min + (1 - phi) * alpha_max
			beta_max = y2
			y2 = y1
			y1 = phi * beta_min + (1 - phi) * beta_max
			gamma_min = z1
			z1 = z2
			z2 = (1 - phi) * gamma_min + phi * gamma_max
			f7 = f2
			f1 = distance_at_angle(points, template, x1, y1, z1)
			f2 = distance_at_angle(points, template, x1, y1, z2)
			f3 = distance_at_angle(points, template, x1, y2, z1)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f5 = distance_at_angle(points, template, x2, y1, z1)
			f6 = distance_at_angle(points, template, x2, y1, z2)
			f8 = distance_at_angle(points, template, x2, y2, z2)
		elif min_f == f3: #x1, y2, z1
			alpha_max = x2
			x2 = x1
			x1 = phi * alpha_min + (1 - phi) * alpha_max
			beta_min = y2
			y1 = y2
			y2 = (1 - phi) * beta_min + phi * beta_max
			gamma_max = z1
			z2 = z1
			z1 = phi * gamma_min + (1 - phi) * gamma_max
			f6 = f3
			f1 = distance_at_angle(points, template, x1, y1, z1)
			f2 = distance_at_angle(points, template, x1, y1, z2)
			f3 = distance_at_angle(points, template, x1, y2, z1)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f5 = distance_at_angle(points, template, x2, y1, z1)
			f7 = distance_at_angle(points, template, x2, y2, z1)
			f8 = distance_at_angle(points, template, x2, y2, z2)
		elif min_f == f4: #x1, y2, z2
			alpha_max = x2
			x2 = x1
			x1 = phi * alpha_min + (1 - phi) * alpha_max
			beta_min = y2
			y1 = y2
			y2 = (1 - phi) * beta_min + phi * beta_max
			gamma_min = z1
			z1 = z2
			z2 = (1 - phi) * gamma_min + phi * gamma_max
			f5 = f4
			f1 = distance_at_angle(points, template, x1, y1, z1)
			f2 = distance_at_angle(points, template, x1, y1, z2)
			f3 = distance_at_angle(points, template, x1, y2, z1)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f6 = distance_at_angle(points, template, x2, y1, z2)
			f7 = distance_at_angle(points, template, x2, y2, z1)
			f8 = distance_at_angle(points, template, x2, y2, z2)
		elif min_f == f5: #x2, y1, z1
			alpha_min = x1
			x1 = x2
			x2 = (1 - phi) * alpha_min + phi * (alpha_max)
			beta_max = y2
			y2 = y1
			y1 = phi * beta_min + (1 - phi) * beta_max
			gamma_max = z1
			z2 = z1
			z1 = phi * gamma_min + (1 - phi) * gamma_max
			f4 = f5
			f1 = distance_at_angle(points, template, x1, y1, z1)
			f2 = distance_at_angle(points, template, x1, y1, z2)
			f3 = distance_at_angle(points, template, x1, y2, z1)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f5 = distance_at_angle(points, template, x2, y1, z1)
			f6 = distance_at_angle(points, template, x2, y1, z2)
			f7 = distance_at_angle(points, template, x2, y2, z1)
			f8 = distance_at_angle(points, template, x2, y2, z2)
		elif min_f == f6: #x2, y1, z2
			alpha_min = x1
			x1 = x2
			x2 = (1 - phi) * alpha_min + phi * (alpha_max)
			beta_max = y2
			y2 = y1
			y1 = phi * beta_min + (1 - phi) * beta_max
			gamma_min = z1
			z1 = z2
			z2 = (1 - phi) * gamma_min + phi * gamma_max
			f3 = f6
			f1 = distance_at_angle(points, template, x1, y1, z1)
			f2 = distance_at_angle(points, template, x1, y1, z2)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f5 = distance_at_angle(points, template, x2, y1, z1)
			f6 = distance_at_angle(points, template, x2, y1, z2)
			f7 = distance_at_angle(points, template, x2, y2, z1)
			f8 = distance_at_angle(points, template, x2, y2, z2)
		elif min_f == f7: #x2, y2, z1
			alpha_min = x1
			x1 = x2
			x2 = (1 - phi) * alpha_min + phi * (alpha_max)
			beta_min = y2
			y1 = y2
			y2 = (1 - phi) * beta_min + phi * beta_max
			gamma_max = z1
			z2 = z1
			z1 = phi * gamma_min + (1 - phi) * gamma_max
			f2 = f7
			f1 = distance_at_angle(points, template, x1, y1, z1)
			f3 = distance_at_angle(points, template, x1, y2, z1)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f5 = distance_at_angle(points, template, x2, y1, z1)
			f6 = distance_at_angle(points, template, x2, y1, z2)
			f7 = distance_at_angle(points, template, x2, y2, z1)
			f8 = distance_at_angle(points, template, x2, y2, z2)
		else: #x2, y2, z2
			alpha_min = x1
			x1 = x2
			x2 = (1 - phi) * alpha_min + phi * (alpha_max)
			beta_min = y2
			y1 = y2
			y2 = (1 - phi) * beta_min + phi * beta_max
			gamma_min = z1
			z1 = z2
			z2 = (1 - phi) * gamma_min + phi * gamma_max
			f1 = f8
			f2 = distance_at_angle(points, template, x1, y1, z2)
			f3 = distance_at_angle(points, template, x1, y2, z1)
			f4 = distance_at_angle(points, template, x1, y2, z2)
			f5 = distance_at_angle(points, template, x2, y1, z1)
			f6 = distance_at_angle(points, template, x2, y1, z2)
			f7 = distance_at_angle(points, template, x2, y2, z1)
			f8 = distance_at_angle(points, template, x2, y2, z2)

	return min(f1, f2, f3, f4, f5, f6, f7, f8)
	
def distance_at_angle(points, template, alpha, beta, gamma):
	#newpoints = rotate_by(points, theta)
	newpoints = rotate_rodrigues(points, Point(1, 0, 0), alpha)
	newpoints = rotate_rodrigues(newpoints, Point(0, 1, 0), beta)
	newpoints = rotate_rodrigues(newpoints, Point(0, 0, 1), gamma)
	d = path_distance(newpoints, template)
	return d

# assume |A| = |B| (usually valid due to resampling)
def path_distance(A, B):
	d = 0
	for i in range(len(A)):
		d = d + distance(A[i], B[i])
	return d / len(A)
