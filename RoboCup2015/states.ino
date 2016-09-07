/** @file   states.ino
    @author A. Warburton, F. Sullivan
    @date   12 OCTOBER 2015
    @brief  Header file for all STATE-based activities
*/


/* Ignition and emergency stop pins. */
#define MASTER_SWITCH_PIN    42
#define PONTENTIOMETER_PIN   A0

/* LED pins. */
#define LED_LEFT             28
#define LED_RIGHT            29

/* Temporary LED's on the top circuit board for HQ and weight identification. */
#define LED_TEMP1            26
#define LED_TEMP2            27
#define LED_TEMP3            28

/* Watchdog timer to limit creep forward state for a desired time. */
unsigned long forwardTimer = 0;
unsigned long endForwardTimer = 0; 

/* Watchdog timer to pivot a certain angle. */
unsigned long pivotTimer = 0;
unsigned long startPivotTimer = 0;
unsigned long endPivotTimer = 0;

/* Time of slow forward state (ms). */
#define CREEP_TIME          5000

/* Time of base-drop off slow forward state (ms). */
#define BASE_CREEP_TIME     1000

/* State variable for entire state machine. */
int overallState = 0;

/* Variable to show if a weight is in the process of pick-up. */
int pickupTriggered = 0;   // 0 = NOT triggered, 1 = LEFT triggered, 2 = RIGHT triggered

/* Variable to represent how many weights are currently on board. */
int weights_onboard = 0;


unsigned long watchdogLast = 30000;

/** A function that sets-up and initialises all pins corresponding to state-defining inputs/outputs */
void stateSetup (void)
{
  /* Button pin setup. */
  pinMode(MASTER_SWITCH_PIN, INPUT);
  pinMode(PONTENTIOMETER_PIN, INPUT);
  
  /* LED pin setup. */
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(LED_TEMP1, OUTPUT);
  pinMode(LED_TEMP2, OUTPUT);
  pinMode(LED_TEMP3, OUTPUT);
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);  
  digitalWrite(LED_TEMP1, LOW);
  digitalWrite(LED_TEMP2, LOW);
  digitalWrite(LED_TEMP3, LOW);
}


/** Determines the current state of the master switch 
    @return true if the switch is down (on)  */
int checkMasterSwitch (void)
{
  return digitalRead (MASTER_SWITCH_PIN);
}


/** Checks the potentiometer pin and maps it to the desired values
    @param  lower limit of the output reading
    @param  upper limit of the output reading
    @return the current potentiometer state when mapped  */
int checkPotentiometer (int lowerLimit, int Upperlimit)
{
  return map ((float) analogRead (PONTENTIOMETER_PIN), 0, 975, lowerLimit, Upperlimit);
}


/** Turns the Left LED to the desired output
    @param  High or low  */
void LEDLeftSet (int setVal)
{
  if (setVal)
    digitalWrite(LED_LEFT, HIGH);
  else
    digitalWrite(LED_LEFT, LOW);
}


/** Turns the Right LED to the desired output
    @param  High or low  */
void LEDRightSet (int setVal)
{
  if (setVal)
    digitalWrite(LED_RIGHT, HIGH);
  else
    digitalWrite(LED_RIGHT, LOW);
}

void LEDTemp1Set (int setVal)
{
  if (setVal)
    digitalWrite(LED_TEMP1, HIGH);
  else
    digitalWrite(LED_TEMP1, LOW);
}

void LEDTemp2Set (int setVal)
{
  if (setVal)
    digitalWrite(LED_TEMP2, HIGH);
  else
    digitalWrite(LED_TEMP2, LOW);
}

void LEDTemp3Set (int setVal)
{
  if (setVal)
    digitalWrite(LED_TEMP3, HIGH);
  else
    digitalWrite(LED_TEMP3, LOW);
}


/** Finds out the current state of the overall state machine
    @return the overall state machine variable   */
int checkCurrentState (void)
{
  return overallState;
}


/** Changes the global state machine variable
    @param  the desired state to be changed to   */
int setCurrentState (int state)
{
  overallState = state;
}


/** Simple fuction to reset the state pickup variable 
    @param  the value you want to set it to.  */
void resetPickupTriggered (int state)
{
  pickupTriggered = state;
}

/** Simply checks the current global pickup varaible
    @return variable   */
int checkPickupTriggered (void)
{
  return pickupTriggered;
}


/** Convert a degrees value to a ms delay time
    @param  the desired angle (deg.) 
    @return the delay time (ms)      */
int degrees_to_delay (float deg) 
{
  if (deg > -180 && deg < 180) {
    return 24.356 * abs(deg) + 272.41;
  }
  return 0;
}


/** Calculating the angle needed to pivot (deg) depending on the weight found 
    @param  the weight structure (left or right)
    @return the degreeds needing to turn around the COM to line up the weight  */
float pivotTrig (long sonarReading) 
{
  float numerator = sin (DEG_TO_RAD * 60) * sonarReading - 
                    sin (DEG_TO_RAD * 40) * 20;

  float denominator = cos (DEG_TO_RAD * 60) * sonarReading + 
                      cos (DEG_TO_RAD * 40) * 20;
  return atan (numerator / denominator);
}


/** Uses trigonometry to calculate the pivot angle to hone into the weight
    @param  the current sensor structure
    @param  the direction wanted to pivot (1 = LEFT  0 = RIGHT)
    @return the end time (ms.) where the watchdog timer will finish  */
unsigned long calculatePivot (struct sensor_reads currSensors, int spinDirection)
{
  if (spinDirection)
  {
    // Use LEFT sonar pivot
    return startPivotTimer + degrees_to_delay (pivotTrig (currSensors.sonarLeft));
  }
  // Use RIGHT sonar pivot
  return startPivotTimer + degrees_to_delay (pivotTrig (currSensors.sonarRight)); 
}


/** Changes between overall state variables and changes Mealy outputs
    @param  the sensor structure
    @param  the weight detection structure  */
void updateStates (struct sensor_reads currSensors, struct weight_detect currWeights)
{
  int currentState = checkCurrentState();

//  Serial.print("State: ");
//  Serial.print(currentState);
//  Serial.print(" Weights onboard: ");
//  Serial.print(weights_onboard);
//  Serial.print(" Green base: ");
//  Serial.print(currSensors.greenBase);
//  Serial.print(" Blue base: ");
//  Serial.println(currSensors.blueBase);

//  Serial.print(currSensors.greenBase);
//  Serial.println(currSensors.blueBase);

//  Serial.println((float) analogRead (PONTENTIOMETER_PIN));
//
//  Serial.print("IR Left: ");
//  Serial.print(currSensors.IRLeft);
//  Serial.print(" Sonar Left: ");
//  Serial.print(currSensors.sonarLeft);
//  Serial.print(" IR Right: ");
//  Serial.print(currSensors.IRRight);
//  Serial.print(" Sonar Right: ");
//  Serial.println(currSensors.sonarRight);
  
  int switch_on = (!checkMasterSwitch() == 0);
  
  if (switch_on)
  {

    if (millis() >= watchdogLast && (overallState == 1))
    {
      startPivotTimer = millis();
      endPivotTimer = startPivotTimer + degrees_to_delay(45);
      overallState = 6;
    }
    
    if (currentState == 4 && (currSensors.IRLeft < 25  || currSensors.IRRight < 25 || (millis() > endForwardTimer)))
    {
      overallState = 1;
      watchdogLast = millis() + 20000;
    }

    // If 3 weights detected to be on board
    if (weights_onboard > 2 && currentState == 1) // Replace with interior IR's
    {
      overallState = 8; // Wall follow home
    }

    if (currentState == 1 || currentState == 4)
    {
      if (currWeights.weightLeft && (currentState != 2 && currentState != 3))
      {
        // Weight on LEFT has been NEWLY found
        startPivotTimer = millis();
        endPivotTimer = calculatePivot (currSensors, 1);
//        endPivotTimer = startPivotTimer + degrees_to_delay (2);
        overallState = 2;
        if ((currSensors.IRLeft < 15 && currSensors.IRLeft > 4) || (currSensors.IRRight < 15 && currSensors.IRRight > 4))
        {
          overallState = 1;
          watchdogLast = millis() + 20000;
        }
      }
      
      else if (currWeights.weightRight && (currentState != 3 && currentState != 2))
      {
        // Weight on RIGHT has been NEWLY found
        startPivotTimer = millis();
        endPivotTimer = calculatePivot(currSensors, 0);
//        endPivotTimer = startPivotTimer + degrees_to_delay (2);
        overallState = 3;
        if ((currSensors.IRLeft < 15 && currSensors.IRLeft > 4)  || (currSensors.IRRight < 15 && currSensors.IRRight > 4))
        {
          overallState = 1;
          watchdogLast = millis() + 20000;
        }
      }
    }
        
    else if (currentState == 5)
    {
      // Switch has been turned ON
      overallState = 1;
    }
    
    else if (((currentState == 2) || (currentState == 3)) && millis() >= endPivotTimer)
    {
      // The pivot timer has timed out
      overallState = 4; // meant to be 4, changed to 1 for testing
      endForwardTimer = millis() + CREEP_TIME;
    }
        
    else if ((currentState == 6) && millis() >= endPivotTimer)
    {
      // Left detect pivot timed out.
      overallState = 8;
    }
    
    else if ((currentState == 7) && millis() >= endPivotTimer)
    {
      // Right detect pivot timed out.
      overallState = 8;
    }

    else if ((currentState == 9) && millis() >= endPivotTimer)
    {
      // Home base rotatation finished
      overallState = 10;
    }

    else if (currentState == 10 && (!currSensors.limitLeft || !currSensors.limitRight))
    {
      endForwardTimer = millis() + 1000;
      overallState = 11;
    }

    else if (currentState == 11 && millis() >= endForwardTimer)
    {
      weights_onboard = 0;
      overallState = 1;
      watchdogLast = millis() + 20000;
    }

    // If wall following home
    else if (currentState == 8)
    {
      // 45 degree turns when wall following home
      if (!currSensors.detectLeft)
      {
        // IR detect left triggered
        startPivotTimer = millis();
        endPivotTimer = startPivotTimer + degrees_to_delay(45);
        overallState = 6;
      }
      else if ((!currSensors.detectRight || !currSensors.detectFront))
      {
        // IR detect right triggered
        startPivotTimer = millis();
        endPivotTimer = startPivotTimer + degrees_to_delay(45);
        overallState = 7;
      }
      
      if (currSensors.blueBase)
      // Rotate 180 degrees when base found
      {
        startPivotTimer = millis();
        endPivotTimer = startPivotTimer + degrees_to_delay(65);
        overallState = 9;
      }
    }
  }

  else 
  {
    overallState = 5;
  }
  
  
  if (!currSensors.detectWeightLeft && pickupTriggered == 0 && currentState != 8)
  {
    // Initiate pick-up Sequence LEFT
    pickupTriggered = 1;
    setSweepState (1);
    weights_onboard++;
  }
  else if (!currSensors.detectWeightRight && pickupTriggered == 0 && currentState != 8)
  {
    // Initiate pick-up Sequence RIGHT 
    pickupTriggered = 2;
    setSweepState (1);
    weights_onboard++;
  }
  
} 

