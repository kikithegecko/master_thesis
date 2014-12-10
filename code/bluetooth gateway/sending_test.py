import serial

BLUETOOTH_ADDR = 'COM4'
bt = serial.Serial(BLUETOOTH_ADDR, 115200)

line = 255255000
while True:
	bt.write((str(line) + '\n').encode())
	if(line < 255255255):
		line += 1
	else:
		line -= 1
