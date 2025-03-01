# RP5-Console

The process of building this console and setting it up can be complex, expensive, and time consuming. Not for the faint of heart.

Keep battery installed but disconnected until prompted
With the back casing removed and the battery disconnected, place console face down on a soft surface

ATSAMD21E18A and ATMEGA32U4 ICs need USB bootloaders. ATMEGAs Leonardo bootloader can acquired and burned using an Arduino Nano in Arduino IDE
ATSAMD21s Adafruit Trinket MO bootloader can be burned using a J-Link EDU mini in Microchip Studio
There are online resources for help with this
You will need to make wire breakouts from the associated connectors for each device

Connect the battery

Connect Raspberry Pi 27W USB-C Power Supply (or any USB-C Power Delivery 12V 2A+ capable supply) to USB-C port on the console. 

Burn the bootloaders

Press the reset buttons on each device

Connect a Micro-USB cable from the slave port (right Micro-USB port with console face down) to PC

Follow the truth table on the Main Board/USB schematic for ATSAMD21 (Switch 1-4 = low, high, low, high) and ATMEGA (Switch 1-4 = high, low, low, high) (low = switch down position and vice versa)

Use Arduino IDE to program the included scripts for each device. For ATSAMD21, double-click reset button to show up as COM Port. For ATMEGA hit the reset button once flashing begins

Follow the instructions in the Ardupad script to calibrate the joysticks

Follow the truth table on the Main Board/USB schematic to boot CM5 in rpiboot mode: (Switch 1-4 = low, low, high, low)

On PC, install and run Raspberry Pi Imager: https://www.raspberrypi.com/software/

On PC, follow the instructions to install and run rpiboot for CM5 at: https://www.raspberrypi.com/documentation/computers/compute-module.html#set-up-the-host-device

Follow the instructions to install retropie on PI5 OS Lite at: https://github.com/danielfreer/raspberrypi5-retropie-setup (Choose SSD if installed or EMMC as storage unless you have CM5 Lite version and want to use SD card)

Follow the truth table on the Main Board/USB schematic for normal operation: (Switch 1-4 = low, low, low, high)

Hold power button for 5 seconds to turn on the console.

Use WinSCP or SSH Terminal to:

Place Scripts folder in /home/pi/
Place service files in /etc/system/system/

In SSH Terminal:

Enable each service: ex: systemctl enable fan.service

Run:

sudo apt remove python3-rpi.gpio

sudo apt-get install python3-rpi-lgpio

chmod 777 /var/log/fan.log

chmod 777 /var/log/fan.rpm
