#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import time

class Arduino(object):

    __OUTPUT_PINS = -1

    def __init__(self, port, baudrate=115200):
        self.serial = serial.Serial(port, baudrate)
        self.serial.write(b'FF')

    def __str__(self):
        return "Arduino is on port %s at %d baudrate" %(self.serial.port, self.serial.baudrate)

    def setup(self, outputPins, inputPins, servoPins):
        print "Setting up master"

    	self._output(outputPins);
    	self._input(inputPins);
    	self._servos(servoPins);

    def setupSlave(self, slaveIdx, slaveTx, outputPins, inputPins, servoPins):
    	self.__sendData('7')
    	self.__sendData(slaveIdx);
    	self.__sendData(slaveTx);

        self.__sendData(len(outputPins))
        for each_pin in outputPins:
            self.__sendData(each_pin)

    	self.__sendData(len(inputPins))
    	for each_pin in inputPins:
    	    self.__sendData(each_pin)

    	self.__sendData(len(servoPins))
    	for each_pin in servoPins:
    	    self.__sendData(each_pin)

    def nextCommandAsSlave(self, slaveIdx):
    	self.__sendData('8')
    	self.__sendData(slaveIdx)

    def _output(self, pinArray):
        print pinArray
        self.__sendData(len(pinArray))
        for each_pin in pinArray:
            self.__sendData(each_pin)    
        return True
    def _input(self, pinArray):
        self.__sendData(len(pinArray))
        for each_pin in pinArray:
            self.__sendData(each_pin)
        return True
    def _servos(self, pinArray):
    	self.__sendData(len(pinArray))
        for each_pin in pinArray:
            self.__sendData(each_pin)
        return True

    def setLow(self, pin):
        self.__sendData('1')
        self.__sendData(pin)
        return True

    def setHigh(self, pin):
        self.__sendData('2')
        self.__sendData(pin)
        return True

    def getState(self, pin):
        self.__sendData('3')
        self.__sendData(pin)
        return self.__formatPinState(self.__getData()[0])

    def analogWrite(self, pin, value):
        self.__sendData('4')
        self.__sendData(pin)
        self.__sendData(value)
        return True

    def analogRead(self, pin):
        self.__sendData('5')
        self.__sendData(pin)
        return self.__getData()
    def digitalRead(self,pin):
        self.__sendData('7')
        self.__sendData(pin)
        return self.__getData()
    def setAngle(self, pin, angle):
        self.__sendData('6')
        self.__sendData(pin)
        self.__sendData(angle)
        return True

    def turnOff(self):
        for each_pin in self.__OUTPUT_PINS:
            self.setLow(each_pin)
        return True

    def __sendData(self, serial_data):
        while(1):
	       data = self.__getData()
	       if len(data) > 0 and data[0] == "w":
		      break
        serial_data = str(serial_data).encode('utf-8')
        self.serial.write(serial_data)

    def __getData(self):
        input_string = self.serial.readline()
        #print "IN: " + input_string
    	if input_string[0] == "X":
            print "DEBUG! : " +  input_string
    	try:
    	    input_string = input_string.decode('utf-8')
    	except:
    	    print "undecoded string " + input_string
        return input_string.rstrip('\n')

    def __formatPinState(self, pinValue):
        if pinValue == '1':
            return True
        else:
            return False

    def close(self):
        self.serial.close()
        return True
