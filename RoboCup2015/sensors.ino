/** @file   sensors.ino
    @author A. Warburton, F. Sullivan
    @date   12 OCTOBER 2015
    @brief  Header file for all SENSOR-based & WEIGHT DETECTION activities
*/


/* IR pins. */
#define IR_LEFT_SHORT 	      A10
#define IR_RIGHT_SHORT 	      A8
#define IR_LEFT_LONG 	        A11
#define IR_RIGHT_LONG         A9

/* Sonar pins. */
#define SON_LEFT_TRIG 	      32
#define SON_LEFT_ECHO 	      35
#define SON_RIGHT_TRIG	      33
#define SON_RIGHT_ECHO	      34

/* Digital IR pins. */
#define DIGI_IR_LEFT 	        37
#define DIGI_IR_RIGHT         36
#define DIGI_IR_WEIGHT_LEFT   45
#define DIGI_IR_WEIGHT_RIGHT  40
#define DIGI_IR_FRONT         43
#define DIGI_IR_FRONT_RIGHT   39

/* Limit switch pins. */
#define LIMIT_LEFT            41
#define LIMIT_RIGHT           38

/* Maximum distance for each sonar. */
#define MAX_SONAR_DIST	      200

/* Sensor angle. */
#define SENSOR_ANGLE          60

/* Distance between the centre Of Mass and sensor (cm). */
#define L_COM_SEN 	          20

/* Angle between the centre Of Mass and sensor (deg). */
#define ANG_COM_SEN 	        40

/* Number of consecutive reads to confirm a weight found. */
#define N_PRESENT             5 

/* Difference in sensor reading to confirm a weight has been found (cm). */
#define WEIGHT_RANGE          15

/* Difference in base positions (cm). */
#define CHANGE_IN_BASE        5

/* Size of sensor buffers */
#define BUF_SIZE              3

/* Create a new instance of the colour sensor. */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);


/* Running Average object for sonar and IR readings */
RunningAverage IR_left_ave (BUF_SIZE);
RunningAverage IR_right_ave (BUF_SIZE);
RunningAverage SON_left_ave (BUF_SIZE);
RunningAverage SON_right_ave (BUF_SIZE);

/* Create NewPing instances for each sonar. */
NewPing SON_LEFT(SON_LEFT_TRIG, SON_LEFT_ECHO, MAX_SONAR_DIST);
NewPing SON_RIGHT(SON_RIGHT_TRIG, SON_RIGHT_ECHO, MAX_SONAR_DIST);


/** A function that sets-up and initialises all pins corresponding to sensors */
void sensorSetup (void)
{
  /* IR pin setup. */
  pinMode (IR_LEFT_LONG, INPUT);
  pinMode (IR_LEFT_SHORT, INPUT);
  pinMode (IR_RIGHT_LONG, INPUT);
  pinMode (IR_RIGHT_SHORT, INPUT);
  
  /* Sonar pin setup. */
  pinMode (SON_RIGHT_TRIG, OUTPUT);
  pinMode (SON_RIGHT_ECHO,INPUT);
  pinMode (SON_LEFT_TRIG, OUTPUT);
  pinMode (SON_LEFT_ECHO,INPUT);
  
  /* Digital IR setup. */
  pinMode(DIGI_IR_LEFT,INPUT);
  pinMode(DIGI_IR_RIGHT,INPUT);
  pinMode(DIGI_IR_WEIGHT_LEFT,INPUT);
  pinMode(DIGI_IR_WEIGHT_RIGHT,INPUT);
  pinMode(DIGI_IR_FRONT,INPUT);
  pinMode(DIGI_IR_FRONT_RIGHT,INPUT);
  
  /* Limit switch pin setup. */
  pinMode(LIMIT_LEFT,INPUT);
  pinMode(LIMIT_RIGHT,INPUT);
  
  /* Clear running average buffers. */
  IR_left_ave.clear();
  IR_right_ave.clear();
  SON_left_ave.clear();
  SON_right_ave.clear();
}


/** Reads all sensors and updates values in the sensor structure
    @param  the old sensor structure
    @return the updated sensor structure WITHOUT affecting the weight detection mechnism  */
struct sensor_reads updateAllSensors (struct sensor_reads oldSensors)
{ 
  float longReadLeft = IR_convert_long_DATASHEET (analogRead (IR_LEFT_LONG));
  float shortReadLeft = IR_convert_short_DATASHEET (analogRead (IR_LEFT_SHORT));
  float longReadRight = IR_convert_long_DATASHEET (analogRead (IR_RIGHT_LONG));
  float shortReadRight = IR_convert_short_DATASHEET (analogRead (IR_RIGHT_SHORT));
  
  /* Insert new values into running average buffers. */
  IR_left_ave.addValue (IR_check(shortReadLeft, longReadLeft));
  IR_right_ave.addValue (IR_check (shortReadRight, longReadRight));
  SON_right_ave.addValue (SON_RIGHT.ping() / US_ROUNDTRIP_CM);
  SON_left_ave.addValue (SON_LEFT.ping() / US_ROUNDTRIP_CM);
  
  /* Compare long and short IR readings and return the valid one to another variable, -1 otherwise. */
  oldSensors.IRLeft = IR_left_ave.getAverage();
  oldSensors.IRRight = IR_right_ave.getAverage();
  
  /* Read both sonar distance values in cm and store into structure. */
  oldSensors.sonarLeft = SON_left_ave.getAverage();
  oldSensors.sonarRight = SON_right_ave.getAverage();
  
  /* Read both side IR detect sensors and store boolean values into structure. */
  oldSensors.detectLeft = digitalRead(DIGI_IR_LEFT);
  oldSensors.detectRight = digitalRead(DIGI_IR_RIGHT);
  oldSensors.detectWeightLeft = digitalRead(DIGI_IR_WEIGHT_LEFT);
  oldSensors.detectWeightRight = digitalRead(DIGI_IR_WEIGHT_RIGHT);
  oldSensors.detectFront = digitalRead(DIGI_IR_FRONT);
  oldSensors.detectFrontRight = digitalRead(DIGI_IR_FRONT_RIGHT);
  
  /* Read left and right limit switches and store boolean values into structure. */
  oldSensors.limitLeft = digitalRead(LIMIT_LEFT);
  oldSensors.limitRight = digitalRead(LIMIT_RIGHT);
  
  return oldSensors;
}


/** Reads colour sensor and updates value in the sensor structure
    @param  the old sensor structure
    @return the updated sensor structure WITHOUT affecting the weight detection mechnism  */
struct sensor_reads updateColourSensor (struct sensor_reads oldSensors)
{
  int currentState = checkCurrentState();
  if (currentState == 8 || currentState == 5)
  {
    
    uint16_t c, r, g, b;
    /* Get the RGB reading from the colour sensor library. */
    tcs.getRawData(&r, &g, &b, &c);
    
    oldSensors.greenBase = green_base_detect(c, r, g, b);
    oldSensors.blueBase = blue_base_detect(c, r, g, b);
    
    Serial.print("C:\t"); Serial.print(c);
    Serial.print("\tR:\t"); Serial.print(r);
    Serial.print("\tG:\t"); Serial.print(g);
    Serial.print("\tB:\t"); Serial.println(b);

  }
  return oldSensors;
}

/** Uses the debouncing theory to test if a weight has been found
    @param  the sensor structure holding all averaged sensor values
    @param  the old weight detection structure holding present reads
    @return the new detection structure  */
struct weight_detect updateWeightDetect (struct sensor_reads currSensors, struct weight_detect oldWeights)
{
  /* LEFT */
  if (currSensors.IRLeft != -2 && currSensors.sonarLeft != 0) 
  {
    float sensorDifLeft = currSensors.IRLeft - (currSensors.sonarLeft - CHANGE_IN_BASE);
    
    if (sensorDifLeft >= WEIGHT_RANGE && sensorDifLeft < 135 && (currSensors.sonarLeft > 25) && (currSensors.sonarLeft < 70)) 
    {
      (oldWeights.readsLeft) += 1;
    }
    else
    {
      (oldWeights.readsLeft) = 0;
    }
  }
  
  else
  {
    (oldWeights.readsLeft) = 0;
  }
  
  if ((oldWeights.readsLeft) >= N_PRESENT) {
    // WEIGHT FOUND!
    (oldWeights.weightLeft) = 1;
    LEDLeftSet (1);
  }
  else {
    (oldWeights.weightLeft) = 0;
    LEDLeftSet (0);
  }
  
  
  /* RIGHT */
  if (currSensors.IRRight != -2 && currSensors.sonarRight != 0) 
  { 
    float sensorDifRight = currSensors.IRRight - (currSensors.sonarRight - CHANGE_IN_BASE);
    
    if (sensorDifRight >= WEIGHT_RANGE && sensorDifRight < 135 && (currSensors.sonarRight > 25) && (currSensors.sonarRight < 70)) 
    {
      (oldWeights.readsRight) += 1;
    }
    else
    {
      (oldWeights.readsRight) = 0;
    }
  }
  
  else
  {
    (oldWeights.readsRight) = 0;
  }
  
  if ((oldWeights.readsRight) >= N_PRESENT) {
    (oldWeights.weightRight) = 1;
    LEDRightSet (1);
    // WEIGHT FOUND!
  }
  else {
    (oldWeights.weightRight) = 0;
    LEDRightSet (0);
  }
    
  return oldWeights; 
}


/** Converts analogue long range IR reading into cm using the DATASHEET values
    @param  analogue reading from long range IR sensor
    @return converted value in cm based on 1st set of tests  */
float IR_convert_long_DATASHEET (long l_analogue_read) 
{
  float dist;
  
  dist = (0.00000001 * pow (l_analogue_read, 4)) +
         (-0.00002 * pow (l_analogue_read, 3)) +
         (0.012 * pow (l_analogue_read, 2)) +
         (-3.0792 * l_analogue_read) + 345.8;
         
  /* Only converts valid analogue readings inside the 20 - 150cm range. */
  if (dist > 149) {
    return 150;
  }
  if (dist < 20) {
    return 19;
  }
  return dist;
}


/** Converts analogue medium range IR reading into cm using the DATASHEET values
    @param  analogue reading from medium range IR sensor
    @return converted value in cm based on 1st set of tests  */
float IR_convert_medium_DATASHEET (long l_analogue_read) 
{
  float dist;
  
  dist = (0.000000005 * pow (l_analogue_read, 4)) +
         (-0.000008 * pow (l_analogue_read, 3)) +
         (0.005 * pow(l_analogue_read, 2)) +
         (-1.3884 * l_analogue_read) + 160.44;
  
  /* Only converts valid analogue readings inside the 10 - 80cm range. */
  if (dist > 80) {
    return 81;
  }
  if (dist < 10) {
    return 9;
  }
  return dist;
}


/** Converts analogue long range IR reading into cm using the DATASHEET values
    @param  analogue reading from short range IR sensor
    @return converted value in cm  */
float IR_convert_short_DATASHEET (long l_analogue_read) 
{
  float dist;
  
  dist = (-0.00000000001 * pow (l_analogue_read, 5)) +
         (0.00000003 * pow (l_analogue_read, 4)) +
         (-0.00002 * pow(l_analogue_read, 3)) +
         (0.0061 * pow(l_analogue_read, 2)) +
         (-1.0483 * l_analogue_read) + 86.933;
         
  /* Only converts valid analogue readings inside the 4 - 30cm range. */
  if (dist > 29) {
    return 30;
  }
  if (dist < 5) {
    return 4;
  }
  return dist;
}


/** Function to check and compare short and long IR values to define the full spectrum
    @param  short range IR sensor
    @param  long range IR sensor
    @return valid sensor reading or combination when between ranges   */
float IR_check (float IR_short, float IR_long) 
{  
  if (IR_short < 20) {
    // SHORT = 4cm - 30cm
    return IR_short;
  }
  else if (IR_long > 30) {
    // LONG = 20cm - 100cm
    return IR_long;
  }
  else {
    // AVERAGING
    return (0.5 * IR_short + 0.5 * IR_long);
  }
}

int green_base_detect (int clear, int red, int green, int blue)
{
//  if ((clear < 250 && clear > 215) && (red < 125 && red > 93) && (green < 96 && green > 70) && (blue < 55 && blue > 32))
//  {
//    LEDTemp3Set(1);
//    return 1;
//  }
//  LEDTemp3Set(0);
//  return 0;

  if ((clear < 280 && clear > 220) && 
      (red < 160 && red > 93) && 
      (green < 140 && green > 80) && 
      (blue < 70 && blue > 40))
  {
    LEDTemp2Set(1);
    return 1;
  }
  LEDTemp2Set(0);
  return 0;
}


int blue_base_detect (int clear, int red, int green, int blue)
{
//  if ((clear < 300 && clear > 240) && (red < 128 && red > 95) && (green < 107 && green > 83) && (blue < 95 && blue > 69))
//  {
//    LEDTemp1Set(1);
//    return 1;
//  }
//  LEDTemp1Set(0);
//  return 0;

  if ((clear < 320 && clear > 250) && 
      (red < 148 && red > 115) && 
      (green < 130 && green > 90) && 
      (blue < 110 && blue > 69))
  {
    LEDTemp1Set(1);
    return 1;
  }
  LEDTemp1Set(0);
  return 0;
}


