import time

# Import SPI library (for hardware SPI) and MCP3008 library.
import Adafruit_GPIO.SPI as SPI
import Adafruit_MCP3008

#Hardware SPI configuration:
SPI_PORT   = 0
SPI_DEVICE = 0
mcp = Adafruit_MCP3008.MCP3008(spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE))

ADC = mcp.read_adc(0)
ADC = ADC * 1.089
Vbat = (ADC * (3.3 / 1023)) * 2

while True:
    print(Vbat)
    time.sleep(2)

