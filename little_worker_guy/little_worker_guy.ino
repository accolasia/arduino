
#include <Servo.h>

/** Pin configuration */
const int BUTTON_PIN = 6;
const int SERVO_PIN = 3;

/** 3 options for our simulation selection type */
const int NO_MEETINGS_SIM = 1;
const int WITH_MEETINGS_SIM = 2;
const int PRESCALED_SIM = 3;

/** Simulation configuration settings */

int simulationSelected = PRESCALED_SIM;
bool simulationStarted = false;
int simulationSteps = 27;
int simulationCurrentStep = 0;
int simulationHourIndex = 0;

float prescaledSimValues[] = {1, 3, 5, 3, 8, 1, 2,3, 
                      5, 8,1, 3, 4,2,1, 3, 
                      4, 1, 5, 2,8, 4, 2, 3,
                      4, 1, 3};

float noMeetingsSimValues[] = {0, 5.75, 15.5, 35.5, 55.5, 75.5, 94.3, 80.3, 
                      73.75, 64.5, 84.5, 103.3, 0, 0, 0, 2.75, 
                      15.5, 20.5, 40.5, 60.5, 80.5, 100.5, 111.5, 101.5,
                      121.5, 105.5, 91.5};
float withMeetingsSimValues[] = {0, 5.75, 10.5, 0, 0, 0, 0, 2.75, 
                      15.5, 20.5, 15.5, 25.5, 0, 0, 0, 0, 
                      2.75, 5.75, 0, 0, 0, 0, 2.75, 15.5,
                      20.5, 35.5, 25.5};

/** Servo configuration settings */

Servo workerArmServo; 

const int NEUTRAL_POSITION = 69;
const int ARC_SIZE = 30;
const int ARC_STEP = 2;

int currentPosition = NEUTRAL_POSITION;
int direction = 1;

/** Button state pressed or not */

int buttonState = 0;

/* Setup our pin configuration and initialize everything */
void setup() {

  pinMode(BUTTON_PIN, INPUT);
  workerArmServo.attach(SERVO_PIN);

  resetWorkerArms();
  Serial.begin(9600);
}

/** Main loop runs forever */
void loop() {  
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) {
    startSimulationIfNotStarted();
  }

  if (simulationStarted) {
    runSimulationStep();
  }

}

/** Start the simulation specified in the selected simulation when the button is pressed */
void startSimulationIfNotStarted() {
  if (!simulationStarted && simulationSelected != 0) {
    simulationStarted = true;
    Serial.println("starting simulation");
  }
  if (!simulationStarted && simulationSelected == 0) {
    Serial.println("Please select a simulation and try again");
  }
}

/** reset the simulation back to it's initial state */
void stopSimulation() {
  Serial.println("stopping simulation");
  simulationStarted = false;
  simulationCurrentStep = 0;
  simulationHourIndex = 0;
  resetWorkerArms();
}

/** Run a single iteration of our simulation for a 20 min interval mapped to 1 second of time */
void runSimulationStep() {
  
   if (simulationCurrentStep < simulationSteps) {

     simulationHourIndex = ((int) simulationCurrentStep / 3) + 1;
     int momentumValue = getMomentumValueForSelectedSimulation(simulationCurrentStep);
     displaySimulationStepDetails(simulationHourIndex, momentumValue);
     doOneSecondOfWork(momentumValue);
  
     simulationCurrentStep++;     
   } else {
     stopSimulation();
   }
}

/** Get the momentum value for where we are in the simulation, for the selected dataset */
float getMomentumValueForSelectedSimulation(int currentStepIndex) {
  if (simulationSelected == NO_MEETINGS_SIM) {
    return noMeetingsSimValues[currentStepIndex];
  } else if (simulationSelected == WITH_MEETINGS_SIM) {
    return withMeetingsSimValues[currentStepIndex];
  } else if (simulationSelected == PRESCALED_SIM) {
    return prescaledSimValues[currentStepIndex];
  }

}

/** Write diagnostic data to the screen for this simulation step */
void displaySimulationStepDetails(int hourIndex, float momentumValue) {
    Serial.print("[RUN SIMULATOR] hour = ");
    Serial.print(simulationHourIndex);
    Serial.print(", momentum = ");
    Serial.println(momentumValue);
}

/** Reset the position of our servo back to our neutral aligned pose */
void resetWorkerArms() {
  workerArmServo.write(NEUTRAL_POSITION); 
}

/** Move the servo position oscillating left and right continuing from the 
current position and adjusting the speed according to the slowdown input factor */
void doOneSecondOfWork(int slowdownFactor) {
  int totalDelaySoFar = 0;
  int minPosition = NEUTRAL_POSITION - ARC_SIZE;
  int maxPosition = NEUTRAL_POSITION + ARC_SIZE;
  int delayAmount = (int) 100 / (ARC_SIZE/ARC_STEP) * slowdownFactor;

  while (totalDelaySoFar < 1000) {
    if (direction > 0) {
      if( currentPosition < maxPosition) {
        currentPosition += ARC_STEP;
        workerArmServo.write(currentPosition); 
        delay(delayAmount);   
        totalDelaySoFar += delayAmount; 
      } else {
        direction = -1;        
      }
    } else if (direction < 0) {
      if (currentPosition > minPosition ) {
        currentPosition -= ARC_STEP;
        workerArmServo.write(currentPosition); 
        delay(delayAmount);
        totalDelaySoFar += delayAmount; 
       } else {
         direction = 1;
       }
    }
  }
  //this would add a delay between each 1 second increment of simulation
  //delay(200);
}

/** Fastest possible movement from the servo with a full movement and no delays */
void fastestWorkerArms() {
  workerArmServo.write((NEUTRAL_POSITION - ARC_SIZE));
  delay(200);  
  workerArmServo.write((NEUTRAL_POSITION + ARC_SIZE)); 
  delay(200);   
}

/** Move the worker arms a single oscillation at different speeds, used for testing */
void slowStepWorkerArms(int slowdownFactor) {
  int minPosition = NEUTRAL_POSITION - ARC_SIZE;
  int maxPosition = NEUTRAL_POSITION + ARC_SIZE;
  int delayAmount = (int) 100 / (ARC_SIZE/ARC_STEP) * slowdownFactor;

  while (currentPosition < maxPosition) {
    currentPosition += ARC_STEP;
    workerArmServo.write(currentPosition); 
    delay(delayAmount);    
  }
  while (currentPosition > minPosition) {
    currentPosition -= ARC_STEP;
    workerArmServo.write(currentPosition); 
    delay(delayAmount);    
  }
}



