# Temperature, humidity and CO2 box
Description:
This Arduino-based weather station measures and displays indoor temperature, humidity, and CO₂ levels. It also shows a 4-day weather forecast (THIS DATA IS NOT UPDATED) and historical graphs. The data is displayed on an SSD1306 OLED screen, and users can navigate between views using buttons.

## What do you need
* Arduino nano(nanoatmega328new in my case).
* 0.96 inch OLED Display SSD1306 128*64.
* BME280 5v with temperature and humidity.
* MH-Z19B for CO2 measurements.
* 2 buttons.
* VS code + PlatformIO for build project.

## Pin Connection Scheme

| Component          | Arduino Pin | Description                                               |
|--------------------|-------------|-----------------------------------------------------------|
| **BME280 Sensor**  | A4 (SDA)    | I2C data line (shared with OLED)                          |
|                    | A5 (SCL)    | I2C clock line (shared with OLED)                         |
| **MH-Z19 CO₂ Sensor** | RX - Pin 3  | Connect MH-Z19 **RX** to Arduino **Pin 3**               |
|                    | TX - Pin 2  | Connect MH-Z19 **TX** to Arduino **Pin 2**               |
| **Button next screen**    | D6          | Navigate to next screen (`btn_next_screen`)                     |
| **Button update**   | D8          | Refresh current graph screen or add random value for debugging if not commented(`btn_update`)              |                    |
| **OLED Display (SSD1306)**  | A4 (SDA)    | I2C data line (shared with BME280)                          |
|                    | A5 (SCL)    | I2C clock line (shared with BME280)                         |

> ⚠️ **Note**: I2C devices (OLED and BME280) must share the same SDA and SCL lines. Also, MH-Z19 sensor connections are logically swapped in code (pin 2 is TX, pin 3 is RX).

## libs (included)
* GyverBME280@^1.5.3: https://github.com/GyverLibs/GyverBME280
* EncButton@^3.7.2: https://github.com/GyverLibs/EncButton
* GyverOLED@^1.6.4: https://github.com/GyverLibs/GyverOLED
* mhz19_uart@^0.31: https://github.com/nara256/mhz19_uart

## TODO:
* rename buttons names to more clarity.
* code refactoring.
* remove forecast data (after creating the same but ESP based project).

---
