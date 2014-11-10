import serial

s = serial.Serial('COM9', 115200)
filename = 1

while True:
	line = s.readline().decode('ascii')
	if line == "start recording\r\n":
		f = open(str(filename)+".txt", "w")
		for i in range(150): #current sample size is 150
			gesture = []
			line = s.readline().decode('ascii')
			line = line.replace('\r', '')
			f.write(line)
		f.close()
		filename += 1
