// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins 
#define SCLK_PIN 12
#define MOSI_PIN 11
#define DC_PIN   13
#define CS_PIN   10
#define RST_PIN  14

// MPU Libraries
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Display Libraries
#include <TFT_eSPI.h>

// General Libraries
#include <SPI.h>
#include <ESP32AnalogRead.h>

// Define Things
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
ESP32AnalogRead mic;
Adafruit_MPU6050 mpu = Adafruit_MPU6050();

void setup(void) {
  mic.attach(4);

  Serial.begin(9600);
  Serial.print("MPU with OLED");

  // Make it the size of your display
  spr.createSprite(128, 128); 

  // Initialise TFT Display
  tft.init();

  // Initialise MPU Sensor
  mpu.begin();

  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(TFT_WHITE);
  tft.println ("Hello World!");
  
  Serial.println("Done Initialisation");
  delay(2000);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  spr.fillScreen(TFT_BLACK); 
  spr.setTextSize(1);
  spr.setTextColor(TFT_WHITE);
  spr.setCursor(0, 0);

  /*Serial.print("Accelerometer ");
  Serial.print("X: ");
  Serial.print(a.acceleration.x, 1);
  Serial.print(" m/s^2, ");
  Serial.print("Y: ");
  Serial.print(a.acceleration.y, 1);
  Serial.print(" m/s^2, ");
  Serial.print("X: ");
  Serial.print(a.acceleration.x, 1);
  Serial.println(" m/s^2, ");*/
  
  spr.setTextColor(TFT_RED);
  spr.println("Accelerometer - m/s^2");
  spr.setTextColor(TFT_WHITE);
  spr.println("");
  spr.print(a.acceleration.x, 1);
  spr.print(", ");
  spr.print(a.acceleration.y, 1);
  spr.print(", ");
  spr.print(a.acceleration.x, 1);
  spr.println("");
  spr.println("");

  /*Serial.print("Gyroscope ");
  Serial.print("X: ");
  Serial.print(g.gyro.x, 1);
  Serial.print(" rps, ");
  Serial.print("Y: ");
  Serial.print(g.gyro.y, 1);
  Serial.print(" rps, ");
  Serial.print("Z: ");
  Serial.print(g.gyro.z, 1);
  Serial.println(" rps");*/

  spr.println("");
  spr.println("");

  spr.setTextColor(TFT_RED);
  spr.println("Gyroscope - rps");
  spr.setTextColor(TFT_WHITE);
  spr.println("");
  spr.print(g.gyro.x, 1);
  spr.print(", ");
  spr.print(g.gyro.y, 1);
  spr.print(", ");
  spr.print(g.gyro.z, 1);
  spr.println("");
  spr.println("");

  // Serial.println("Voltage = "+String(mic.readVoltage()));

  spr.println("");
  spr.println("");
  spr.setTextColor(TFT_RED);
  spr.println("Voltage - Volts");
  spr.setTextColor(TFT_WHITE);
  spr.println("");
  spr.println(String(mic.readVoltage()));

  spr.pushSprite(0,0);
  delay(50);
}

