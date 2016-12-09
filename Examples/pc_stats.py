import psutil

print "CPU usage: %" + str(psutil.cpu_percent(interval=1))
print "RAM usage: %" + str(psutil.virtual_memory().percent)

