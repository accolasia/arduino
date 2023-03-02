
#include <Servo.h>

int buttonState = 0;
const int buttonPin = 6;

const int NO_MEETINGS_SIM = 1;
const int WITH_MEETINGS_SIM = 2;
const int PRESCALED_SIM = 3;

bool simulationStarted = false;
int simulationSelected = PRESCALED_SIM;
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


Servo myservo;  // create servo object to control a servo


const int neutralPosition = 69;
const int arcSize = 30;
const int arcStep = 2;
float workerSlowdownFactor = 1;

int currentPosition = neutralPosition;
int direction = 1;

void setup() {

  pinMode(buttonPin, INPUT);
  myservo.attach(3); 

  resetWorkerArms();
  Serial.begin(9600);
}

void loop() {  
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    startSimulationIfNotStarted();
  }

  if (simulationStarted) {
    runSimulationStep();
  }

}


void startSimulationIfNotStarted() {
  if (!simulationStarted && simulationSelected != 0) {
    simulationStarted = true;
    Serial.println("starting simulation");
  }
  if (!simulationStarted && simulationSelected == 0) {
    Serial.println("Please select a simulation A or B");
  }
}

void stopSimulation() {
  Serial.println("stopping simulation");
  simulationStarted = false;
  simulationCurrentStep = 0;
  simulationHourIndex = 0;
  resetWorkerArms();
}

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

float getMomentumValueForSelectedSimulation(int currentStepIndex) {
  if (simulationSelected == NO_MEETINGS_SIM) {
    return noMeetingsSimValues[currentStepIndex];
  } else if (simulationSelected == WITH_MEETINGS_SIM) {
    return withMeetingsSimValues[currentStepIndex];
  } else if (simulationSelected == PRESCALED_SIM) {
    return prescaledSimValues[currentStepIndex];
  }

}

void displaySimulationStepDetails(int hourIndex, float momentumValue) {
    Serial.print("[RUN SIMULATOR] hour = ");
    Serial.print(simulationHourIndex);
    Serial.print(", momentum = ");
    Serial.println(momentumValue);
}

void resetWorkerArms() {
  myservo.write(neutralPosition); 
}

void fastestWorkerArms() {
  myservo.write((neutralPosition - arcSize));
  delay(200);  
  myservo.write((neutralPosition + arcSize)); 
  delay(200);   
}

void doOneSecondOfWork(int slowdownFactor) {
  int totalDelaySoFar = 0;
  int minPosition = neutralPosition - arcSize;
  int maxPosition = neutralPosition + arcSize;
  int delayAmount = (int) 100 / (arcSize/arcStep) * slowdownFactor;

  while (totalDelaySoFar < 1000) {
    if (direction > 0) {
      if( currentPosition < maxPosition) {
        currentPosition += arcStep;
        myservo.write(currentPosition); 
        delay(delayAmount);   
        totalDelaySoFar += delayAmount; 
      } else {
        direction = -1;        
      }
    } else if (direction < 0) {
      if (currentPosition > minPosition ) {
        currentPosition -= arcStep;
        myservo.write(currentPosition); 
        delay(delayAmount);
        totalDelaySoFar += delayAmount; 
       } else {
         direction = 1;
       }
    }
  }
  delay(200);
}


void slowStepWorkerArms() {
  int minPosition = neutralPosition - arcSize;
  int maxPosition = neutralPosition + arcSize;
  int delayAmount = (int) 100 / (arcSize/arcStep) * workerSlowdownFactor;

  while (currentPosition < maxPosition) {
    currentPosition += arcStep;
    myservo.write(currentPosition); 
    delay(delayAmount);    
  }
  while (currentPosition > minPosition) {
    currentPosition -= arcStep;
    myservo.write(currentPosition); 
    delay(delayAmount);    
  }
}



