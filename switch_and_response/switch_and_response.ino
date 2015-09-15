#include <Servo.h>

struct trap{  
  int servoPin;
  int buttonPin;
  int ledPin;
  Servo servo;
};

int traps_len=4;
trap traps[] = {{2,A0,13},{3,A1,12},{4,A2,11},{5,A3,10}};

int startAngle =0;
int endAngle=90;
int servoDelay=  400;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int i=0;i<traps_len;i++)
  {
    traps[i].servo.attach(traps[i].servoPin);
    traps[i].servo.write(startAngle);
    pinMode(traps[i].ledPin,OUTPUT);
  }
}

void loop() {
    for(int i=0;i<traps_len;i++)
    {
      int btnVal = analogRead(traps[i].buttonPin);
      if (btnVal > 0)
      {
        Serial.println(i);
        Serial.println(btnVal);
        digitalWrite(traps[i].ledPin,HIGH);
        traps[i].servo.write(endAngle);
        delay(servoDelay);  
        traps[i].servo.write(startAngle);
        digitalWrite(traps[i].ledPin,LOW);
        delay(servoDelay);
      }
    }
  delay(10);
}

