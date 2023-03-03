
#include <Servo.h>
#include <LiquidCrystal.h>

// initialize the arduino pin configuration for the lcd display
const int rs = 1, en = 2, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/** Pin configuration */
const int BUTTON_PIN = 6;
const int SERVO_PIN = 3;

/** 3 options for our simulation selection type */
const int NO_MEETINGS_SIM = 1;
const int WITH_MEETINGS_SIM = 2;
const int PRESCALED_SIM = 3;

/** Simulation configuration settings */

int simulationSelected = NO_MEETINGS_SIM;
bool simulationStarted = false;
int simulationSteps = 27;
int simulationCurrentStep = 0;
int simulationHourIndex = 0;
float maxSlowdown = 10;
float maxMomentumForScalingSlowdown = 121.5;

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

char *hourLabels[10] = {"--", "8am", "9am", "10am", "11am", "12pm", "1pm", "2pm", "3pm", "4pm"};

/** Servo configuration settings */

Servo workerArmServo; 

const int NEUTRAL_POSITION = 45;
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

  lcd.begin(16, 2);
  lcd.print("     Press");
  lcd.setCursor(0, 1);
  lcd.print("Start Simulation");

  //serial print isn't compatible with the lcd writing, enable only when we need debugging
  //Serial.begin(9600); 
}

/** Main loop runs forever */
void loop() {  

  //buttonState = digitalRead(BUTTON_PIN);
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
    resetWorkerArms();
    Serial.println("starting simulation");
  }
  if (!simulationStarted && simulationSelected == 0) {
    Serial.println("Please select a simulation and try again");
  }
}

/** reset the simulation back to it's initial state */
void stopSimulation() {
  Serial.println("stopping simulation");
  doUpdateStatusDisplayForFinishedSimulation(simulationSelected);

  simulationCurrentStep = 0;
  simulationHourIndex = 0;

  doUpdateLightStrip(simulationHourIndex);

  simulationStarted = false;

  delay(10000);
}

/** Run a single iteration of our simulation for a 20 min interval mapped to 1 second of time */
void runSimulationStep() {
  
   if (simulationCurrentStep < simulationSteps) {

     simulationHourIndex = ((int) simulationCurrentStep / 3) + 1;
     float momentumValue = getMomentumValueForSelectedSimulation(simulationSelected, simulationCurrentStep);
     float slowdownFactor = translateMomentumToSlowdownFactor(momentumValue);
     displaySimulationStepDetails(simulationHourIndex, momentumValue);
     doOneSecondOfWork(slowdownFactor);
     doUpdateLightStrip(simulationHourIndex);
     doUpdateStatusDisplayForRunningSimulation(simulationHourIndex, momentumValue);
  
     simulationCurrentStep++;     
   } else {
     stopSimulation();
   }
}

/** Update the light position showing on the strip, 0 index is no lights, 1 index is the first light */
void doUpdateLightStrip(int hourIndex) {
  //TODO fill this in to update the actual lights
  Serial.print("updating the light strip to position ");
  Serial.println(hourIndex);
}

/** Update the summary display with relevant text information showing the current momentum status */
void doUpdateStatusDisplayForRunningSimulation(int hourIndex, float momentumValue) {
  //TODO fill this in to update the actual display
  Serial.print("updating the summary display with momentum ");
  Serial.print(momentumValue);
  Serial.print(" at time ");
  Serial.println(hourLabels[hourIndex]);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hour: ");
  lcd.print(hourLabels[hourIndex]);
  lcd.setCursor(0, 1);
  lcd.print("Momentum: ");
  lcd.print((int)momentumValue);
}

/** Update the summary display once the simulation is complete to show final stats of avg/max momentum for the run */
void doUpdateStatusDisplayForFinishedSimulation(int simType) {

  //get the avg and max for the dataset.. lets just do this inline for simplicity
  float sumMomentum = 0;
  float maxMomentum = 0;
  float avgMomentum = 0;

  for (int i = 0; i < simulationSteps; i++) {
    float momentum = getMomentumValueForSelectedSimulation(simType, i);
    if (momentum > maxMomentum) {
      maxMomentum = momentum;
    }
    sumMomentum += momentum;
  }
  avgMomentum = sumMomentum / simulationSteps;

  //TODO make this update the actual display
  Serial.print("updating the summary display with max momentum ");
  Serial.print(maxMomentum);
  Serial.print(" and avg momentum ");
  Serial.println(avgMomentum);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Avg: ");
  lcd.print((int)avgMomentum);
  lcd.print(" Max: ");
  lcd.print((int)maxMomentum);
  lcd.setCursor(0, 1);
  lcd.print("Total: ");
  lcd.print((int)sumMomentum);

}


/** Get the momentum value for where we are in the simulation, for the selected dataset */
float getMomentumValueForSelectedSimulation(int simType, int currentStepIndex) {
  if (simType == NO_MEETINGS_SIM) {
    return noMeetingsSimValues[currentStepIndex];
  } else if (simType == WITH_MEETINGS_SIM) {
    return withMeetingsSimValues[currentStepIndex];
  } else if (simType == PRESCALED_SIM) {
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

/** Translate our momentum value which increases speed with bigger numbers into an inverted slowdown factor
which goes slower with bigger numbers and also has a normalized scale, 1 being the fastest, 10 being the slowest */
float translateMomentumToSlowdownFactor(float inputMomentum) {

  if (simulationSelected == PRESCALED_SIM || inputMomentum == 0) {
    return inputMomentum;
  } else {
    float percentMomentum = invLerp(0, maxMomentumForScalingSlowdown, inputMomentum);
    float slowDownFactor = lerp(1, maxSlowdown, 1 - percentMomentum);

    return slowDownFactor;
  }

}

/** Reset the position of our servo back to our neutral aligned pose */
void resetWorkerArms() {
  workerArmServo.write(NEUTRAL_POSITION); 
}

/** Move the servo position oscillating left and right continuing from the 
current position and adjusting the speed according to the slowdown input factor */
void doOneSecondOfWork(float slowdownFactor) {
  int totalDelaySoFar = 0;
  int minPosition = NEUTRAL_POSITION - ARC_SIZE;
  int maxPosition = NEUTRAL_POSITION + ARC_SIZE;
  int delayAmount = (int) 100 / (ARC_SIZE/ARC_STEP) * slowdownFactor;

  //handle special case of slowdown == 0, do nothing for 1 second
  if (slowdownFactor == 0) {
    delay(1000);
    return;
  }

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
void slowStepWorkerArms(float slowdownFactor) {
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

/** linear interpolation */
float lerp(float start, float end, float percent) {
  return start + percent * (end - start);
}

/** inverse linear interpolation to get the percent */
float invLerp(float start, float end, float value) {
  return (value - start) / (end - start);
}



