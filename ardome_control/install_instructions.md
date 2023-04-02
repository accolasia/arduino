# AR Code Planetarium Controller Circuit Overview

This circuit is designed to work on an arduino compatible controller board, and has been tested with the Elegoo UNO R3 board.  The circuit creates an input device for the AR Code Planetarium to with a selection knob and two capacitive touch buttons.  The interactions are translated to a serial port message which can be then read by another application.  Currently the serial port read is built into the FlowInsight desktop application and then the messages are then forwarded onward to the AR device.  

## Install the circuit controller code

To install the circuit controller code, you will need to download and install the Arduino IDE which can be found at: https://www.arduino.cc/en/software

Open the ardome_control.ino file in the IDE, plug in your controller board to the computer using the provided cable, and press the install button to install the code to the controller.
