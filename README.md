# RP5-Console
![20250217_181708](https://github.com/user-attachments/assets/bb11127e-8a09-4bc5-866f-330abdede5b4)
![20250220_175925](https://github.com/user-attachments/assets/74970137-21f9-4dec-ac34-e7d4264b4763)
![20250221_055637](https://github.com/user-attachments/assets/17e3d2fc-bc0d-46cf-8d3f-710262dd478a)
![20250221_060310](https://github.com/user-attachments/assets/b28b8ec1-e9c4-479c-8aaf-4ef1404c62e9)
![20250220_180016](https://github.com/user-attachments/assets/d9fcb935-722a-42e8-9214-ad88465778b3)
![20250220_180003](https://github.com/user-attachments/assets/f41678a5-d000-4839-a484-7cb65c2e1dbb)
![RP5 _ Assembly1](https://github.com/user-attachments/assets/34a4e818-2b05-4846-8007-b0b71aa49648)
![RP5 _ Assembly2](https://github.com/user-attachments/assets/baed8283-d7a0-4850-bc22-539dbef000c4)


The process of building this console and setting it up can be complex, expensive, and time consuming. Not for the faint of heart.

# Setup

Keep battery installed but disconnected until prompted

With the back casing removed and the battery disconnected, place console face down on a soft surface

ATSAMD21E18A and ATMEGA32U4 ICs need USB bootloaders. ATMEGAs Leonardo bootloader can acquired and burned using an Arduino Nano in Arduino IDE. ATSAMD21s Adafruit Trinket MO bootloader can be burned using a J-Link EDU mini in Microchip Studio. There are online resources for help with this. You will need to make wire breakouts from the associated connectors for each device

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
