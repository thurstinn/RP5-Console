Program EDID using Arduino Uno or Nano boards:

GND EDID connector pin 1 pin to Arduino GND
5V EDID connector pin 2 to Arduino 5V
SDA EDID connector pin 3 to Arduino SDA A4
SCL EDID connector pin 4 to Arduino SCL A5 
 

1. Power off RP5 PCB
2. Program Arduino Uno or Nano board with EDID.ino
3. Unplug Arduino board USB from PC
4. Connect above Arduino board pins to EDID connector on RP5 PCB
5. Move EDID switch to EDID Flash position (down position on attached PCB)
6. Connect Arduino board USB to PC
7. Check Arduino board is connected and open Serial Monitor in Arduino IDE
8. Follow Serial Instructions (Press any key and Enter)
9. If verified successfully, Remove Arduino board USB from PC
10. Move EDID switch back to normal position (up position on attached PCB)
11. Power up RP5 and verify display is working