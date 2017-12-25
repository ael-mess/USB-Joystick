# USB PROJECT

The purpose of this project is to create a USB device with a game controller connected to an Arduino Uno using libusb-1.0 library. To test the program the pacman4console game was used from: `https://github.com/YoctoForBeaglebone/pacman4console.git`, and was adapted to the device.

## PEREQUIREMENTS :

- `ncurses` library to launch the game.
- `libusb-1.0` library to access the usb device.

## MAIN PROGRAM :

### INSTALLING

To install the program type :
~~~ 
$  make install
~~~
- To compile type only `make`.
- And to uninstall type `make clean`.

### RUNING

To run the program type :
~~~
$  ./pacman [level_#] 
~~~
where level_# is 1-9.

Then direct the pacman with the buttons, exit by pressing the analog pad, and pause by pressing the analog pad and the bottom button.
the LED 13 stays on during the game.

## ARDUINO PROGRAM :

If the device is not recognized, the Arduino card must be reprogrammed.

### ATMega328p

The ATMega328p files are in `atmega328p`. The joystick is pluged in `PORTD & 0b01111100` and managed by interruption with the `PCINT2` vector.
To reprogramme the microcontroller it's necessary to flash the `atmega328p` with the Arduino core program :

- Short-circuit the ATMega16u2 reset and ground lines present on the ICSP of this microcontroller.
- The Arduino must no longer be listed as a serial port in an lsusb, you can then execute the following commands:
~~~
$  cd atmega328p
$  make all
$  make upload
$  dfu-programmer atmega16u2 erase
$  dfu-programmer atmega16u2 flash Arduino-usbserial-uno.hex
$  dfu-programmer atmega16u2 reset
$  make clean
~~~
- Disconnect and reconnect your Arduino, your program must be active on ATMega16u2.

### ATMega16u2

The ATMega16u2 files are in `lufa/PolytechLille/atmega16u2`. `0x0002` is the device id vendor and id product.
To reprogramme the microcontroller :

- Short-circuit the ATMega16u2 reset and ground lines present on the ICSP of this microcontroller.
- The Arduino must no longer be listed as a serial port in an lsusb, you can then execute the following commands:
~~~
$  cd lufa/PolytechLille/atmega16u2
$  make
$  dfu-programmer atmega16u2 erase
$  dfu-programmer atmega16u2 flash PAD.hex
$  dfu-programmer atmega16u2 reset
$  make clean
~~~
- Disconnect and reconnect your Arduino, your program must be active on ATMega16u2.
