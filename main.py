#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from subprocess import PIPE, Popen

import project_library
import serial
import time
import urllib
import psutil
import time

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=0)

params = urllib.urlencode({'key': 'KI5JPZ91PRQUBKN7'})
ser.flushInput()
while 1:
    time.sleep(1)
    try:
        x=ser.readline()
	print x
	with open("log.txt", "a") as myfile:
    		current_time=time.strftime("%Y-%m-%d-%H-%M-%S: ")
		myfile.write(current_time+x+'\n')
        data = x.split(',')

	D1=str((data[0][0:2]))
	M1=str((data[0][2:9]))
	
	D2=str((data[1][0:3]))
	M2=str((data[1][3:10]))

	saat = str(int(data[2][:2])+3)
	dak = data[2][2:4]
	san = data[2][4:6]

	long=str(float(D1)+float(M1)/60)
	lat=str(float(D2)+float(M2)/60)

	print project_library.send_twit("Saat=" + saat +  ":" + dak + ":" + san  +  " http://maps.google.com/maps?q=loc:" + str(float(long)) + "," +str(float(lat)))
	# @KarakusOktay @AzadKaratas
#	f1 = urllib.urlopen("https://api.thingspeak.com/update?field1="+str( float(data[0])/100), data=params)
#	f2 = urllib.urlopen("https://api.thingspeak.com/update?field1="+str( float(data[0])/100),"https://api.thingspeak.com/update?field2="+str( float(data[1])/100) data=params)
#	f3 = urllib.urlopen("https://api.thingspeak.com/update?field3="+str(psutil.cpu_percent(interval=1)), data=params)

	process = Popen(['vcgencmd', 'measure_temp'], stdout=PIPE)
	output, _error = process.communicate()
#	f4 = urllib.urlopen("https://api.thingspeak.com/update?field4="+str(output[output.index('=') + 1:output.rindex("'")]), data=params)

#	f5 = urllib.urlopen("https://api.thingspeak.com/update?field5="+str(psutil.virtual_memory().percent), data=params)

	f1 = urllib.urlopen("https://api.thingspeak.com/update?field1="+str( float(data[0])/100)+"&field2="+str( float(data[1])/100)+"&field3="+str(psutil.cpu_percent(interval=1))+"&field4="+str(output[output.index('=')+1:output.rindex("'")])+"&field5="+str(psutil.virtual_memory().percent), data=params)
	print f1.read
    except:
        y=x
