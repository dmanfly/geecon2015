from arduino import Arduino
import time

MASTER_CHIP = -1
SLAVE1_CHIP = 0
SLAVE2_CHIP = 1

SLAVE1_TX = 7
SLAVE2_TX = 5

current_scene = None

class Item:
    def __init__(self, active_sessions, chip):
	self.active = None
	self.active_sessions = active_sessions
	self.chip = chip
    
    def _start_active_session(self, arduino, session, now):
	pass 

    def _live_session(self, arduino, session, now):
	pass 

    def _stop_active_session(self, arduino, session):
	pass

    def _get_input_pin(self):
	return None

    def _get_output_pin(self):
	return None

    def _get_servo_pin(self):
	return None
	
    def live(self, arduino, now):
	for session in self.active_sessions:
	    if now > session["start"] and now < av["start"] + av["length"]:
	    	if not self.active_session: 
	    	    self.active = session
		    self._start_active_session(self, arduino, session, now)
		else:
		    self._live_session(self, arduino, session, now)    	
		return
	   
	if self.active:
	    self._stop_active_session(self, arduino, self.active)
	    self.active = None 
    
    def destroy():
	if self.active:
	    self._stop_active_session(self, arduino, self.active)
	    self.active = None

class Servo(Item):
    def __init__(self, chip, pin, active_sessions):
	Item.__init__(self, active_sessions, chip)
	self.pin = pin

    def _setAngle(self, angle):
	if self.chip != MASTER_CHIP:
	    arduino.nextCommandAsSlave(self.chip)
	arduino.setAngle(self.pin, angle); 	
    
    def _start_active_session(self, arduino, session, now):
	self._setAngle(session["angle_start"])

    def _live_session(self, arduino, session, now):	
	total_journey = session["angle_end"] - session["angle_start"]  	
	rel_angle = session["angle_start"] + (now - session["start"]) / session["length"] * total_journey
	self._setAngle(rel_angle)

    def _stop_active_session(self, arduino, session):
	self._setAngle(session["angle_start"])

    def _get_servo_pin(self):
	return self.pin

class LED(Item):
    def __init__(self, chip, pin, active_sessions):
	Item.__init__(self, active_sessions, chip)
	self.pin = pin

    def _setPin(to_high):
	if self.chip != MASTER_CHIP:
	    arduino.nextCommandAsSlave(self.chip)
	if to_high:	
	    arduino.setHigh(self.pin);
	else:
	    arduino.setLow(self.pin);

    def _start_active_session(self, arduino, session, now):
	self._setPin(True);

    def _stop_active_session(self, arduino, session):
	self._setPin(False);

    def _get_output_pin(self):
	return self.pin

class Switch:
    def __init__(self, pin, scene):
	self.pin = pin
	self.scene = scene

    def get_input_pin(self):
	return self.pin

    def check_trigger(self, arduino):
	if arduino.getState(self.pin):
            if current_scene is not None:
		current_scene.destroy()
	    current_scene.start()	    

class Scene:
    def __init__(self, scene_items): 
	self.scene_items = scene_items
	self.now = 0
    
    def start(self):
	self.now = 0

    def play_for(self, arduino, interval):
	for item in self.scene_items:
	    item.live(arduino, self.now)
	self.now += interval
			
    def destory(self):
	for item in self.scene_items:
	    item.destroy()	

scene1 = Scene(
[
    LED(MASTER_CHIP, 13, [dict(start=1000, end=4000)]),
    LED(MASTER_CHIP, 12, [dict(start=1000, end=1200), dict(start=1400, end=1600), dict(start=1800, end=2000)]), 
    Servo(MASTER_CHIP, 9, [dict(start=4000, end=5000, angle_start=40, angle_end=100)])
])

all_scenes = [scene1]
switches = [Switch(6, scene1)]

pins_by_chip = {chip: dict(servos=set(), output=set()) for chip in [MASTER_CHIP, SLAVE1_CHIP, SLAVE2_CHIP]}
for scene in all_scenes:
    for item in scene.scene_items:
	chip = item.chip
	servo_pin = item._get_servo_pin()
	output_pin = item._get_output_pin()	
	if servo_pin:
	   pins_by_chip[chip]["servos"].add(servo_pin)
	elif output_pin:
	   pins_by_chip[chip]["output"].add(output_pin)
	
master_inputs = [switch.pin for switch in switches]

try:
    b = Arduino('/dev/tty.usbmodem1421')
    print "Connected to usbmodem1421"
except:
    try:
    	b = Arduino('/dev/ttyACM1')
	print "Connected to ttyACM1"
    except:
        b = Arduino('/dev/ttyUSB0')
	print "Connected to ttyUSB0"

b.setup(pins_by_chip[MASTER_CHIP]["output"], master_inputs, pins_by_chip[MASTER_CHIP]["servos"])
#b.setupSlave(SLAVE1_CHIP, SLAVE1_TX, pins_by_chip[SLAVE1_CHIP]["output"], [], pins_by_chip[SLAVE1_CHIP]["output"])
#b.setupSlave(SLAVE2_CHIP, SLAVE2_TX, pins_by_chip[SLAVE2_CHIP]["output"], [], pins_by_chip[SLAVE2_CHIP]["output"])
  
while (1):
    for switch in switches:
	switch.check_trigger(b)	    
   
    if current_scene:
	current_scene.play_for(b, 30)
    time.sleep(30)
	
