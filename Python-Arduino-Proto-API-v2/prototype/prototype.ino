
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
//void print(const char* str)
//{
//  snprintf(debug_msg, 12, "XXread len %d\n", len);
//  Serial.println(debug_msg);`
//}
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
    Serial.println("r");
    while (1) {
      if(Serial.available() > 0) {
        if (Serial.parseInt() == 99) {
          Serial.println("XXFound 99!");
          break;
       }
      }
      else {
        delay(20);
      }
    }
    Serial.println("XXGetting Output pins!");
    // Output
    int len = readArray(pins, MAX_PINS);
    for (int i = 0; i < len; i++) {
      pinMode(pins[i], OUTPUT);
    }
    Serial.println("XXGot Output pins");
    // Input
    len = readArray(pins, MAX_PINS);
    for (int i = 0; i < len; i++) {
      pinMode(pins[i], INPUT);
    }
    //Servos
    Serial.println("XXGetting Input pins!");
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
    Serial.println("XXGetting Servo pins!");
    pinMode(7,OUTPUT);
    digitalWrite(7,HIGH);
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
            char debug_msg[40] = {0};
            snprintf(debug_msg, 40, "XXtx:%d in:%d, out : %d, ser : %d",slaveTx, inputLen,outputLen, servoLen);
            Serial.println(debug_msg);
            if (slavesSerials[slaveId]) {
              slavesSerials[slaveId]->end();
              delete slavesSerials[slaveId];
            }
            
            slavesSerials[slaveId] = new SendOnlySoftwareSerial((uint8_t)slaveTx); 
            slavesSerials[slaveId]->begin(SERIAL_RATE);
            
            writeInt(slavesSerials[slaveId], 99);
            delay(100);
            
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
    while (len == 99){
      len = readData();
    }
    
    char debug_msg[12] = {0};
    snprintf(debug_msg, 12, "XXread len %d", len);
    Serial.println(debug_msg);
    
    for (int i = 0; i < len; i++) {
        int val = readData();
//        Serial.println("XXread data read:%d",val);
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
  char debug_msg[12] = {0};
  snprintf(debug_msg, 12, "XX%d", intie);
  Serial.println(debug_msg);
  
  char msg[10] = {0};
  int len = snprintf(msg, 10, "%d", intie);
  for (int i = 0; i < len; i++) {
    ser->write(msg[i]);
  }
}

void writeArray(SendOnlySoftwareSerial* ser, int len, int* array) {
  writeInt(ser, len);
  for (int i = 0; i < len; i++) {
    writeInt(ser, array[i]);
  }
}
