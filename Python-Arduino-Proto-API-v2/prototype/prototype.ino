
#ifndef SERIAL_RATE
#define SERIAL_RATE         115200
#endif

#ifndef SERIAL_TIMEOUT
#define SERIAL_TIMEOUT      5
#endif

#include <Servo.h> 
#include <SendOnlySoftwareSerial.h>

int minPulse = 600;   // minimum servo position, us (microseconds)
int maxPulse = 2400;  // maximum servo position, us

#define _MAX_SERVOS 10
#define MAX_SLAVES 3
#define MAX_PINS 32
#define INVALID_PIN -1
         
struct ServoAndPin {
  Servo o;
  int pin;
};
ServoAndPin servos[_MAX_SERVOS];
SendOnlySoftwareSerial* slavesSerials[MAX_SLAVES];

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

    // Null slaves
    for (int i = 0; i < MAX_SLAVES; i++) {
      slavesSerials[i] = NULL;
    }
    int pins[MAX_PINS];
    
    while (readData() != 9);
    
    // Output
    int len = readArray(pins, MAX_PINS);
    for (int i = 0; i < len; i++) {
      pinMode(pins[i], OUTPUT);
    }
    // Input
    len = readArray(pins, MAX_PINS);
    for (int i = 0; i < len; i++) {
      pinMode(pins[i], INPUT);
    }
    //Servos
    len = readArray(pins, MAX_PINS);
    for (int i = 0; i < _MAX_SERVOS; i++) {
      if (i < len) { 
          servos[i].pin = pins[i];          
          servos[i].o.attach(pins[i]);
      }
      else {
        servos[i].pin = INVALID_PIN;
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
        case 6:
        {
            // Initialize slave's serial
            int slaveId = readData();
            int slaveTx = readData();
            int inputPins[MAX_PINS];
            int outputPins[MAX_PINS];
            int servoPins[MAX_PINS];
            int inputLen = readArray(inputPins, MAX_PINS);
            int outputLen = readArray(outputPins, MAX_PINS);
            int servoLen = readArray(servoPins, MAX_PINS);
            
            if (slavesSerials[slaveId]) {
              slavesSerials[slaveId]->end();
              delete slavesSerials[slaveId];
            }
            slavesSerials[slaveId] = new SendOnlySoftwareSerial((uint8_t)slaveTx); 
            slavesSerials[slaveId]->begin(SERIAL_RATE);
            
            writeArray(slavesSerials[slaveId], inputLen, inputPins);
            writeArray(slavesSerials[slaveId], outputLen, outputPins);
            writeArray(slavesSerials[slaveId], servoLen, servoPins);
            
            break;
        }
        case 7:
        {
          int slaveId = readData();
          int cmd = readData();
          
          int cmdTypes[6] = {0, 0, 1, 2, 1, 2};
          int type = cmdTypes[cmd]; 
          SendOnlySoftwareSerial* ser = slavesSerials[slaveId];
          if (!ser) return;                            
          
          int pin = readData();
          if (type == 0) { // set/unsetPin
            writeInt(ser, cmd);
            writeInt(ser, pin);
          }
          else if (type == 1) { // readPin
             // nothing
          }
          else if (type == 2) { // writeAnalog/Servo  
             int val = readData();
             writeInt(ser, cmd);
             writeInt(ser, pin);
             writeInt(ser, val);
          }       
        }   
        case 99:
            //just dummy to cancel the current read, needed to prevent lock 
            //when the PC side dropped the "w" that we sent
            break;
    }
}

int readArray(int* array, int BUFFER_SIZE) {
    int len = readData();
    for (int i = 0; i < len; i++) {
        int val = readData();
        if (i < BUFFER_SIZE)
          array[i] = val; 
    }
    
    return len;
}

int readData() {
    Serial.println("w");
    while(1) {
        if(Serial.available() > 0) {
            return Serial.parseInt();
        }
    }
}


void writeInt(SendOnlySoftwareSerial* ser, int intie) {
  uint8_t* pByteArray = (uint8_t*)&intie;
  
  for (int i = 0; i < 4; i++) {
    ser->write(pByteArray[i]);
  }
}

void writeArray(SendOnlySoftwareSerial* ser, int len, int* array) {
  writeInt(ser, len);
  for (int i = 0; i < len; i++) {
    writeInt(ser, array[i]);
  }
}
