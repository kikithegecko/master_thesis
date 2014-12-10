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
#TODO fill templates

while True:
	line = bracelet.readline().decode('ascii')
	#print(line)
	if line == "start recording\r\n": # gesture recognition
		gesture = []
		for i in range(150): #current sample size is 150
			line = bracelet.readline().decode('ascii')
			line = line.replace('\r', '')
			coords = line.split(' ')
			p = onedollar.Point(float(coords[0]), float(coords[1]), float(coords[2].strip('\n')))
			gesture.append(p)
		gesture = onedollar.resample(gesture, 64)
		gesture = onedollar.rotate_to_zero(gesture)
		gesture = onedollar.scale_to_square(gesture, 100)
		gesture = onedollar.translate_to_origin(gesture)
		#print("Gesture length: " + str(len(gesture)))
		result = onedollar.recognize(gesture, templates, 100)
		#print("score: " + str(result[1]))
		match = templates.index(result[0])
		#TODO return the corresponding shape
		bracelet.write(match)
	elif line.startswith("LAMP"): #lamp color change command
		#the first 6 chars can be omitted, they are always "LAMP: "
		line = line[6:]
		#print(line)
		bt.write(line.encode())
		
		
		
