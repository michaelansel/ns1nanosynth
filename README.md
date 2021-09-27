# Overview
This is the firmware I run on my Soundmachines NS1nanosynth.

I have two goals with this project:
1. Do fun things with my nanosynth
2. Characterize the hardware and make a framework for musical experimentation (i.e. make it easier to do Goal 1)

# Notes
* I do this for fun on the side and I'm sharing the code as a reference so others can skip the reverse engineering and guesswork I needed to do.
* All pin connection information and MIDI CC configuration is in main.cpp. Reusable logic is pushed as much as possible into separate files.
* The nanosynth is a USB peripheral and must be connected to a USB host. In order to connect to the MiniLab which is also a USB peripheral, I connect both to either my laptop or iPad and route all MIDI messages between the two. I use Pocket MIDI on Windows and midimittr on iPhone/iPad and set up bidirectional routing.