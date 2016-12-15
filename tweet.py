import project_library
import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=0)
print ser
#print project_library.send_twit("IYTE")
while 1:
    time.sleep(4)
    try:
	print "try"
        x=ser.readline()
	print x
        data = x.split(',')
	print data[0]
	print data[1]
        print data[2]
	print type(data[0])
	print float(data[0])/100
       # print  project_library.send_twit( "long=" + data[0] + " lat=" + data[1] + " time=" + data[2] )
	print project_library.send_twit("http://maps.google.com/maps?q=loc:" + str( float(data[0])/100) + "," +str(float(data[1])/100))
    except:
        print('Data could not be read')
