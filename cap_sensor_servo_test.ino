#include <Servo.h>

const int cap_sensor = 8;
bool has_sensed = false;

const int servo_pin = 9;
int default_angle = 0;
int paper_angle = 90;
int plastic_angle = 130;

Servo sliderServo;

void setup() {
  Serial.begin(9600);
  pinMode(cap_sensor, INPUT);
  sliderServo.attach(servo_pin);
  sliderServo.write(default_angle);
}

void loop() {
  has_sensed = digitalRead(cap_sensor);

  if (has_sensed == true) {
    Serial.println("I have detected something!... grrr");
    //Go to paper
    sliderServo.write(paper_angle);
  }
  else {
    Serial.println("Nothing is detected....");
    sliderServo.write(plastic_angle);
  }

  delay(500);
  sliderServo.write(default_angle);
}
