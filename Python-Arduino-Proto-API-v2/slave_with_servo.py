from arduino import Arduino
import time

try:
    b = Arduino('/dev/tty.usbmodem1421')
    print "Connected to usbmodem1421"
except:
    try:
    	b = Arduino('/dev/ttyACM0')
	print "Connected to ttyACM0"
    except:
        b = Arduino('/dev/ttyUSB0')
	print "Connected to ttyUSB0"

master_led_pin = 13
master_servo_pin = 12

slave_connection_pin = 11# In master
slave_led_pin = 13
slave_servo_pin = 12

#declare output pins as a list/tuple
b.setup([master_led_pin], [], [master_servo_pin])
b.setupSlave(0, slave_connection_pin, [slave_led_pin], [], [slave_servo_pin])
    
for i in xrange(18):
     b.setHigh(master_led_pin)
     b.nextCommandAsSlave(0)
     b.setHigh(slave_led_pin)

     time.sleep(0.5)
     b.setLow(master_led_pin)
     b.nextCommandAsSlave(0)
     b.setLow(slave_led_pin)
     time.sleep(0.5) 
"""
     
     b.setAngle(master_servo_pin, i * 10)
     b.nextCommandAsSlave(0)
     b.setAngle(slave_servo_pin, i * 10)    
"""	
b.close()

