# Gate Delay with Arduino Pro Micro

In this project a Gate Delay is designed as a module for a modular synth. It uses an Arduino Pro Micro as microcontroller. 
It uses 1024 bytes aka 8192 bits max for the delay, giving a max delay of around 8 seconds at approximately 1 kHz sampling rate. 
Course delay is determined by ANALOG_COURSE (A8) in 8 steps. Fine delay is controlled by ANALOG_FINE (A9), setting the
interval of the interrupt timer. Both analogue pins are connected to linear 10k pots. The digitalWriteFast-library was used to optimize the ISR.

The input and output circuits were designed using LtSpice. Leds were added to the input and output circuits for visual feedback.

The circuit was laid out on a prototype board, using a buck converter to provide the 5V for the Arduino from the +12V-rail.

90 degree 3d-printed connectors were used to connect the front pane to the PCB, using 3mm bolts. The design is provided in STL-format.