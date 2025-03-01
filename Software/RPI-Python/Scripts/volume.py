import evdev
import subprocess
import os

# Find the correct event file dynamically
device_path = None
device_name = "USB_AUDIO_CODEC"  # Adjust based on `ls /dev/input/by-id/` output

for event_file in os.listdir("/dev/input/by-id/"):
    if device_name in event_file and "-event" in event_file:
        device_path = f"/dev/input/by-id/{event_file}"
        break

if not device_path:
    print("Error: Could not find the USB audio input device.")
    exit(1)

try:
    device = evdev.InputDevice(device_path)
    print(f"Listening for volume keys on {device_path}...")

    for event in device.read_loop():
        if event.type == evdev.ecodes.EV_KEY:
            if event.value == 1:  # Key press detected
                if event.code == evdev.ecodes.KEY_VOLUMEDOWN:
                    subprocess.run(["amixer", "-c", "2", "set", "PCM", "5%-"])  # Reduce volume
                elif event.code == evdev.ecodes.KEY_VOLUMEUP:
                    subprocess.run(["amixer", "-c", "2", "set", "PCM", "5%+"])  # Increase volume
                elif event.code == evdev.ecodes.KEY_MUTE:
                    subprocess.run(["amixer", "-c", "2", "set", "PCM", "toggle"])  # Mute/unmute
except FileNotFoundError:
    print(f"Error: Device {device_path} not found. Check 'ls /dev/input/by-id/'")
except PermissionError:
    print("Error: Run this script with sudo.")
