int potPin = A1; // Potentiometer output connected to analog pin
float potVal = 0; // Variable to store the input from the potentiometer


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  potVal = analogRead(potPin);   // read the potentiometer value at the input pin
  Serial.println(potVal);
  //now map the range to range 0 to 2
  //can be accomplished by dividing by (maxValue/2)
  //then multiply into the left motor
  //right motor should get 2-potVal


  //for speed map to 0 to 1
  //divide by max Value
}
