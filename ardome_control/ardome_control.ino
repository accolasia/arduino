
#include <CapacitiveSensor.h>

//selector knob constants

const int THRESHOLD_TO_REPORT_CHANGE = 20;
const int SENSOR_HIGH_VALUE = 1023;
const int SENSOR_LOW_VALUE = 0;

//capacitive button constants

#define COMMON_PIN 2
#define NUM_OF_SAMPLES 10
#define CAP_THRESHOLD 100

CapacitiveSensor cs_2_3 = CapacitiveSensor(2, 3);    
CapacitiveSensor cs_2_4 = CapacitiveSensor(2, 4); 

//initial state of the buttons and knobs

int lastSensorDialValue = 0;
int sensorDialValue = 0;

int msgNumber = 0;

int frictionToggle = 0;
int flowToggle = 0;

void setup() {

  //calibrate capacitive sensor
  cs_2_3.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_2_4.set_CS_AutocaL_Millis(0xFFFFFFFF);

  Serial.begin(9600);
}

void loop() {

  int sensorDialValue = analogRead(A0);

  long frictionSensor =  cs_2_3.capacitiveSensor(NUM_OF_SAMPLES);
  long flowSensor =  cs_2_4.capacitiveSensor(NUM_OF_SAMPLES);

  //use for debugging only, since the serial data is being read by consumer app

  // Serial.println(sensorDialValue);
  // Serial.println(frictionSensor);
  // Serial.println(flowSensor);

  if (frictionSensor > CAP_THRESHOLD) {
    toggleFrictionOnIfOff();
  } else {
    toggleFrictionOffIfOn();
  }

    if (flowSensor > CAP_THRESHOLD) {
    toggleFlowOnIfOff();
  } else {
    toggleFlowOffIfOn();
  }

  if (isChangeOverThreshold(lastSensorDialValue, sensorDialValue)) {
    lastSensorDialValue = sensorDialValue;
    sendStatusUpdate("SELECT", (String)lastSensorDialValue);
  }

  delay(200);  // delay in between reads for stability

}

/** Sends a serial message formatted for parsing by consumer app */

void sendStatusUpdate(String commandType, String value) {
  Serial.println(((String)msgNumber) + ":"+commandType + ":" + value);
  msgNumber++;
}

/** If the friction toggle button state is off, turn it on, sends a status update */
void toggleFrictionOnIfOff() {
  if (frictionToggle == 0) {
    sendStatusUpdate("FRICTION", "1");
    frictionToggle = 1;
  }
}

/** If the friction toggle button state is on, turn it off, without a status update */
void toggleFrictionOffIfOn() {
  if (frictionToggle == 1) {
    frictionToggle = 0;
  }
}

/** If the flow toggle button state is off, turn it on, sends a status update */
void toggleFlowOnIfOff() {
  if (flowToggle == 0) {
    sendStatusUpdate("FLOW", "1");
    flowToggle = 1;
  }
}

/** If the flow toggle button state is off, turn it on, sends a status update */
void toggleFlowOffIfOn() {
  if (flowToggle == 1) {
    flowToggle = 0;
  }
}

/** If the knob change amount is over a threshold, send an update, prevents shakes spamming event */
bool isChangeOverThreshold(int lastSensorValue, int sensorValue) {
  if (abs(lastSensorValue - sensorValue) > THRESHOLD_TO_REPORT_CHANGE || 
    (lastSensorValue != sensorValue && (sensorValue == SENSOR_LOW_VALUE || sensorValue == SENSOR_HIGH_VALUE))) {
    return true;
  } else {
    return false;
  }
}