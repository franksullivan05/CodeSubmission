/** @file   drive.ino
    @author A. Warburton, F. Sullivan
    @date   12 OCTOBER 2015
    @brief  Header file for all DRIVE-based activities
*/

#define DC_LEFT_PIN          2
#define DC_RIGHT_PIN         3

/* Speed of honing (%). */
#define SLOW_SPEED           30

/* Speed defines. */
#define SPEED                50
#define TURN_SPEED           25
#define MED_SPEED            40

/* Desired distance between the IR sensors and the wall for the wall following algorithm. */
#define SENSOR_MID_POINT     50
#define KP                   1

#define WALLBOUNCEDIST       30

/* Timer to trigger downward flap movement (ms). */
#define FLAPDOWNTIMER        30

/* Servo objects for left and right tracks. */
Servo DCmotor_LEFT;
Servo DCmotor_RIGHT;

/* Error for PID control for left wall follow (CW). */
float right_sensor_error;
float left_sensor_error;

/* PWM signals sent to main drive motors. */
float wallFollowLeft = 0;
float wallFollowRight = 0;
float wallBounceLeft = 0;
float wallBounceRight = 0;

/* Pivot angle that define how much the robot needs to turn to line up the weight. */
long pivot_angle = 0;

/* ALL wall bouncing variables. */
int bounceTriggered = 0;
unsigned long bounceTimer = 0;
unsigned long curveTimer = 500;
int curveState = 0;
int bounceState = 0;
int wallFollowDir;
unsigned int rotateDir;

/** A function that sets-up and initialises all pins corresponding to drive motors */
void driveSetup (void)
{
  /* Attach DC motors to servo objects. */
  DCmotor_LEFT.attach(DC_LEFT_PIN);
  DCmotor_RIGHT.attach(DC_RIGHT_PIN);
}


/** Limits the PWM speed of the main drive motors between -25 (65) and 25 (115)
    @param  the current angle of the motor (left or right)
    @return the new limited angle of the desired motor  */
float limited_speed (float motor_angle)
{
  if (motor_angle <= -SPEED) {
    return -SPEED;
  }
  else if (motor_angle >= SPEED) {
    return SPEED;
  }
  
  return motor_angle;  
}


/** Takes the drive speed on a -100% to +100% scale and drives the motors with these speeds
    @param  the desired speed to be written
    @param  the desired side of the robot to drive (1 = LEFT  0 = RIGHT)  */
void driveScaled (float desiredSpeed, int driveSide)
{
  float translatedSpeed = limited_speed (desiredSpeed);
  translatedSpeed = map (translatedSpeed, -100, +100, 180, 0);

  if (driveSide) {
    DCmotor_LEFT.write((int) translatedSpeed);
  }
  else {
    DCmotor_RIGHT.write((int) translatedSpeed);
  }
}


/** Returns a random integer between the two desirded values
    &param  Lower bound
    &param  Upper Bound
    @return Unsigned random integer between the full range.  */
unsigned int randomInt (int minRange, int maxRange) 
{
  long actualRand = random (minRange, maxRange + 1);
  
  if (actualRand > 0.5) 
  {
    return actualRand + 0.5;
  }
  return actualRand - 0.5;    
}


/** Simple, crude P-control to follow the wall clockwise
    @param  the sensory data structure to create error */
void calculateWallFollowSpeeds (struct sensor_reads currSensors)
{
  right_sensor_error = currSensors.IRRight - SENSOR_MID_POINT;
  
  wallFollowLeft = SPEED - KP * right_sensor_error;
  wallFollowRight = SPEED + KP * right_sensor_error;
  
  if (currSensors.IRRight < 40) { // Rotate on the spot if too close to a wall/object
    wallFollowRight = -SPEED;
    wallFollowLeft = SPEED;
  }
  else if (currSensors.IRRight > 100) {
    wallFollowRight = SPEED;
    wallFollowLeft = 0;
  }
}

/** A simple bouncing algorithm to a random pivot angle if the wall is too close
    @param  the sensory data structure to create error */
void determineBounce (struct sensor_reads currSensors)
{
  unsigned long rotateAngle;
    
  int bounceTriggeredCurrent = bounceTriggered;
  //long currentTime = millis ();  
  
  if (currSensors.IRLeft < WALLBOUNCEDIST || currSensors.IRRight < WALLBOUNCEDIST || !currSensors.detectLeft || !currSensors.detectRight)
  {
    // Too close, PIVOT!
    bounceTriggered = 1;
  }
  else if (!currSensors.detectFront)
  {
    bounceTriggered = 2;
  }
  else
  {
    bounceTriggered = 0;
  }

  if (bounceTriggeredCurrent == 0)
  {
    if (bounceTriggered == 1)
    {
      // If the wall has been newly detected...
      rotateAngle = randomInt (0, 20);
  //    rotateDir = randomInt (0, 1); // 1 = LEFT; 0 = RIGHT;
      if (currSensors.IRLeft < WALLBOUNCEDIST || !currSensors.detectRight) {
        rotateDir = 1;
      }
      else {
        rotateDir = 0;
      }
      bounceTimer = millis() + degrees_to_delay (rotateAngle);
      bounceState = 1;
    }
    else if (bounceTriggered == 2)
    {
      // If the wall has been newly detected...
      rotateAngle = 30;
  //    rotateDir = randomInt (0, 1); // 1 = LEFT; 0 = RIGHT;
      if (currSensors.IRLeft < currSensors.IRRight) {
        rotateDir = 1;
      }
      else {
        rotateDir = 0;
      }
      bounceTimer = millis() + degrees_to_delay (rotateAngle);
      bounceState = 1;
    }
  }
  
  if (bounceState == 1 && (millis() > bounceTimer))
  {
    // If the random pivot timer has elapsed
    bounceState = 0;
    bounceTriggered = 0;
  }

  if (millis() > curveTimer)
  {
    curveTimer = millis() + 1000;
    curveState = !curveState;
  }
  Serial.println(curveTimer);
  
  if (bounceState == 0)
  {
    // Drive forward at <100% speed
    if (curveState) {
      wallBounceLeft = 20;
      wallBounceRight = 50;
    }
    else {
      wallBounceLeft = 50;
      wallBounceRight = 20;
    } 
  }
  
  else if (bounceState == 1)
  {
    if (rotateDir)
    {
      // Random Rotate LEFT
      wallBounceLeft = -SPEED;
      wallBounceRight = +SLOW_SPEED;
    }
    else
    {
      // Random Rotate RIGHT
      wallBounceLeft = +SLOW_SPEED;
      wallBounceRight = -SPEED;
    }
  }
}

/** Main drive function called to update the main drive speeds
    @param  the sensor structure holding current sensor values  */
void updateDrive (struct sensor_reads currSensors)
{
  int currState = checkCurrentState();
  
  if (currState == 1)
  {
    // Implement random searching algorithm
    determineBounce(currSensors);
    driveScaled (wallBounceLeft, 1);
    driveScaled (wallBounceRight, 0);
//    // Implement wall follow home movement
//    calculateWallFollowSpeeds (currSensors);
//    driveScaled (wallFollowLeft, 1);
//    driveScaled (wallFollowRight, 0);  
  }
  
  else if (currState == 2)
  {
    // Implement PIVOT LEFT movement
    driveScaled (TURN_SPEED, 1);
    driveScaled (-TURN_SPEED, 0);
  }
  
  else if (currState == 3)
  {
    // Implement PIVOT RIGHT movement
    driveScaled (-TURN_SPEED, 1);
    driveScaled (TURN_SPEED, 0);
  }
  
  else if (currState == 4)
  {
    // Impliment SLOW FORWARD movement
    driveScaled (SLOW_SPEED, 1);
    driveScaled (SLOW_SPEED, 0);
  }
  
  else if (currState == 5)
  {
    // Impliment STOP
    driveScaled (0, 0);
    driveScaled (0, 1);   
  }
  
  else if (currState == 6)
  {
    // Implement PIVOT LEFT movement
    driveScaled (TURN_SPEED, 1);
    driveScaled (-TURN_SPEED, 0);
  }
  
  else if (currState == 7)
  {
    // Implement PIVOT RIGHT movement
    driveScaled (-TURN_SPEED, 1);
    driveScaled (TURN_SPEED, 0);
  }

  else if (currState == 8)
  {
    // Implement wall follow home movement
    calculateWallFollowSpeeds (currSensors);
    driveScaled (wallFollowLeft, 1);
    driveScaled (wallFollowRight, 0);  
  }

  else if (currState == 9)
  {
    // Implement PIVOT RIGHT movement
    driveScaled (MED_SPEED, 1);
    driveScaled (-MED_SPEED, 0);
  }

  else if (currState == 10)
  {
    // Reverse slowly into base
    driveScaled (-MED_SPEED, 1);
    driveScaled (-MED_SPEED, 0);
  }

  else if (currState == 11)
  {
    // Fly outta base like a mofo
    driveScaled (SPEED,1);
    driveScaled (SPEED,0);
    flapMovement(0);
    setFlapTime (millis() + FLAPDOWNTIMER);
  }
  
}
