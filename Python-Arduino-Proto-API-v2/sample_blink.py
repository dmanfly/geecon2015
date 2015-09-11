from arduino import Arduino
import time

try:
    b = Arduino('/dev/tty.usbmodem1421')
except:
    b = Arduino('/dev/ttyACM0')
servo1 = 13
servo2 = 12

b.output([])
b.input([])
b.servos([servo1, servo2])   
# for i in xrange(18):
#     # b.setHigh(pin)
#     # time.sleep(1)
#     # print b.getState(pin)
#     # b.setLow(pin)
#     # print b.getState(pin)
#     # time.sleep(1)
#     an = 3.14/180 * i * 10
#     an = i*10
#     print an
#     b.setAngle(12,an)
    
    # time.sleep(1)
for i in xrange(10):
    for pos in range (0,180,5):
        b.setAngle(servo1,pos)
        b.setAngle(servo2,pos)
        time.sleep(0.030)
    for pos in range (180,0,-5):
        b.setAngle(servo1,pos)
        b.setAngle(servo2,pos)
        time.sleep(0.030)


b.close()

