Lenzhound
=========

##Building
First, make sure you have the 1.5.x beta for Arduino installed

- [Windows](http://downloads.arduino.cc/arduino-1.5.8-windows.exe)
- [OSX](http://downloads.arduino.cc/arduino-1.5.8-macosx.zip)
- Linux - [32](http://downloads.arduino.cc/arduino-1.5.8-linux32.tgz) - [64](http://downloads.arduino.cc/arduino-1.5.8-linux64.tgz)

And that the `arduino` command in the console ends up pointing to the 1.5.x `arduino` executable. (This is tricky and platform-dependent and probably deserves its own guide.)

And install [CMake](http://www.cmake.org/download/)

###Linux/OSX
Clone the repo:
```
git clone https://github.com/MotionDogs/Lenzhound.git
```

Build:
```
cd Lenzhound/
cmake .
make
```

Test:
```
make run-tests
```

Plug in the transmitter unit and call:
```
make upload-txr
```

Then plug in the receiver unit and call:
```
make upload-rxr
```

And you're set!

###Windows
Coming soon...
