from arduino import Arduino
import time

b = Arduino('/dev/tty.usbmodem1421')
pin = 13

#declare output pins as a list/tuple
b.output([pin,13])
b.input([8])
    
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
pos=1
d = 1
s=20
while (1):
    button = b.digitalRead(8)
    
    # if (button!=u'0\r'):
    #     print "["+button+"]" + str(pos)
    #     b.setAngle(12,pos)
    #     pos += 4* d
    #     time.sleep(0.001)
    #     if pos>180 or pos<= 0:
    #         d=-1*d
    if (button!=u'0\r'):
         b.setAngle(12,90)
         time.sleep(1)
         b.setAngle(12,0)
         time.sleep(1)

# for pos in range (0,180,5):
#     b.setAngle(12,pos)
#     time.sleep(0.030)
# for pos in range (180,0,-5):
#     b.setAngle(12,pos)
#     time.sleep(0.030)


b.close()

