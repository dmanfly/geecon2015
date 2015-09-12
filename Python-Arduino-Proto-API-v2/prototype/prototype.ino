
#ifndef SERIAL_RATE
#define SERIAL_RATE         115200
#endif

#ifndef SERIAL_TIMEOUT
#define SERIAL_TIMEOUT      5
#endif

#include <Servo.h> 
#include <SoftwareSerial.h>

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
SoftwareSerial slaves[1] = {SoftwareSerial(10,11)};

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
    slaves[0].begin(SERIAL_RATE); 
    
    int pins[MAX_PINS];

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
    Serial.println("XXGot pins!");
    pinMode(7,OUTPUT);
    digitalWrite(7,HIGH);
}

void loop() {
    int cmd = readData();
    char debug_msg[40] = {0};
    snprintf(debug_msg, 40, "cmd %d", cmd);
    Serial.println(debug_msg);

    switch (cmd) {
        case 1 :
            // set digital low
            digitalWrite(readData(), LOW); 
            break;
        case 2 :
            //set digital high
            digitalWrite(readData(), HIGH); 
            break;
        case 3 :
            //get digital value
            Serial.println(digitalRead(readData())); 
            break;
        case 4 :
            // set analog value
            analogWrite(readData(), readData()); 
            break;
        case 5 :
            //read analog value
            Serial.println(analogRead(readData())); 
            break;
        case 6 :
        {
            int servoPin = readData();
            int angle = readData();
            Servo* servo = findServo(servoPin);
            if (servo) {
              servo->write(angle);  
            }
            break;
        }
        case 7:
        {
            // Initialize slave's serial
            int slaveId = readData();
            int slaveTx = readData();
            int inputPins[MAX_PINS];
            int outputPins[MAX_PINS];
            int servoPins[MAX_PINS];
            int outputLen = readArray(outputPins, MAX_PINS);
            int inputLen = readArray(inputPins, MAX_PINS);
            int servoLen = readArray(servoPins, MAX_PINS);

            SoftwareSerial& ser = slaves[slaveId];
            
            writeArray(ser, outputLen, outputPins);
            writeArray(ser, inputLen, inputPins);
            writeArray(ser, servoLen, servoPins);
            
            break;
        }
        case 8:
        {
          int slaveId = readData();
          int slave_cmd = readData();
          int pin = readData();
          
          int cmdTypes[7] = {0, 0, 0, 1, 2, 1, 2};
          int type = cmdTypes[slave_cmd]; 
          SoftwareSerial& ser = slaves[0];                            
          
          if (type == 0) { // set/unsetPin
            writeInt(ser, slave_cmd); 
            writeInt(ser, pin);
            char debug_msg[20] = {0};
            snprintf(debug_msg, 20, "Set %d %d %d", slave_cmd, pin, slave_cmd == 1);
            Serial.println(debug_msg);
          }
          else if (type == 1) { // readPin
             // nothing
          }
          else if (type == 2) { // writeAnalog/Servo  
             int val = readData();
             char debug_msg[20] = {0};
             snprintf(debug_msg, 20, "Set %d %d %d", cmd, pin, val);
             Serial.println(debug_msg);
             
             writeInt(ser, slave_cmd);
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
    char debug_msg[20] = {0};
    snprintf(debug_msg, 20, "XXread len %d", len);
    Serial.println(debug_msg);
    
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


void writeInt(SoftwareSerial& ser, int intie) {
  //Serial.println("writeInt");
  //while (ser.available() == 0);
  int inei = ser.read();
  while (inei != 'w' && inei != -1) {
    inei = ser.read();
  }
  //Serial.print("reading.. :");
  //Serial.print(ser.read());
  char msg[100]={' '};
  //sprintf(msg,"%d", intie);
  itoa(intie, msg + 1, 10);
  ser.print(msg);
}

void writeArray(SoftwareSerial& ser, int len, int* array) {
  writeInt(ser, len);
  for (int i = 0; i < len; i++) {
    writeInt(ser, array[i]);
  }
}
