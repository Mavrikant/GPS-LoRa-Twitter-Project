import serial
import time

ser = serial.Serial('COM3', 9600, timeout=0)

while 1:
    time.sleep(1.6)
    try:
        x=ser.readline()
        data = x.split(',')

        print data[0]
        print data[1]
        print data[2]


    except ser.SerialTimeoutException:
        print('Data could not be read')

