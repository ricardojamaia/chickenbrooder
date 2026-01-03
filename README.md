# Chicken Brooder Controller

An intelligent chicken brooder temperature controller built with ESP32/Arduino, featuring automated heating control, environmental monitoring, and MQTT integration for remote monitoring and control.

## Features

- **Automated Temperature Control**: Dual heating lamp control via thermostat to maintain optimal brooding temperature
- **Environmental Monitoring**: Real-time temperature and humidity tracking using DHT11 sensor
- **OLED Display**: 128x64 OLED screen showing current conditions, target temperature, and relay states
- **Manual Controls**: Physical buttons for adjusting target temperature and controlling lighting
- **Remote Monitoring**: MQTT integration for monitoring and control via Home Assistant or other MQTT clients
- **WiFi Connectivity**: OTA (Over-The-Air) updates support for easy firmware updates

## Hardware Requirements

### Core Components
- ESP32 4 relay board 
- DHT11 temperature and humidity sensor
- 128x64 OLED display (I2C, SSD1306)
- 2x heating lamp relays (for heating control)
- 1x lighting relay (for manual light control)
- 2x push buttons - with optional HW debouncing - (temperature adjustment)
- 1x push button  - with optional HW debouncing - (light toggle)

### Pin Configuration (ESP32)

| Component | Pin |
|-----------|-----|
| DHT11 Sensor | GPIO 27 |
| Heating Lamp 1 | GPIO 32 |
| Heating Lamp 2 | GPIO 33 |
| Light Relay | GPIO 25 |
| Relay 4 (spare) | GPIO 26 |
| Increment Button | GPIO 34 |
| Decrement Button | GPIO 35 |
| Light Button | GPIO 14 |
| OLED Display | I2C (SDA/SCL) |

## Software Requirements

- [PlatformIO](https://platformio.org/) (VSCode extension recommended)
- Arduino framework

### Dependencies

All dependencies are automatically managed by PlatformIO:
- Adafruit SSD1306
- Adafruit GFX Library
- DHT sensor library
- PubSubClient (MQTT)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/ricardojamaia/chickenbrooder.git
cd chickenbrooder
```

### 2. Configure Secrets

On first build, the project will automatically prompt you for configuration values:

- WiFi SSID
- WiFi Password
- MQTT Server (IP or hostname)
- MQTT Port (default: 1883)
- MQTT Username
- MQTT Password

These values will be saved in `include/secrets.h` and excluded from git tracking.

**Alternative manual setup:**

Create `include/secrets.h` with the following content:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

// WiFi credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// MQTT configuration
#define MQTT_SERVER "your_mqtt_server"
#define MQTT_PORT 1883
#define MQTT_USER "your_mqtt_username"
#define MQTT_PASSWORD "your_mqtt_password"

#endif
```

### 3. Build and Upload

```bash
pio run -e esp32dev -t upload
```

### 4. Monitor Serial Output

```bash
pio device monitor
```

## Over-The-Air (OTA) Updates

After initial USB upload, you can update the ESP32 wirelessly:

1. Update the `upload_port` in `platformio.ini` to match your ESP32's IP address
2. Run:
```bash
pio run -e esp32dev -t upload
```

## MQTT Topics

The brooder publishes and subscribes to MQTT topics for remote control:

### Published Topics (Status Updates)
- `homeassistant/chicken_brooder/temperature/state` - Current temperature (°C)
- `homeassistant/chicken_brooder/humidity/state` - Current humidity (%)
- `homeassistant/chicken_brooder/target_temperature/state` - Target temperature (°C)
- `homeassistant/chicken_brooder/light/state` - Light state (ON/OFF)
- `brooder/heating` - Heating status

### Subscribed Topics (Commands)
- `homeassistant/chicken_brooder/light/command` - Control light (ON/OFF)
- `homeassistant/chicken_brooder/target_temperature/command` - Set target temperature (float)

## Usage

### Manual Operation

1. **Adjust Target Temperature**: Use the increment/decrement buttons to set desired temperature
2. **Toggle Light**: Press the light button to turn the brooder light on/off
3. **Monitor Display**: View current temperature, humidity, target temperature, and relay states on OLED

### Remote Operation

Control the brooder via MQTT:

**Set target temperature to 32°C:**
```bash
mosquitto_pub -h mqtt_server -t "brooder/target_temperature/set" -m "32.0"
```

**Turn light on:**
```bash
mosquitto_pub -h mqtt_server -t "brooder/light/set" -m "ON"
```

## Home Assistant Integration

Add to your `configuration.yaml`:

```yaml
mqtt:
  sensor:
    - name: "Brooder Temperature"
      state_topic: "brooder/temperature"
      unit_of_measurement: "°C"
      device_class: temperature
    
    - name: "Brooder Humidity"
      state_topic: "brooder/humidity"
      unit_of_measurement: "%"
      device_class: humidity
    
    - name: "Brooder Target Temperature"
      state_topic: "brooder/target_temperature"
      unit_of_measurement: "°C"
  
  light:
    - name: "Brooder Light"
      state_topic: "brooder/light"
      command_topic: "brooder/light/set"
      payload_on: "ON"
      payload_off: "OFF"
  
  number:
    - name: "Brooder Target Temperature"
      state_topic: "brooder/target_temperature"
      command_topic: "brooder/target_temperature/set"
      min: 20
      max: 40
      step: 0.5
      unit_of_measurement: "°C"
```

## Project Structure

```
chickenbrooder/
├── src/
│   └── main.cpp              # Main application code
├── include/
│   ├── secrets.h             # WiFi and MQTT credentials (gitignored)
│   ├── DisplayManager.h      # OLED display control
│   ├── InputManager.h        # Button input handling
│   ├── MqttManager.h         # MQTT client management
│   ├── NetworkManager.h      # WiFi connection management
│   ├── Sensor.h              # DHT11 sensor interface
│   ├── StateControlledLamp.h # Relay control
│   ├── Thermostat.h          # Temperature control logic
│   └── ...
├── lib/                      # Custom libraries
├── build/
│   └── pre_build.py          # Pre-build script for secrets setup
├── platformio.ini            # PlatformIO configuration
└── README.md                 # This file
```

## Development

### Build Flags

- `DEBUG_BROODER`: Enable debug output
- `UDP_SERIAL_MONITOR`: Enable UDP-based serial debugging for wireless monitoring

### Running Tests

```bash
pio test -e test-native
```

## Troubleshooting

### WiFi Connection Issues
- Verify WiFi credentials in `secrets.h`
- Check signal strength
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

### MQTT Connection Issues
- Verify MQTT server is running and accessible
- Check MQTT credentials
- Ensure correct MQTT port (default: 1883)
- Check firewall settings

### Temperature Not Updating
- Verify DHT11 sensor connections
- Check DHT11 power supply (3.3V or 5V)
- Ensure proper pull-up resistor (usually built-in on modules)

### Display Not Working
- Verify I2C connections (SDA/SCL)
- Check display address (default: 0x3C)
- Try different I2C addresses if needed

### OTA Upload Fails
- Ensure ESP32 is on same network
- Verify IP address in `platformio.ini`
- Check that ESP32 is powered on and running

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open source. Feel free to use, modify, and distribute as needed.

## Author

Ricardo Maia

## Acknowledgments

- Built with [PlatformIO](https://platformio.org/)
- Uses [Arduino framework](https://www.arduino.cc/)
- MQTT integration via [PubSubClient](https://github.com/knolleary/pubsubclient)
- Display support from [Adafruit](https://github.com/adafruit)

## Safety Notice

⚠️ **Important**: This project controls heating elements. Always follow proper electrical safety practices:
- Use appropriate relays rated for your heating lamps
- Install proper circuit protection
- Never leave heating elements unattended
- Regularly monitor brooder temperature
- Ensure proper ventilation
- Keep flammable materials away from heating elements

---

**Version**: See `version.h` for current build version and date.