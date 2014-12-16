# This is a gateway for Bluetooth communication over serial connection,
# since the Bluetooth module in the bracelet is currently not working.
# Color control data sent over USB serial is forwarded to the connected
# Bluetooth device via serial profile.
# In addition, this code also handles gesture recognition.

import serial
import onedollar

# CONFIG
#BRACELET_ADDR = '/dev/ttyACM0'
BRACELET_ADDR = 'COM9'
#BLUETOOTH_ADDR = '/dev/ttyACM1'
BLUETOOTH_ADDR = 'COM4'

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
	#recognizer preprocessing
	gesture = onedollar.resample(gesture, 64) #magic number for the start
	gesture = onedollar.rotate_to_zero(gesture)
	gesture = onedollar.scale_to_square(gesture, 100) #magic number from 3$ paper
	gesture = onedollar.translate_to_origin(gesture)
	f.close()
	return gesture

bracelet = serial.Serial(BRACELET_ADDR, 9600)
bt = serial.Serial(BLUETOOTH_ADDR, 9600)
templates = []
#circles
templates.append(file2gesture("74.txt")) #circle[24], jan
templates.append(file2gesture("135.txt")) #circle[45], henning
templates.append(file2gesture("27.txt")) #karo
templates.append(file2gesture("42.txt")) #oli
templates.append(file2gesture("105.txt")) #sync
templates.append(file2gesture("180.txt")) #imp
templates.append(file2gesture("201.txt")) #mcsilver
templates.append(file2gesture("227.txt")) #hut

#pigatils
templates.append(file2gesture("21.txt")) #karo
templates.append(file2gesture("114.txt")) #sync
templates.append(file2gesture("59.txt")) #oli
templates.append(file2gesture("90.txt")) #jan
templates.append(file2gesture("144.txt")) #henning
templates.append(file2gesture("177.txt")) #imp
#templates.append(file2gesture("209.txt")) #mcsilver
templates.append(file2gesture("245.txt")) #hut

#zorros
templates.append(file2gesture("40.txt")) #oli
templates.append(file2gesture("6.txt")) #karo
templates.append(file2gesture("64.txt")) #jan
templates.append(file2gesture("97.txt")) #sync
templates.append(file2gesture("130.txt")) #henning
templates.append(file2gesture("158.txt")) #imp
#templates.append(file2gesture("185.txt")) #mcsilver
templates.append(file2gesture("224.txt")) #hut


while True:
	line = bracelet.readline().decode('ascii')
	print(line.encode())
	if line == "start recording\r\n": # gesture recognition
		gesture = []
		for i in range(150): #current sample size is 150
			line = bracelet.readline().decode('ascii')
			line = line.replace('\r', '')
			coords = line.split(' ')
			p = onedollar.Point(float(coords[0]), float(coords[1]), float(coords[2].strip('\n')))
			gesture.append(p)
		print("end recording")
		gesture = onedollar.resample(gesture, 64)
		gesture = onedollar.rotate_to_zero(gesture)
		gesture = onedollar.scale_to_square(gesture, 100)
		gesture = onedollar.translate_to_origin(gesture)
		#print("Gesture length: " + str(len(gesture)))
		result = onedollar.recognize(gesture, templates, 100)
		#print("score: " + str(result[1]))
		match = templates.index(result[0])
		print(match)
		if match < 8: #circle
			print("circle")
			#bracelet.write("circle\n".encode())
			bt.write("255050000\n".encode())
		elif match < 15: #pigtail
			print("pigtail")
			#bracelet.write("pigtail\n".encode())
			bt.write("150000150\n".encode())
		else:
			print("zorro")
			#bracelet.write("zorro\n".encode())
			bt.write("255255255\n".encode())
		#TODO return the corresponding shape
		#bracelet.write(match)
	elif line.startswith("LAMP"): #lamp color change command
		#the first 6 chars can be omitted, they are always "LAMP: "
		line = line[6:]
		line = line.replace('\r','')
		#print(line.encode())
		bt.write(line.encode())
		
		
		
