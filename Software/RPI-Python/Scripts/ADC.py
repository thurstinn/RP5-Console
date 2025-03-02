import time
import spidev  # For SPI-based ADCs like MCP3008

# Initialize SPI for MCP3008
spi = spidev.SpiDev()
spi.open(0, 0)  # Open SPI bus 0, device 0
spi.max_speed_hz = 1350000

def read_adc(channel):
    """Reads an ADC channel using SPI."""
    adc = spi.xfer2([1, (8 + channel) << 4, 0])
    data = ((adc[1] & 3) << 8) | adc[2]
    return data

def read_battery_voltage(samples=10, v_ref=3.3, resolution=1024, divider_ratio=2):
    """Reads battery voltage with averaging."""
    total = 0
    for _ in range(samples):
        adc_value = read_adc(0)  # Assuming battery voltage is on channel 0
        total += adc_value
        time.sleep(0.005)  # Small delay for stability

    avg_adc = total / samples  # Compute average
    voltage = (avg_adc / resolution) * v_ref * divider_ratio
    return voltage

# Example usage
bat_voltage = read_battery_voltage(samples=20)
bat_voltage *= 1.19
print(f"Battery Voltage: {bat_voltage:.2f}V")
