To create a bootable rPi circle SD Card for the Looper

- Uses a Regular FAT32 formatted SD Card
- Copy the contents of the /circle/_prh/boot/to_sd_card to the SD Card
- Copy the appropriate kernel7.img to the SD Card (built in /_apps/Looper) to it

Note that the recovery.img is special and MUST BE
built with #define ARM_ALLOW_MULTI_CORE commented out
of /circle/include/sysconfig.h !!!

There is currently an issue of some kind (probably with the new "audioMonitor")
that I am trying to resolve at this time.

There is a "#define USE_AUDIO_MONITOR = 0/1" in /_prh/ws/wsApp.cpp.




