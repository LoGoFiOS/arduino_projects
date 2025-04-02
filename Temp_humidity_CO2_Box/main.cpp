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
#include <GRGB.h>

#define SCL A5 // for BME280 and display
#define SDA A4 // for BME280 and display
#define R_PIN 9 // for RGB led
#define G_PIN 10 // for RGB led
#define B_PIN 11 // for RGB led
#define RX_PIN 2 // BUT "REAL" WIRE FROM RX CONNECTED TO PIN 3!!!
#define TX_PIN 3 // BUT "REAL" WIRE FROM T CONNECTED TO PIN 2!!!
#define BTN_NEXT_SCREEN_PIN 4 // btn_next_screen
#define BTN_UPDATE_PIN 5 // btn_update

GRGB led(COMMON_CATHODE, R_PIN, G_PIN, B_PIN);
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> display;
GyverBME280 bme;
MHZ19_uart mhz19;
Button btn_next_screen(BTN_NEXT_SCREEN_PIN);
Button btn_update(BTN_UPDATE_PIN);

// Timers initialization
uint32_t last_sensor_read_time = 0;
uint32_t last_display_blink_time = 0;
#define DISPLAY_TIME_BLINK_INTERVAL 1000
const uint32_t SENSOR_READ_INTERVAL = 1800000; // every half hour

// display
uint8_t current_screen = 0; // can be changed for screen debug
uint8_t display_font_size = 1; // don't change!

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
  mhz19.begin(RX_PIN, TX_PIN);
  mhz19.setAutoCalibration(false);
  led.setCRT(true);
  led.setBrightness(210);
  led.setHSV(200, 200, 200);
  delay(500);

  display.init();
  display.setContrast(128);
  display.clear();
  display_font_size = 1;
  display.setScale(display_font_size);
  display.setCursorXY(CURSOR_X(0), CURSOR_Y(0));
  display.print("initializing...");
  display.setCursorXY(CURSOR_X(0), CURSOR_Y(2));
  display.print("Wait for 3 min.");
  display.setCursorXY(CURSOR_X(0), CURSOR_Y(3));
  display.print("CO2 sensor must");
  display.setCursorXY(CURSOR_X(0), CURSOR_Y(4));
  display.print("warm up...");
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
    // setRGBColor(200, 200, 200); //for RGB debug
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