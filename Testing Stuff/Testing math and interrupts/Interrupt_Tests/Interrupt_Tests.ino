/*
 * This code was a test to make sure that the interrupt works
 * the problem was that it was happening too fast
 * dont make the timer 4 interrupt less than 50 ms
 * 
 * 
 * this program does not talk to the motor controllers at all
 */
int input_c_R = 0;
int input_c_L = 0;
int output_R = 0;
int output_L = 0;


void setup() {

  // put your setup code here, to run once:
  cli();//stop interrupts

  //set timer4 interrupt at 10Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  
  // set compare match register for 10hz increments
  OCR4A = 12499/1;// = (16*10^6) / (100*64) - 1 (must be <65536)
  
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 64 prescaler
  TCCR4B |= (1 << CS41) | (1 << CS40);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);

  sei();//allow interrupts

}
/*
 * Main loop
*/
void loop() {
  // put your main code here, to run repeatedly:


  for (int i = 0; i <= 255; i++){//just change the input to hard coded values and then wait for the interrupt to happen twice
  input_c_R = i;
  delay(100);
  }
  
}

ISR(TIMER4_COMPA_vect){//timer 4 interrupts every 50ms
  output_R = SmoothVelocity_R(input_c_R); //calculate the output values and just print to console with the input value
  output_L = SmoothVelocity_L(input_c_L);
  Serial.print("Input: ");
  Serial.println(input_c_R);
  Serial.print("Output: ");
  Serial.println(output_R);
 
}

/*
 * Ramp function
 */
 
int CurrVelocity_R [6] = {0,0,0,0,0,0};
int CurrAcceleration_R [6] = {0,0,0,0,0,0};
int CurrJerk_R [6] = {0,0,0,0,0,0};

int ptVelocity_R = 0;
int ptAcceleration_R = 0;
int ptJerk_R = 0;

int MeanVelocity_R = 0;
int MeanAcceleration_R = 0;
int MeanJerk_R = 0;

int CurrVelocity_L [6] = {0,0,0,0,0,0};
int CurrAcceleration_L [6] = {0,0,0,0,0,0};
int CurrJerk_L [6] = {0,0,0,0,0,0};

int ptVelocity_L = 0;
int ptAcceleration_L = 0;
int ptJerk_L = 0;

int MeanVelocity_L = 0;
int MeanAcceleration_L = 0;
int MeanJerk_L = 0;

int SmoothVelocity_R(int Velocity_new_R){
  MeanVelocity_R = MeanVelocity_R + Velocity_new_R - CurrVelocity_R[ptVelocity_R];
  CurrVelocity_R[ptVelocity_R] = Velocity_new_R;
  ptVelocity_R = (ptVelocity_R + 1) % 6;
  return SmoothAcceleration_R(MeanVelocity_R / 6);
}//here we are doing the first pass of smoothing the instant accerlation by taking a moving average
//all three functions do the same thing, they just create a more smooth curve each time
//they take the mean of the last 6 values and print that to the next function
//this way accerlation is smoother

int SmoothAcceleration_R(int Acceleration_new_R){
  MeanAcceleration_R = MeanAcceleration_R + Acceleration_new_R - CurrAcceleration_R[ptAcceleration_R];
  CurrAcceleration_R[ptAcceleration_R] = Acceleration_new_R;
  ptAcceleration_R = (ptAcceleration_R + 1) % 6;
  return SmoothJerk_R(MeanAcceleration_R / 6);
}

int SmoothJerk_R(int Jerk_new_R){
  MeanJerk_R = MeanJerk_R + Jerk_new_R - CurrJerk_R[ptJerk_R];
  CurrJerk_R[ptJerk_R] = Jerk_new_R;
  ptJerk_R = (ptJerk_R + 1) % 6;
  return MeanJerk_R / 6;
}

int SmoothVelocity_L(int Velocity_new_L){
  MeanVelocity_L = MeanVelocity_L + Velocity_new_L - CurrVelocity_L[ptVelocity_L];
  CurrVelocity_L[ptVelocity_L] = Velocity_new_L;
  ptVelocity_L = (ptVelocity_L + 1) % 6;
  return SmoothAcceleration_L(MeanVelocity_L / 6);
}//here we are doing the first pass of smoothing the instant accerlation by taking a moving average
//all three functions do the same thing, they just create a more smooth curve each time
//they take the mean of the last 6 values and print that to the next function
//this way accerlation is smoother

int SmoothAcceleration_L(int Acceleration_new_L){
  MeanAcceleration_L = MeanAcceleration_L + Acceleration_new_L - CurrAcceleration_L[ptAcceleration_L];
  CurrAcceleration_L[ptAcceleration_L] = Acceleration_new_L;
  ptAcceleration_L = (ptAcceleration_L + 1) % 6;
  return SmoothJerk_L(MeanAcceleration_L / 6);
}

int SmoothJerk_L(int Jerk_new_L){
  MeanJerk_L = MeanJerk_L + Jerk_new_L - CurrJerk_L[ptJerk_L];
  CurrJerk_L[ptJerk_L] = Jerk_new_L;
  ptJerk_L = (ptJerk_L + 1) % 6;
  return MeanJerk_L / 6;
}
