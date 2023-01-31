#include <Sabertooth.h>
//Packet serial
Sabertooth ST1(128);
Sabertooth ST2(130); //actual value unkown, will have to chenge later
//st1 and st2 refer to the different motor controllers

//later, we can comdense both motor controllers to one class such as
//Sabertooth ST [2] = { Sabertooth(128), Sabertooth(130) };


int motorSpeed = 0;
// Connections to make:
//   Arduino TX->1  ->  Sabertooth S1
//   Arduino GND    ->  Sabertooth 0V
//   Arduino VIN    ->  Sabertooth 5V (OPTIONAL, if you want the Sabertooth to power the Arduino)
// baud rate 9600
//dip switches for address 128
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
  Serial.println("Enter motor speed(-127 to 127)..."); // ask user input
  ST1.setTimeout(10000); // this will cause the motor controls to stop all motors if a new input is not received in this timeframe
  ST2.setTimeout(10000);
  //only works in increments of 100 milliseconds
  //this will stop motors on 10 seconds without new input

//  ST.setDeadband(20); //this will create a deadspot from -20 to 20
  //this will stop the motor if the incoming input is in the deadspot for one second
  //will help later to fine tune controller

    ST1.setRamping(14); //this makes sure the motors dont burn themselves out when changing speed suddenly
    ST1.setRamping(14);


    ST1.setMinVoltage(30);//will turn off motors if the voltage gets too low, such as low battery
    ST2.setMinVoltage(30);//Value = (desired volts-6) x 5 
 
    //ST.setMaxVoltage(71);//this will stop regenerative braking if they start producing more power than the battery can accept at a time
    //ST.setMaxVoltage(71);//Value = Desired Volts*5.12
    //according to the documentation this should not matter since we are using a battery, we can leave at default
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()>0){ //receive user input
    motorSpeed = Serial.parseInt();
    Serial.print("Motor Speed: ");
    Serial.println(motorSpeed);
  }
  if (motorSpeed >=-127 && motorSpeed <=127){
  ST1.motor(1, motorSpeed); //turn on motor 1
  delay(5000); //wait 5 sec
  ST1.motor(1, 0); // turn off motor 1
  ST1.motor(2, motorSpeed); // turn on motor 2
  delay(5000);//wait 5 sec
  ST1.motor(2, 0); // turn off motor 2
  delay (5000);
  
  ST2.motor(1, motorSpeed); //turn on motor 1
  delay(5000); //wait 5 sec
  ST2.motor(1, 0); // turn off motor 1
  ST2.motor(2, motorSpeed); // turn on motor 2
  delay(5000);//wait 5 sec
  ST2.motor(2, 0); // turn off motor 2
  }
}
