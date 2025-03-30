/*
 * Weather Station with OLED Display
 * Features:
 * - Current temperature and humidity (BME280)
 * - Weather forecast (8 days)
 * - Multiple screens with statistics
 * - Button navigation
 */

#include <Arduino.h>
#include "weather_station.h"
#include <GyverOLED.h>

#define SCL A5
#define SDA A4

GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> display;
GyverBME280 bme;
MHZ19_uart mhz19;
Button btn_next_screen(6);
Button btn_update(8);

const int rx_pin = 2; //BUT "REAL" WIRE FROM RX CONNECTED TO PIN 3!!!
const int tx_pin = 3; //BUT "REAL" WIRE FROM T CONNECTED TO PIN 2!!!

// Timers initialization
uint32_t last_sensor_read_time = 0;
uint32_t last_display_blink_time = 0;
#define DISPLAY_TIME_BLINK_INTERVAL 1000
const uint32_t SENSOR_READ_INTERVAL = 1800000; // every half hour

// display
uint8_t current_screen = 0; // can be changed for screen debug
uint8_t display_font_size = 1; // don't change

void setup()
{
  // to debug
  // wdt_disable(); // Disable watchdog on startup
  // Serial.println("System starting");
  // Serial.print("Reset reason: ");
  // Serial.println(MCUSR);
  // MCUSR = 0; // Clear reset flags
  // Serial.print(F("Free RAM: "));
  // Serial.println(freeRam());

  randomSeed(analogRead(A0));
  Serial.begin(9600);
  if (!bme.begin(0x76))
    Serial.println("BME280 Error!");
  mhz19.begin(rx_pin, tx_pin);
  mhz19.setAutoCalibration(false);
  delay(500);

  display.init();
  display.setContrast(128);
  display.clear();
  display_font_size = 1;
  display.setScale(display_font_size);
  display.setCursorXY(CURSOR_X(0), CURSOR_Y(0));
  display.print("initializing...");
  display.update();
  initHistoryBuffers();
  delay(180000); // 3 min to warm up MZ-19b
  display.clear();
  readTempAndHumInside();
  readCO2();

  setMainScreen();

  // display.clear(); //for screen debug
  // current_screen = 5;
  // setCO2histScreen();
}

void loop()
{
  // debug
  // static uint32_t last_ram_check = 0;
  // if (millis() - last_ram_check > 1000)
  // {
  //   last_ram_check = millis();
  //   Serial.print("Free RAM: ");
  //   Serial.println(freeRam());
  // }

  uint32_t now = millis();

  // Handle button inputs
  btn_next_screen.tick();
  if (btn_next_screen.click())
    changeScreen();

  btn_update.tick();
  if (btn_update.click()) {
    switch (current_screen)
    {
    case 5:
      //CO2genRandomValue(); //for debug
      drawHistoryGraph(getCO2_HoursAgo, 2000, 400);
      break;
    case 6:
      //TempInGenRandomValue(); //for debug
      drawHistoryGraph(getTempIn_HoursAgo, 30, -10);  
      break;
    }
  }

  // Blink time colon on main screen
  if ((current_screen == 0) && (now - last_display_blink_time >= DISPLAY_TIME_BLINK_INTERVAL))
  {
    last_display_blink_time = now;
    blinkTimeSeparator();
  }

  // Read sensors at regular intervals
  if (now - last_sensor_read_time >= SENSOR_READ_INTERVAL)
  {
    last_sensor_read_time = now;
    readTempAndHumInside();
    readCO2();
  }
}