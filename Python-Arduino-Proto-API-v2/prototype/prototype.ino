
#ifndef SERIAL_RATE
#define SERIAL_RATE         115200
#endif

#ifndef SERIAL_TIMEOUT
#define SERIAL_TIMEOUT      5
#endif

#include <Servo.h> 

int minPulse = 600;   // minimum servo position, us (microseconds)
int maxPulse = 2400;  // maximum servo position, us

#define _MAX_SERVOS 10
#define INVALID_SERVO_PIN -1

struct ServoAndPin {
  Servo o;
  int pin;
};
ServoAndPin servos[_MAX_SERVOS];

Servo* findServo(int pin) {
  for (int i = 0; i < _MAX_SERVOS; i++) {
    if (servos[i].pin == pin) {
      return &servos[i].o;
    }
  }
  return NULL;
}  

void setup() {
    Serial.begin(SERIAL_RATE);
    Serial.setTimeout(SERIAL_TIMEOUT);
    
    // Output pins
    int len = readData();
    for (int i = 0; i < len; i++) {
        pinMode(readData(), OUTPUT);
    }
    // Input pins
    len = readData();
    for (int i = 0; i < len; i++) {
        pinMode(readData(), INPUT);
    }
    // Servos
    len = readData();
    for (int i = 0; i < _MAX_SERVOS; i++) {
        if (i < len) {
          int pin = readData(); 
          servos[i].pin = pin;          
          servos[i].o.attach(pin);
        }
        else {
          servos[i].pin = INVALID_SERVO_PIN;
        }
    }
}

void loop() {
    switch (readData()) {
        case 0 :
            //set digital low
            digitalWrite(readData(), LOW); break;
        case 1 :
            //set digital high
            digitalWrite(readData(), HIGH); break;
        case 2 :
            //get digital value
            Serial.println(digitalRead(readData())); break;
        case 3 :
            // set analog value
            analogWrite(readData(), readData()); break;
        case 4 :
            //read analog value
            Serial.println(analogRead(readData())); break;
        case 5 :
        {
            int servoPin = readData();
            int angle = readData();
            Servo* servo = findServo(servoPin);
            if (servo) {
              servo->write(angle);  
            }
            break;
        }
        case 6 :
            Serial.println(digitalRead(readData()));break;
        case 99:
            //just dummy to cancel the current read, needed to prevent lock 
            //when the PC side dropped the "w" that we sent
            break;
    }
}

int readData() {
    Serial.println("w");
    while(1) {
        if(Serial.available() > 0) {
            return Serial.parseInt();
        }
    }
}
