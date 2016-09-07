/** @file   main_module_v6_00.ino
    @author A. Warburton, F. Sullivan
    @date   12 OCTOBER 2015
    @brief  FINAL Assesment Code
*/

#include <Servo.h>
#include <Math.h>
#include <NewPing.h>
#include <RunningAverage.h>
#include <Herkulex.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

unsigned long timC;

/* Number of main tasks in the scheduler. */
#define NUM_TASKS     6

/* Scheduling clock. */
unsigned long schedClock = 0;

/* Time array for each corresponding task. */
unsigned long schedTime[NUM_TASKS] = {1, 2, 3, 4, 5, 6};

/* Frequency at which each task is called (ms). */
unsigned long schedInterval[NUM_TASKS] = {50,   // Sensors
                                          400,  // Colour Sensor
                                          50,   // Weight Detection
                                          80,   // State Machine
                                          80,   // Update Speed (Drive)
                                          150}; // Update Servos (Pick-up)

/* Structure to impliment the weight detection debouncing. */
struct weight_detect {
  int readsLeft;           // Number of consecutive positive weight reads
  int readsRight;
  boolean weightLeft;      // Weight defined as found by debouncing algorithm
  boolean weightRight;
};
typedef struct weight_detect weight_detect;

/* Structure to hold the most current sensor reads. */
struct sensor_reads {
  float IRLeft;            // Most recent reading of the IR sensors
  float IRRight;
  long sonarLeft;          // Most recent reading of the sonar sensors
  long sonarRight;
  int detectLeft;          // Most recent reading of side IR detect sensors
  int detectRight;
  int detectWeightLeft;    // Most recent initial weight detection readings
  int detectWeightRight;
  int detectFront;     // Most recent reading of front IR detect sensors
  int detectFrontRight;
  int limitLeft;           // Most recent limit switch readings
  int limitRight;
  int greenBase;           // Most recent base detection readings
  int blueBase;
};
typedef struct sensor_reads sensor_reads;


/* Create a structure to hold all current sensor readings. */
struct sensor_reads allSensors = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0};
/* Create a structure to hold all current weight detects. */
struct weight_detect allWeights = {0, 0, 0, 0};


/** Calls the corresponding function to each task if it is called to be executed
    @param  The task number being called to execute   */
void taskExecute (int taskNumber)
{
  switch (taskNumber) {
    case 0:
      /* Updates IR sensor & sonar values. */
      allSensors = updateAllSensors (allSensors);
      break;
      
    case 1:
      /* Update colour sensor values. */
       allSensors = updateColourSensor (allSensors);
       break;
       
    case 2:
      /* Check if a weight has been found. */
      allWeights = updateWeightDetect (allSensors, allWeights);
      break;
      
    case 3:
      /* Using the state machine to update the overall state of the robot. */
      updateStates (allSensors, allWeights);
      break;
      
    case 4:
      /* The drive speed is updated based primarily on the current state of the robot. */
      updateDrive (allSensors);
      break;
      
    case 5:
      /* The servos are updated based on the triggering of the digital IR sensors. */
      updateServos (allSensors);
      break;
  }
}


void setup (void)
{
  /* Initialize serial communication. */
  Serial.begin(115200);
  Serial.println ("Program Starting");

  /* Initialise all sensors ,arrays and servos. */
  sensorSetup();
  driveSetup();
  stateSetup();
  servoSetup();
}


//void loop (void)
//{
//  schedClock = millis();
//  
//  int i;
//  /* Iterates through all the tasks. */
//  for (i = 0; i < NUM_TASKS; i++) 
//  {
//    /* Checks if that task is ready to be carried out . */
//    if (schedClock >= schedTime[i])
//    {
//      schedTime[i] += schedInterval[i];
//      taskExecute(i);
//      break;
//    }
//  }
//}

void loop (void)
{
  allSensors = updateAllSensors (allSensors);
  allSensors = updateColourSensor (allSensors);
  allWeights = updateWeightDetect (allSensors, allWeights);
  updateStates (allSensors, allWeights);
  updateDrive  (allSensors);
  updateServos (allSensors);
}

