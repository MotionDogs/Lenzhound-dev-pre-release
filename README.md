Lenzhound
=========

##Building
First, make sure you have the 1.5.x beta for Arduino installed

- [Windows](http://downloads.arduino.cc/arduino-1.5.8-windows.exe)
- [OSX](http://downloads.arduino.cc/arduino-1.5.8-macosx.zip)
- Linux - [32](http://downloads.arduino.cc/arduino-1.5.8-linux32.tgz) - [64](http://downloads.arduino.cc/arduino-1.5.8-linux64.tgz)

###Linux
Clone the repo
```
git clone https://github.com/MotionDogs/Lenzhound.git
```

Verify the builds
```
cd Lenzhound/
make -C Rxr/
make -C Txr/
```

Plug in the transmitter unit and call
```
make -C /Txr upload
```

Then plug in the receiver unit and call
```
make -C /Rxr upload
```

And you're set!
