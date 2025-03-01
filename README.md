# RP5-Console
![20250220_175925](https://github.com/user-attachments/assets/acff3b9c-0091-4454-b69b-ee2c6de1da45)
![20250217_181708](https://github.com/user-attachments/assets/bb11127e-8a09-4bc5-866f-330abdede5b4)
The above image of the boards differs slightly from the included board files and the bodges you see were from minor mistakes on first build. Those have been corrected for this repository.

The process of building this console and setting it up can be complex, expensive, and time consuming. Not for the faint of heart.

BOM coming soon..

# Setup

Keep battery installed but disconnected until prompted

With the back casing removed and the battery disconnected, place console face down on a soft surface

ATSAMD21E18A and ATMEGA32U4 ICs need USB bootloaders. ATMEGAs Leonardo bootloader can be acquired and flashed using an Arduino Nano in Arduino IDE. ATSAMD21s Adafruit Trinket M0 bootloader can be flashed using a J-Link EDU mini in Microchip Studio. There are online resources for help with this. You will need to make wire breakouts from the associated connectors for each device

Connect the battery

Connect Raspberry Pi 27W USB-C Power Supply (or any USB-C Power Delivery 12V 2A+ capable supply) to USB-C port on the console. 

Burn the bootloaders

Press the reset buttons on each device

Connect a Micro-USB cable from the slave port (right Micro-USB port with console face down) to PC

Follow the truth table on the Main Board/USB schematic for ATSAMD21 (Switch 1-4 = low, high, low, high) and ATMEGA (Switch 1-4 = high, low, low, high) (low = switch down position and vice versa)

Use Arduino IDE to flash the included scripts for each device. For ATSAMD21, install and select Adafruit Trinket M0 and double-click reset button to show up as COM Port. For ATMEGA, select Arduino Leonardo and hit the reset button once flashing begins

Follow the instructions in the Ardupad script to calibrate the joysticks

Follow the instructions in the README in the EDID folder

Follow the truth table on the Main Board/USB schematic to boot CM5 in rpiboot mode: (Switch 1-4 = low, low, high, low)

On PC, install and run Raspberry Pi Imager: https://www.raspberrypi.com/software/

On PC, follow the instructions to install and run rpiboot for CM5 at: https://www.raspberrypi.com/documentation/computers/compute-module.html#set-up-the-host-device

Follow the instructions to install retropie on PI5 OS Lite at: https://github.com/danielfreer/raspberrypi5-retropie-setup (Choose SSD if installed or EMMC as storage unless you have CM5 Lite version and want to use SD card)

Remove power supply and disconnect battery

Follow the truth table on the Main Board/USB schematic for normal operation: (Switch 1-4 = low, low, low, high)

Reconnect battery and connect power supply

Use WinSCP or SSH Terminal to:

Make sure /boot/firmware/config.txt is identical to included config.txt file

Place Scripts folder in /home/pi/

Place service files in /etc/systemd/system/

In SSH Terminal:

Enable each service: ex: systemctl enable fan.service

Run:

sudo apt remove python3-rpi.gpio

sudo apt-get install python3-rpi-lgpio

chmod 777 /var/log/fan.log

chmod 777 /var/log/fan.rpm

To turn on the console, press and hold the power button for 5 seconds. The power led will turn on (red) and then switch to the activity led (now solid green). To turn off, press and hold the power button for 5 seconds

If you shutdown via the menu or terminal, press the power button once after the shutdown process has finished
