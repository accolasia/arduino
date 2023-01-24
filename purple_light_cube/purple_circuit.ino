
int sensorPin = A0;   // input pin for the photoresistor
int sensorValue = 0;  // initial input value for the photoresistor

#define BLUE 3 
#define GREEN 5 
#define RED 6

void setup() {

  pinMode(RED, OUTPUT); 
  pinMode(GREEN, OUTPUT); 
  pinMode(BLUE, OUTPUT); 
  digitalWrite(RED, LOW); 
  digitalWrite(GREEN, LOW); 
  digitalWrite(BLUE, LOW);

  Serial.begin(9600);
}

void loop() {

  sensorValue = analogRead(sensorPin);   // read the value from the photoResistor:

  Serial.print("Sensor:");
  Serial.print(sensorValue);
  Serial.print("\n");

  if (sensorValue < 600) { //sensor covered up

    //create a purple light
    analogWrite(RED, 220);
    analogWrite(GREEN, 30);
    analogWrite(BLUE, 220);
  } else {

    //turn the light off
    analogWrite(RED, 0);
    analogWrite(GREEN, 0);
    analogWrite(BLUE, 0);

  }
}
