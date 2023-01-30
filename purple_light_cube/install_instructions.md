# Purple Circuit Overview

This circuit is designed to work on an arduino compatible controller board, and has been tested with the Elegoo UNO R3 board.  The circuit creates a purple light that turns on when a sensor is covered.  

## Circuit Ingredients

You will need:

1 solderless breadboard
1 arduino compatible controller board such as the Elegoo UNO R3
1 photoresistor
1 RGB LED
3 220 ohm resistors
1 2k ohm resistor
15 jumper wires

## Install the circuit controller code

To install the circuit controller code, you will need to download and install the Arduino IDE which can be found at: https://www.arduino.cc/en/software

Open the purple_circuit.ino file in the IDE, plug in your controller board to the computer using the provided cable, and press the install button to install the code to the controller.

## Wire up the solderless breadboard

Use the wiring diagram shown in the purple_circuit_diagram.png image to wire up the circuit.  The circuit is designed to wire the major components (the LED and photoresistor) onto the edge of the breadboard to make it easy to slide the breadboard into the cardboard enclosure prototype.

## Test the Circuit

Make sure the circuit is in good lighting, as when the photoresistor is in shadow, the purple light will already be turned on.  To test the circuit, cover the photoresistor with your finger and the purple light should turn on, then turn off when no longer covered.

If you are operating the circuit in a darker room, you may need to adjust the threshold for the amount of light required to hit the photoresistor for it to be considered "not covered".

To do this modification, first examine the serial output for the sensor value being reported within the room, and observe the values when the photoresistor is covered versus uncovered.  Change the line in the code that reads "if (sensorValue < 600)" so that the threshold of 600 is a number that corresponds to a value in-between the observed values.

After the code has been modified, press the install button in the arduino IDE again to install the updated code on the board.
