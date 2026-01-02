# Live Train Departure Update for Berlin

This repository contains code for an ESP32 controlling a 20x04 LCD screen to display departures going into a certain direction from the station of your choice

### Setup

To setup your esp32 modify the config.example.h into config.h and insert your parameters

The code was written in platform.io in a VSCode environment, it is configured for the ESP32-WROVER-KIT which you can change in the platformio.ino file or in the project settings

### Additional Features

The code is written to allow your to pick another station of your choice, which will display if you flip a switch make sure you wire the switch to the correct pin or specify your pin in the code

