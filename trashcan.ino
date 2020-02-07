//include library/ies
#include <Servo.h>

//Declare Variables
int servoPin = 11;
int openAngle = 90;
int closeAngle = 10;

Servo myServo;

void setup() {
  //initial setup here
  Serial.begin(9600);
  myServo.attach(servoPin);
  myServo.write(closeAngle);
}


void loop() {
  //declare input variable
  char input;

  //check if input is available
  if (Serial.available() > 0) {
    //get the input
    input = Serial.read();
    Serial.println(input);
    //evaluate the input
    if (input == 'k') {
      myServo.write(openAngle);
    }
    else if (input == 'o') {
      myServo.write(closeAngle);
    }
    else {
      Serial.println("Invalid..");
    }
  }

}
