import project_library
import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=0)
print ser
while 1:
    time.sleep(4)
    try:
	print "try"
        x=ser.readline()
	print x
        data = x.split(',')

        print  project_library.send_twit( "long=" + data[0] + " lat=" + data[1] + " time=" + data[2] )
    except:
        print('Data could not be read')
