/** @file   servos.ino
    @author A. Warburton, F. Sullivan
    @date   12 OCTOBER 2015
    @brief  Header file for all PICKUP-based activities
*/


#define SWEEP_FRONT_ID       1
#define SWEEP_LEFT_ID        3
#define SWEEP_RIGHT_ID       4

#define SERVO_LEFT_PIN       4
#define SERVO_RIGHT_PIN      5

Servo Servo_LEFT;
Servo Servo_RIGHT;

unsigned long blokingTimer = 0;
int sweepState = 0;

/* Timer to trigger downward flap movement (ms). */
unsigned long triggerDownFlap = 0;

/** A function that sets-up and initialises all pins corresponding to smart servos */
void servoSetup (void) 
{
  /* Attach the servos for the storage flap. */
  Servo_LEFT.attach(SERVO_LEFT_PIN);
  Servo_RIGHT.attach(SERVO_RIGHT_PIN);
  Servo_LEFT.write(75);
  Servo_RIGHT.write(105);
  
  /* Begin serial communication on port 2. */
  Herkulex.beginSerial2(115200);
  Herkulex.reboot(SWEEP_FRONT_ID);
  Herkulex.reboot(SWEEP_RIGHT_ID);
  Herkulex.reboot(SWEEP_LEFT_ID);
  delay(500);
  Herkulex.initialize();

  /* Move smart servos to initial position (-10 deg. = centre). */
  Herkulex.moveOneAngle(SWEEP_FRONT_ID, -10, 1000, 1); // -10 dead centre
  Herkulex.moveOneAngle(SWEEP_RIGHT_ID, -5, 1000, 1); // -5 dead centre
  Herkulex.moveOneAngle(SWEEP_LEFT_ID, -5, 1000, 1); // -5 dead centre
  delay (500);
}


/** Function that drop the storage flap, servos co-ordinated
    @param  The direction you want to drop it off (DOWN = 0; UP = UP).  */
void flapMovement (int dir)
{
  if (dir)
  {
    Servo_LEFT.write(75);
    Servo_RIGHT.write(105);
  } 
  else
  {
    Servo_LEFT.write(20);
    Servo_RIGHT.write(160);
  }
}    


/** The pick up sequence of the pickup mechinism, where each smart servo is triggered
    NOTE: This is needing to be extended once the back servos are attached
    @param  The side of the robot that the pickup uses (1 = LEFT 0 = RIGHT)    */
void servoSequence (int side)
{
  if (side) { 
    // LEFT side defined
    Herkulex.moveOneAngle (SWEEP_FRONT_ID, -140, 2000, LED_BLUE);
    delay(2000);
    Herkulex.moveOneAngle (SWEEP_LEFT_ID, -135, 2000, LED_BLUE);
    delay(2000);
    Herkulex.moveOneAngle (SWEEP_LEFT_ID, -5, 2000, LED_BLUE);
    delay(2000);
    LEDTemp3Set(1);
    // delays and move other sweepers
  }
  else { 
    // RIGHT side defined
    Herkulex.moveOneAngle (SWEEP_FRONT_ID, 120, 2000, LED_BLUE);
    delay(2000);
    Herkulex.moveOneAngle (SWEEP_RIGHT_ID, 125, 2000, LED_BLUE);
    delay(2000);
    Herkulex.moveOneAngle (SWEEP_RIGHT_ID, -5, 2000, LED_BLUE);
    delay(2000);
    LEDTemp3Set(1);
    // delays and move other sweepers
  }
  setCurrentState(1);
}


/** Calling the servo motor to hoist the NZ flag  */
void specialSequence (void)
{
  Herkulex.moveOneAngle (SWEEP_RIGHT_ID, 110, 1000, LED_BLUE);
  delay(1000);
  Herkulex.moveOneAngle (SWEEP_RIGHT_ID, 70, 500, LED_BLUE);
  delay(500);
  Herkulex.moveOneAngle (SWEEP_RIGHT_ID, 90, 500, LED_BLUE);
  delay(500);
  setCurrentState(1);
}

void setSweepState (int state)
{
  sweepState = state;
}


void setFlapTime (unsigned long t)
{
  triggerDownFlap = t;
}


/** Function to initate sweeper action using blocking timer
    @param  direction of movement   */    
void initiateSweep (int side)
{
  if ((sweepState == 1))
  {
    //Serial.println ("1");
    blokingTimer = millis() + 400;
    if (side)
    {
      Herkulex.moveOneAngle (SWEEP_FRONT_ID, -140, 600, LED_BLUE);
    }
    else
    {
      Herkulex.moveOneAngle (SWEEP_FRONT_ID, 120, 600, LED_BLUE);
    }
    sweepState = 2;
  }
  
  else if ((sweepState == 2) && (millis() >= blokingTimer))
  {
    //Serial.println ("2");
    if (side)
    {
      Herkulex.moveOneAngle (SWEEP_LEFT_ID, -125, 500, LED_BLUE);
    }
    else
    {
      Herkulex.moveOneAngle (SWEEP_RIGHT_ID, 125, 500, LED_BLUE);
    }
    blokingTimer = millis() + 500;
    sweepState = 3;
  }
  
  else if ((sweepState == 3) && (millis() >= blokingTimer))
  {
    //Serial.println ("3");
    Herkulex.moveOneAngle(SWEEP_FRONT_ID, -10, 500, 1); // -10 dead centre
    if (side)
    {
      Herkulex.moveOneAngle(SWEEP_LEFT_ID, -5, 500, 1); // -5 dead centre
    }
    else
    {
      Herkulex.moveOneAngle(SWEEP_RIGHT_ID, -5, 500, 1); // -5 dead centre
    }
    blokingTimer = millis() + 500;
    sweepState = 0;
    resetPickupTriggered (0);
  }
  
}

/** Main servo function called to update the smart servo positions
    @param  the sensor structure holding current sensor values  */
void updateServos (struct sensor_reads currSensors)
{
  int currState = checkCurrentState();

  if (checkPickupTriggered() == 1) 
  {
    initiateSweep(0);
  }
  else if (checkPickupTriggered() == 2)
  {
    initiateSweep(1); 
  }
  
  else if (currState == 99) 
  {
    // Call flag to go UP
    specialSequence();
  }
  
  if (millis() >= triggerDownFlap)
  {
    // Flap goes down after time
    flapMovement(1);
  }
}




