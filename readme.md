# rpi Bare Metal Looper

Please see the **[docs/readme.md](docs/readme.md)** file for
the [documentation](docs/readme.md) regarding this project.

## Background and Context

This repository contains the source code and other information required to
build a bare metal rPi based Audio Looper.

[![Looper1-openAndRunning](docs/images/Looper1-openAndRunning_resized.jpg)](docs/images/Looper1-openAndRunning.jpg)

This project brings together what, until now, have been multiple disparate parts of the puzzle,
and is a milestone in my **[rPi bare metal vGuitar rig](https://hackaday.io/project/165696-rpi-bare-metal-vguitar-rig)**
hackaday project.

It is based on my prior projects (repositories) which include my fork of the
**[circle](https://github.com/phorton1/circle)** rPi bare metal C++ framework,
and my extensions to it, including a port of the Teensy Audio library to
rPi bare metal in my **[circle-prh](https://github.com/phorton1/circle-prh)** project.

The box (my bare metal rPi development environment) also has a Teensy 3.2 board
in it (for serial communications with, and control of, the rPi).  So the
box ALSO includes the **[teensyPiLooper](https://github.com/phorton1/Arduino-teensyPiLooper)**
repository which contains the INO program that runs on the Teensy 3.2 that communicates
with, and controls, the rPi, as well as commmunicating with my
3D printed teensy 3.6 MIDI controller pedal, the
**[teensyExpression Pedal](https://github.com/phorton1/Arduino-teensyExpression)**,
also via serial data.

Together these projects, along with a **Fishman Triple Play** pickup and an **iPad**
running *Tonestack* and *SampleTank* inside of *AudioBus*, form an **actual working rig**.
A set of physical, working hardware, that I can take to a gig, put on the floor, plug in,
and use to play guitar with *effects*, a *midi synthesizer*, and controllable *live loops*
with a user interface (foot pedal) that I like, and that can be somewhat easily extended
and programmed.

In open source.

That is the milestone.  The next full integrated generation of a working rig
I can put on the floor at a bar or restaurant.

Above is a photo of the first 3D printed version of the Looper.  There have
been several phsyical iterations of the "box".  The 3D printing information
(fusion, STL files, etc) can be within the **["documentation"](docs/readme.md)**
folder.

**------------------------------------------**

Please see the **[docs/readme.md](docs/readme.md)** file for
the actual [documentation](docs/readme.md) regarding this project!
