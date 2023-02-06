# MIDI controlled oscillator with 3 SN76489 and Arduino Nano

In this project 3 SN76489 chips are used to create sound. Each SN76489 has it's own oscillator provided by a SI5351.
This creates the possibility to detune the second en third SN76489 with respect to the first SN76489.

As an IDE I used Visual Studio Code with the PlatformIO plugin. To get started I made copies of
[MIDI library](https://github.com/FortySevenEffects/arduino_midi_library), 
[Si5351 library](https://github.com/etherkit/Si5351Arduino) and 
[TM1638lite library](https://github.com/danja/TM1638lite)

I intend to properly use these libraries as libraries. I added the SN76489::setDivider method to be able to set exact pre-calculated divisors for each note
so there is no need for float-arithmetic.
