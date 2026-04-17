# Digiching Code [![PlatformIO](https://img.shields.io/badge/PlatformIO-Registered-blue)](https://platformio.org/) [![ESP32-S3](https://img.shields.io/badge/ESP32-S3-yellow)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/index.html) [![Arduino](https://img.shields.io/badge/Framework-Arduino-blue)](https://www.arduino.cc/)

Digiching Code is an ESP32-S3 project that displays live motion and voltage data on a compact SPI screen. It reads accelerometer and gyroscope values from an MPU6050 sensor and shows a live analog voltage reading on a 128x128 display [file:43][file:44].

## Features

- Live accelerometer readings from the MPU6050.
- Live gyroscope readings from the MPU6050.
- Voltage monitoring from an analog input.
- 128x128 SPI display output using TFT_eSPI.
- Sprite-based drawing for smoother screen updates.
- Built for the ESP32-S3 DevKitC-1 with PlatformIO and Arduino [file:44].

## Hardware Setup

### Required hardware

- ESP32-S3 DevKitC-1.
- MPU6050 sensor.
- Compatible 128x128 SSD1351 SPI display.
- Analog input source for voltage measurement.
- USB cable for power and uploading.

### Display wiring

Connect the display using these pins:

- SCLK → GPIO 12
- MOSI → GPIO 11
- CS → GPIO 10
- DC → GPIO 13
- RST → GPIO 14 [file:44]

### Sensor wiring

Connect the MPU6050 using I2C. Make sure the sensor has power and ground connected, and wire SDA and SCL to the pins used in your setup.

### Analog input

The sketch reads analog voltage from GPIO 4 [file:43].

## Getting Started

### Install PlatformIO

Install [PlatformIO](https://platformio.org/) in VS Code if you do not already have it.

### Open the project

Open the repository folder in VS Code with PlatformIO support enabled.

### Build and upload

Use the PlatformIO terminal:

```bash
pio run
pio run --target upload
```

### Open the serial monitor

```bash
pio device monitor
```

## How It Works

On startup, the board initializes the display, MPU6050 sensor, and analog input. It briefly shows a welcome message, then continuously updates the screen with accelerometer, gyroscope, and voltage readings [file:43].

## Troubleshooting

### Display is blank

- Check the display wiring.
- Confirm the screen matches the configured driver and resolution.
- Make sure the display is powered correctly.
- Verify the TFT_eSPI configuration matches your hardware [file:44].

### MPU6050 is not showing data

- Check the I2C wiring.
- Confirm the sensor is powered.
- Make sure the MPU6050 address is correct.
- Rebuild and upload the sketch again [file:43].

### Voltage reading looks wrong

- Confirm the analog input is connected to GPIO 4.
- Make sure the voltage stays within the safe ADC range for the ESP32-S3.
- Check the wiring and ground reference [file:43].

### Upload fails

- Verify the correct board is selected in PlatformIO.
- Use a USB cable that supports data.
- Reconnect the board and try again [file:44].

### Text is clipped or misaligned

- Confirm the display is 128x128.
- Adjust sprite size, font size, or cursor position if needed [file:43].

## Project Structure

- `main_screen.cpp` — Main application code [file:43].
- `platformio-2.ini` — PlatformIO project configuration and library setup [file:44].

## Notes

- The project is currently configured for a 128x128 display.
- If you change the display or board, you may need to update the configuration [file:44].

## License

Add your license here if you plan to publish the project publicly.

## Acknowledgements

Built with:
- ESP32 and Arduino.
- PlatformIO.
- Adafruit sensor libraries [file:44].
- TFT_eSPI [file:44].
