// Copyright (c) 2020, Christian Brothers University
// All rights reserved.
// This program may be freely copied and modified provided that the above copyright is included.

#include <CytronMotorDriver.h>  //library for MDD10A

#define pwm 3  //define digital pin connected to the MDD10A for pulse width modulation.
#define dir 6 //define digital pin connected to the MDD10A for direction
#define redForward A2 //define analog pin for button going forwards.
#define blackReverse A5 //define analog pin for button going backwards

int indexForward = 0; //initialize indexing variable for Forward.
int indexReverse = 0; //initialize indexing variable for Reverse.

int lowvalue = 300;  //Voltages below 3.52-V are considered low (12-V system) of (300/1023)*12 = 3.52.
int highvalue = 900; //Voltages above 10.55-V are considered high (12-V system) of (900/1023)*12 = 10.55.

int td = 40;// this  defines the time between each iteration of a loop for the PWM and controls acceleration.
int td_system = 10; //system delay 

int topspeedF = 150;  // this defines the top speed Forward and reduces speed by 150/255
int topspeedR = 150;  // this defines the top speed Reverse and reduces speed by 150/255

int forwardValue; //define variable for voltage from existing controller
int reverseValue; //define variable for voltage from existing controller

CytronMD motor(PWM_DIR, pwm, dir); // set the arguments for the MDD10A 

void setup()
{
  pinMode(pwm, OUTPUT); //set motor as output
  pinMode(dir, OUTPUT);
  pinMode(redForward, INPUT); //set buttons as inputs
  pinMode(blackReverse, INPUT);
  Serial.begin(9600); //set baud rate for communications
  motor.setSpeed(0);
}
void loop()
{
  delay(td_system); //delay to ensure systen is in steady state 

  forwardValue = analogRead(redForward); //reading the analog input coming from the car controller and define variables
  reverseValue = analogRead(blackReverse); //reading the analog input coming from the car controller and define variables

  // -------------------------------------------------------------------------------------------------------------------------------
  // ACCELERATOR PEDAL NOT PRESSED, or just released

  if ((forwardValue < lowvalue && reverseValue < lowvalue) || (forwardValue > highvalue && reverseValue > highvalue)) //condition for when both buttons are high or low: insure that speed is zero 

  {
    if (indexForward > 0) {
      for (indexForward; indexForward > 0; indexForward --)
      {
        analogWrite(pwm, indexForward);
        delay(td);
        
      }
    }
    if (indexReverse > 0) {
      for (indexReverse; indexReverse > 0; indexReverse --)
      {
        analogWrite(pwm, indexReverse);
        delay(td);
      }
    }
    indexReverse = 0;
    indexForward = 0;
  }

  // -------------------------------------------------------------------------------------------------------------------------------
  //FORWARD DIRECTION

  if (forwardValue > highvalue && reverseValue < lowvalue) //condition for when button for forward is high and reverse is low
  {
    indexReverse = 0;

    digitalWrite(dir, LOW); // set direction of the motor
    for (indexForward; indexForward <= topspeedF; indexForward++) // slow acceleration function to top forward speed
    {
      analogWrite(pwm, indexForward); //give the corresponding velocity value to the motor
      delay(td); //delay for every time the velocity is increased

      forwardValue = analogRead(redForward); //reread the button to make sure it is still high
      reverseValue = analogRead(blackReverse); //reread the button to make sure it is still low
      if (forwardValue < lowvalue || reverseValue > highvalue)  // if one the buttons change states, leave the loop and reducing the speed to zero 
      {
        for (indexForward; indexForward > 0; indexForward--) {
          analogWrite(pwm, indexForward);
          delay(td);
          reverseValue = analogRead(blackReverse);
          forwardValue = analogRead(redForward);
        }
        break;
      }
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------
  //REVERSE DIRECTION

  if (forwardValue < lowvalue && reverseValue > highvalue)  //condition for when button reverse is high and forward is low
  {
    indexForward = 0;

    digitalWrite(dir, HIGH); //set direction of the motor
    for (indexReverse; indexReverse <= topspeedR; indexReverse++) //slow acceleration to top reverse speed
    {
      analogWrite(pwm, indexReverse); //give the corresponding velocity value to the motor
      delay(td); //delay for every time the velocity is increased
      forwardValue = analogRead(redForward);//reread the button to make sure it is still low
      reverseValue = analogRead(blackReverse); //reread the button to make sure it is still high
      if (reverseValue < lowvalue || forwardValue > highvalue) { // if one the buttons change states, leave the loop and reducing the speed to zero 
        for (indexReverse; indexReverse > 0; indexReverse--) {
          analogWrite(pwm, indexReverse);
          delay(td);
          forwardValue = analogRead(redForward);
          reverseValue = analogRead(blackReverse);
        }
        break;
      }
    }
  }
}
