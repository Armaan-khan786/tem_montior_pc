import serial
import time
import subprocess
import socket

class mqtt_library:
    ROBOT_LIBRARY_SCOPE = 'GLOBAL'

    def __init__(self):
        self.ser = None

    # ---------------- Compile ESP ----------------
    def compile_esp(self):
        """Compile the ESP32 sketch"""
        subprocess.run([
            "arduino-cli",
            "compile",
            "--fqbn", "esp32:esp32:esp32",
            "temperature"   # Make sure your sketch folder is named 'temperature'
        ], check=True)

    # ---------------- Flash ESP ----------------
    def flash_esp(self):
        """Upload the compiled sketch to ESP32"""
        subprocess.run([
            "arduino-cli",
            "upload",
            "-p", "COM6",   # Change COM port if needed
            "--fqbn", "esp32:esp32:esp32",
            "temperature"
        ], check=True)

    # ---------------- Open Serial ----------------
    def open_serial(self):
        """Open serial connection to ESP32"""
        if self.ser is None:
            self.ser = serial.Serial("COM6", 115200, timeout=5)
            time.sleep(3)

    # ---------------- Validate Temperature ----------------
    def validate_temperature(self):
        """Check if ESP32 is sending valid temperature"""
        self.open_serial()
        for i in range(50):  # Try 50 lines
            line = self.ser.readline().decode(errors="ignore").strip()
            if "TEMP:" in line:
                value = float(line.split(":")[1])
                if 0 < value < 100:
                    return
        raise Exception("Temperature validation failed: No valid TEMP value received")

    # ---------------- Validate Wi-Fi ----------------
    def validate_wifi(self):
        """Check if Wi-Fi RSSI is valid. Fail if internet is down."""
        # First, check if internet is reachable
        try:
            socket.create_connection(("8.8.8.8", 53), timeout=5)
        except OSError:
            raise Exception("Internet/Wi-Fi not reachable")

        # Then, check RSSI from ESP
        self.open_serial()
        for i in range(50):
            line = self.ser.readline().decode(errors="ignore").strip()
            if "RSSI:" in line:
                value = int(line.split(":")[1])
                if value < 0:
                    return
        raise Exception("Wi-Fi RSSI validation failed")