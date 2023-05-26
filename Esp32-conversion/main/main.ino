#include "Sabertooth.h"
#include <SoftwareSerial.h>

#include <Bluepad32.h>

GamepadPtr myGamepads[BP32_MAX_GAMEPADS];

//Packet serial
SoftwareSerial SWSerial(NOT_A_PIN, 23 ); // RX on no pin (unused), TX on pin 14 (to S1).
Sabertooth STL(128, SWSerial);
Sabertooth STR(129, SWSerial);
//  Sabertooth STL(128);
//  Sabertooth STR(129);
//STL and STR refer to the different motor controllers


//these are the pins the pots are connected to
int pot_SPEED = 5;
int pot_DRIFT = 8;
int outprinter = 0;//0 means drive
int driftpot = 1;//0 means no pot

//these are the max recorded values on the pots, use encoder_input test function to verify whenever pots change
const float Drift_pot_max_val = 798;
const float Speed_pot_max_val = 798;

//declare the variables i use to multiply in
float SPEED_PERCENT = .5;
float DRIFT_CONTROL = 1.0;

//these are the motor variables initialized
int input_c_R = 0;
int input_c_L = 0;
int output_R = 0;
int output_L = 0;


// Connections to make:
//   Arduino (TX->1)use 14 instead!!!!!!  ->  Sabertooth S1
//   Arduino GND    ->  Sabertooth 0V
//   Arduino VIN    ->  Sabertooth 5V (OPTIONAL, if you want the Sabertooth to power the Arduino)
// baud rate 9600

//dip switches for address 128
// 1 off
// 2 off
// 3 on
// 4 on
// 5 on
// 6 on

//dip switches for address 129
// 1 off
// 2 off
// 3 on
// 4 off
// 5 on
// 6 on



hw_timer_t *Timer0_Cfg = NULL;



void IRAM_ATTR Timer0_ISR() {//timer 4 interrupts every 50ms

      //safety, just make sure the imputs dont reach values they are not supposed to
    if (input_c_R >127){ input_c_R = 127;}
    if (input_c_R <-127){ input_c_R = -127;}
    if (input_c_L >127){ input_c_L = 127;}
    if (input_c_L <-127){ input_c_L = -127;}
    
  //first call the math functions using our inputs
  output_R = SmoothVelocity_R(input_c_R * SPEED_PERCENT);
  output_L = SmoothVelocity_L(input_c_L * SPEED_PERCENT);

//testing data, either write to motors, or write to laptop to debug
if (outprinter == 1){
   Serial.print("Output R ");
   Serial.println(output_R);
   Serial.print("Output L ");
   Serial.println(output_L);
   Serial.print("Speed ");
   Serial.println(SPEED_PERCENT);
   Serial.print("Drift ");
   Serial.println(DRIFT_CONTROL);
}else {
  STR.motor(1, output_R);
  STR.motor(2, output_R);
  STL.motor(1, output_L);
  STL.motor(2, output_L);
}
  
}


void setup() { 
  // put your setup code here, to run once:
  // start communication  
  SWSerial.begin(9600); // start talking to motor controller
  STL.setTimeout(100); // this will cause the motor controls to stop all motors if a new input is not received in this timeframe
  STR.setTimeout(100);
  //only works in increments of 100 milliseconds
  //this will stop motors on .10 seconds without new input

  //  ST.setDeadband(20); //this will create a deadspot from -20 to 20
  //this will stop the motor if the incoming input is in the deadspot for one second
  //will help later to fine tune controller


  cli();//stop interrupts

    Timer0_Cfg = timerBegin(0, 800, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 5000, true);
    timerAlarmEnable(Timer0_Cfg);

  sei();//allow interrupts
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
  BP32.forgetBluetoothKeys();
}

void onConnectedGamepad(GamepadPtr gp) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myGamepads[i] == nullptr) {
            Serial.printf("CALLBACK: Gamepad is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            GamepadProperties properties = gp->getProperties();
            Serial.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n", gp->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myGamepads[i] = gp;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Gamepad connected, but could not found empty slot");
    }
}

void onDisconnectedGamepad(GamepadPtr gp) {
    bool foundGamepad = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myGamepads[i] == gp) {
            Serial.printf("CALLBACK: Gamepad is disconnected from index=%d\n", i);
            myGamepads[i] = nullptr;
            foundGamepad = true;
            break;
        }
    }

    if (!foundGamepad) {
        Serial.println("CALLBACK: Gamepad disconnected, but not found in myGamepads");
    }
}


/*
 * Main loop
*/
void loop() {
  // put your main code here, to run repeatedly:
  //both joysticks go from 0 to 255
  //forward is zero backwards is 255
  //resting is 127

  // This call fetches all the gamepad info from the NINA (ESP32) module.
    // Just call this function in your main loop.
    // The gamepads pointer (the ones received in the callbacks) gets updated
    // automatically.
    BP32.update();

    // It is safe to always do this before using the gamepad API.
    // This guarantees that the gamepad is valid and connected.
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        GamepadPtr myGamepad = myGamepads[i];

        if (myGamepad && myGamepad->isConnected()) {
            // There are different ways to query whether a button is pressed.
            // By query each button individually:
            //  a(), b(), x(), y(), l1(), etc...
            // Another way to query the buttons, is by calling buttons(), or
            // miscButtons() which return a bitmask.
            // Some gamepads also have DPAD, axis and more.
//            Serial.printf(
//                "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, "
//                "%4d, brake: %4d, throttle: %4d, misc: 0x%02x\n",
//                i,                        // Gamepad Index
//                myGamepad->dpad(),        // DPAD
//                myGamepad->buttons(),     // bitmask of pressed buttons
//                myGamepad->axisX(),       // (-511 - 512) left X Axis
//                myGamepad->axisY(),       // (-511 - 512) left Y axis
//                myGamepad->axisRX(),      // (-511 - 512) right X axis
//                myGamepad->axisRY(),      // (-511 - 512) right Y axis
//                myGamepad->brake(),       // (0 - 1023): brake button
//                myGamepad->throttle(),    // (0 - 1023): throttle (AKA gas) button
//                myGamepad->miscButtons()  // bitmak of pressed "misc" buttons
//            );
              
              input_c_R = map(myGamepad->axisRY(), -511, 512, 255, -255);
              input_c_L = map(myGamepad->axisY(), -511, 512, 255, -255);
              
             

            // You can query the axis and other properties as well. See Gamepad.h
            // For all the available functions.
        }
    

    delay(20);
    //read input from controller and put it into the variable input_c_L and input_c_R
  }
  
}


/*
 * Ramp function
 */
const int ramper = 3;
int CurrVelocity_R [ramper] = {0,0,0};
int CurrAcceleration_R [ramper] = {0,0,0};
int CurrJerk_R [ramper] = {0,0,0};

int ptVelocity_R = 0;
int ptAcceleration_R = 0;
int ptJerk_R = 0;

int MeanVelocity_R = 0;
int MeanAcceleration_R = 0;
int MeanJerk_R = 0;

int CurrVelocity_L [ramper] = {0,0,0};
int CurrAcceleration_L [ramper] = {0,0,0};
int CurrJerk_L [ramper] = {0,0,0};

int ptVelocity_L = 0;
int ptAcceleration_L = 0;
int ptJerk_L = 0;

int MeanVelocity_L = 0;
int MeanAcceleration_L = 0;
int MeanJerk_L = 0;

int SmoothVelocity_R(int Velocity_new_R){
  MeanVelocity_R = MeanVelocity_R + Velocity_new_R - CurrVelocity_R[ptVelocity_R];
  CurrVelocity_R[ptVelocity_R] = Velocity_new_R;
  ptVelocity_R = (ptVelocity_R + 1) % ramper;
  return SmoothAcceleration_R(MeanVelocity_R / ramper);
}//here we are doing the first pass of smoothing the instant accerlation by taking a moving average
//all three functions do the same thing, they just create a more smooth curve each time
//they take the mean of the last 6 values and print that to the next function
//this way accerlation is smoother

int SmoothAcceleration_R(int Acceleration_new_R){
  MeanAcceleration_R = MeanAcceleration_R + Acceleration_new_R - CurrAcceleration_R[ptAcceleration_R];
  CurrAcceleration_R[ptAcceleration_R] = Acceleration_new_R;
  ptAcceleration_R = (ptAcceleration_R + 1) % ramper;
  return SmoothJerk_R(MeanAcceleration_R / ramper);
}

int SmoothJerk_R(int Jerk_new_R){
  MeanJerk_R = MeanJerk_R + Jerk_new_R - CurrJerk_R[ptJerk_R];
  CurrJerk_R[ptJerk_R] = Jerk_new_R;
  ptJerk_R = (ptJerk_R + 1) % ramper;
  return MeanJerk_R / ramper;
}

int SmoothVelocity_L(int Velocity_new_L){
  MeanVelocity_L = MeanVelocity_L + Velocity_new_L - CurrVelocity_L[ptVelocity_L];
  CurrVelocity_L[ptVelocity_L] = Velocity_new_L;
  ptVelocity_L = (ptVelocity_L + 1) % ramper;
  return SmoothAcceleration_L(MeanVelocity_L / ramper);
}//here we are doing the first pass of smoothing the instant accerlation by taking a moving average
//all three functions do the same thing, they just create a more smooth curve each time
//they take the mean of the last 6 values and print that to the next function
//this way accerlation is smoother

int SmoothAcceleration_L(int Acceleration_new_L){
  MeanAcceleration_L = MeanAcceleration_L + Acceleration_new_L - CurrAcceleration_L[ptAcceleration_L];
  CurrAcceleration_L[ptAcceleration_L] = Acceleration_new_L;
  ptAcceleration_L = (ptAcceleration_L + 1) % ramper;
  return SmoothJerk_L(MeanAcceleration_L / ramper);
}

int SmoothJerk_L(int Jerk_new_L){
  MeanJerk_L = MeanJerk_L + Jerk_new_L - CurrJerk_L[ptJerk_L];
  CurrJerk_L[ptJerk_L] = Jerk_new_L;
  ptJerk_L = (ptJerk_L + 1) % ramper;
  return MeanJerk_L / ramper;
}
