# RP5-Console
![20250220_175925](https://github.com/user-attachments/assets/efd2b189-bc79-4237-8d38-fa1be679ee37)
![20250218_021543](https://github.com/user-attachments/assets/cad879eb-6285-4e7b-8d83-0847d62c0543)

The above image of the boards differs slightly from the included board files and the bodges you see were from minor mistakes on first build. Those have been corrected for this repository.

This project was inspired by StonedEdge's impressive Retro-Lite-CM4 (https://github.com/StonedEdge/Retro-Lite-CM4). I do not use any of their files directly (exception being their project partner's Ardupad.ino (https://github.com/dmcke5/Ardupad), but some of the design choices were influenced by their console

The process of building this console and setting it up can be complex, expensive, and time consuming. Not for the faint of heart

BOM coming soon..

# Setup

Keep battery installed but disconnected until prompted

With the back casing removed and the battery disconnected, place console face down on a soft surface

Set manual power switch to on position (Use dull-ended tweezers)

ATSAMD21E18A and ATMEGA32U4 ICs need USB bootloaders. ATMEGAs Leonardo bootloader can be acquired and flashed using an Arduino Nano in Arduino IDE. ATSAMD21s Adafruit Trinket M0 bootloader can be flashed using a J-Link EDU mini in Microchip Studio. There are online resources for help with this (https://learn.adafruit.com/how-to-program-samd-bootloaders/overview). (I couldn't get Microchip Studio to read the chip on my Windows 11 desktop but it worked on my Windows 10 laptop) You will need to make wire breakouts from the associated connectors for each device. The MPN for the mates of each connector are located in the schematics (SUHR-0XV-S). The MPN of the metal contacts that need crimped on the wires (28 awg) for all connector mates is: SSUH-003T-P0.15

Connect the battery

Connect Raspberry Pi 27W USB-C Power Supply (or any USB-C Power Delivery 12V 2A+ capable supply) to USB-C port on the console

Burn the bootloaders

Press the reset buttons on each device

Connect a Micro-USB cable from the slave port (right Micro-USB port with console face down) to PC

Follow the truth table on the Main Board/USB schematic for ATSAMD21 (Switch 1-4 = low, high, low, high) and ATMEGA (Switch 1-4 = high, low, low, high) (low = switch down position and vice-versa)

Use Arduino IDE to flash the included scripts for each device. For ATSAMD21, install and select Adafruit Trinket M0 and double-click reset button to show up as COM Port. For ATMEGA, select Arduino Leonardo and hit the reset button once flashing begins

Follow the instructions in the Ardupad script to calibrate the joysticks

Follow the instructions in the README in the EDID folder

Set manual power switch to off postition

If using an SSD or EMMC, follow the truth table on the Main Board/USB schematic to boot CM5 in rpiboot mode: (Switch 1-4 = low, low, high, low)

Set manual power switch to on position

On PC, install and run Raspberry Pi Imager: https://www.raspberrypi.com/software/

On PC, follow the instructions to install and run rpiboot for CM5 at: https://www.raspberrypi.com/documentation/computers/compute-module.html#set-up-the-host-device

Follow the instructions to install retropie on PI5 OS Lite at: https://github.com/danielfreer/raspberrypi5-retropie-setup (Choose SSD if installed or EMMC as storage unless you have CM5 Lite version. In that case, skip rpiboot and just flash an SD Card)

Set manual power switch to off postition

Follow the truth table on the Main Board/USB schematic for normal operation: (Switch 1-4 = low, low, low, high)

Turn the trimming resistor (R68) just over half way. If turned much more it could damage the speakers

Set manual power switch to on postition and keep it there

Use WinSCP or SSH Terminal to:

Make sure /boot/firmware/config.txt is identical to included config.txt file

Copy Scripts folder to /home/pi/

In SSH Terminal:

Copy service file contents for each file to new files of the same names in /etc/systemd/system/ ex: sudo nano /etc/systemd/system/fan.service

Enable each service: ex: sudo systemctl enable fan.service

Run:

sudo apt remove python3-rpi.gpio

sudo apt-get install python3-rpi-lgpio

sudo apt-get install python3-evdev

sudo nano /var/log/fan.log (write out empty file)

sudo nano /var/log/fan.rpm (write out empty file)

sudo chmod 777 /var/log/fan.log

sudo chmod 777 /var/log/fan.rpm

In (sudo raspi-config) and retropie/audio, set the audio output to USB AUDIO CODEC

In the retropie main menu/sound settings (press start button), the audio card should be default and device should be PCM

To turn on the console, press and hold the power button for 5 seconds. The power led will turn on (red) and then switch to the activity led (now solid green). To turn off, press and hold the power button for 5 seconds. This will soft shutdown the OS and then turn off the console. If you shutdown the OS via the UI or terminal, you will have to press and hold the power button once the shutdown process has finished to turn off the console
