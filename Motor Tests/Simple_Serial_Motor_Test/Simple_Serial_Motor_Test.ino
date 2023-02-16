#include <SabertoothSimplified.h>
/simple serial
SabertoothSimplified ST;
int motorSpeed = 0;
// Connections to make:
//   Arduino TX->1  ->  Sabertooth S1
//   Arduino GND    ->  Sabertooth 0V
//   Arduino VIN    ->  Sabertooth 5V (OPTIONAL, if you want the Sabertooth to power the Arduino)
// baud rate 9600
//dip switches
// 1 on
// 2 off
// 3 on
// 4 off
// 5 on
// 6 off

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SabertoothTXPinSerial.begin(9600);// start communication
  Serial.println("Enter motor speed(0-255)..."); // ask user input
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()>0){ //receive user input
    motorSpeed = Serial.parseInt();
    Serial.print("Motor Speed: ");
    Serial.println(motorSpeed);
  }
  ST.motor(1, motorSpeed); //turn on motor 1
  delay(5000); //wait 5 sec
  ST.motor(1, 0); // turn off motor 1
  ST.motor(2, motorSpeed); // turn on motor 2
  delay(5000);//wait 5 sec
  ST.motor(2, 0); // turn off motor 2
}
