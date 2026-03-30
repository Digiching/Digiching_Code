// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins 
#define SCLK_PIN 12
#define MOSI_PIN 11
#define DC_PIN   8
#define CS_PIN   10
#define RST_PIN  9

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Fonts/Picopixel.h>
#include <Fonts/TomThumb.h>
#include <Fonts/Org_01.h>
#include <Fonts/FreeSans9pt7b.h>
#include <SPI.h>

// Option 1: use any pins but a little slower
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);  

void setup(void) {
  Serial.begin(9600);
  Serial.print("hello!");
  tft.begin();

  Serial.println("init");

  uint16_t time = millis();
  time = millis() - time;
  
  //tft.fillScreen(BLACK);
  
  Serial.println("done");
  delay(1000);
}

void loop() {
  tft.fillScreen(BLACK);

  tft.setCursor(0, 16);
  tft.setFont(&TomThumb);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setTextWrap(false);
  tft.println("Great Abundance");

  tft.setCursor(0, 0);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.println("Abundance comes into full flower when a group is unified and the power of its leadership is at its peak.");

  delay(3000);
}
