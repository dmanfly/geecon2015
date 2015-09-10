
#ifndef SERIAL_RATE
#define SERIAL_RATE         115200
#endif

#ifndef SERIAL_TIMEOUT
#define SERIAL_TIMEOUT      5
#endif

#include <Servo.h> 

int minPulse = 600;   // minimum servo position, us (microseconds)
int maxPulse = 2400;  // maximum servo position, us

Servo servo;
      

void setup() {
    Serial.begin(SERIAL_RATE);
    Serial.setTimeout(SERIAL_TIMEOUT);
    
    servo.attach(12);
    
    int len = readData();
    for (int i = 0; i < len; i++) {
        pinMode(readData(), OUTPUT);
    }
    len = readData();
    for (int i = 0; i < len; i++) {
        pinMode(readData(), INPUT);
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
            servo.write(readData());  break;
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
