// MPU Libraries
#include "HardwareSerial.h"

#include "WString.h"
#include "esp32-hal.h"

#include <Adafruit_LSM6DSOX.h>

// Amp Libraries
//#include <SimpleMAX98357A.h>

// Display Libraries
#include <TFT_eSPI.h>

// JSON Libraries
#include <FS.h>

#include <LittleFS.h>

#include <ArduinoJson.h>

// General Libraries
#include <SPI.h>

#include <pgmspace.h>

#include <ESP32AnalogRead.h>

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

// Image Dimensions
#define IMAGE_WIDTH 96
#define IMAGE_HEIGHT 96

// Define Display Pins
#define SCLK_PIN 12
#define MOSI_PIN 11
#define DC_PIN 13
#define CS_PIN 10
#define RST_PIN 14

// Define Microphone Pin
#define MIC_PIN 4

// Define MPU Pins
#define LSM_CS 15
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 18 // SCL Pin
#define LSM_MISO 17 // DO Pin
#define LSM_MOSI 16 // SDA Pin

// Define Amp Pins

// Define Things
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite( & tft);
ESP32AnalogRead mic;
Adafruit_LSM6DSOX sox;

int shakeyshake = 6; // Counter for how many lines have been drawn, starts at 7 to indicate a full hexagram (6 lines) and trigger a reset on the first shake/blow
String trueLines = ""; // Default to Hexagram 1, The Creative
//SimpleMAX98357A player;

uint16_t imageBuffer[IMAGE_WIDTH * IMAGE_HEIGHT];
fs::File file;


// Define Hexagram Class
class Hexagram {
    public: int id;
    String name;
    String character;
    String image_file;
    String phrase;
};

bool loadRgb565Bin(const char * path, uint16_t * buffer, size_t pixelCount) {
    fs::File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.print("Failed to open image: ");
        Serial.println(path);
        return false;
    }

    size_t expectedBytes = pixelCount * sizeof(uint16_t);
    size_t bytesRead = file.read((uint8_t * ) buffer, expectedBytes);
    file.close();

    if (bytesRead != expectedBytes) {
        Serial.print("Wrong image size, got bytes: ");
        Serial.println(bytesRead);
        return false;
    }

    return true;
}

bool getHexagramById(Stream & input, String wantedLines, Hexagram & out) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }

    for (JsonObject hexagram: doc["hexagrams"].as < JsonArray > ()) {
        int id = hexagram["id"] | -1;
        String lines = hexagram["lines"].as<String>();

        if (lines == wantedLines) {
            out.id = id;
            out.name = hexagram["name"].as < String > ();
            out.character = hexagram["character"].as < String > ();
            out.image_file = hexagram["image_file"].as < String > ();
            out.phrase = hexagram["phrase"].as < String > ();
            return true;
        }
    }

    return false;
}

void printHexagram(Hexagram & hexy, uint16_t image[], TFT_eSprite spritz) {

    spritz.fillScreen(TFT_BLACK);
    spritz.setTextSize(1);
    spritz.setTextColor(TFT_WHITE);
    spritz.setCursor(0, 0);

    spritz.setTextDatum(TC_DATUM);
    spritz.setTextWrap(true);
    spritz.drawString(hexy.name, tft.width() / 2, 0);

    spritz.setSwapBytes(true);
    spritz.pushImage(14, 8, IMAGE_WIDTH, IMAGE_HEIGHT, image);

    spritz.setTextWrap(true);
    spritz.setTextDatum(TL_DATUM);
    spritz.setCursor(0, 104);
    spritz.println(hexy.phrase);

    // Old Method of Wrapping
    //spr.drawString("Success will come from", 0, 104);
    //spr.drawString("the primal depths", 0, 112);
    //spr.drawString("of the universe.", 0, 120);
}


void setup(void) {
    // Initialise Serial for debugging
    Serial.begin(9600);
    delay(2000); // Wait for Serial to initialize
    Serial.println("Digiching - Main Screen");

    // Initialise Audio Amp
    //if (!player.begin  ) {
    //    Serial.println("Player init failed");
    //    return;
    //}

    // Initialise MPU Sensor
    if (!sox.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
        Serial.println("Failed to find LSM6DSOX chip");
        while (1) {
            delay(10);
        }
    }
    Serial.println("LSM6DSOX Found!");

    // sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    Serial.print("Accelerometer range set to: ");
    switch (sox.getAccelRange()) {
        case LSM6DS_ACCEL_RANGE_2_G:
            Serial.println("+-2G");
            break;
        case LSM6DS_ACCEL_RANGE_4_G:
            Serial.println("+-4G");
            break;
        case LSM6DS_ACCEL_RANGE_8_G:
            Serial.println("+-8G");
            break;
        case LSM6DS_ACCEL_RANGE_16_G:
            Serial.println("+-16G");
            break;
    }

    // sox.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS );
    Serial.print("Gyro range set to: ");
    switch (sox.getGyroRange()) {
        case LSM6DS_GYRO_RANGE_125_DPS:
            Serial.println("125 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_250_DPS:
            Serial.println("250 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_500_DPS:
            Serial.println("500 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_1000_DPS:
            Serial.println("1000 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_2000_DPS:
            Serial.println("2000 degrees/s");
            break;
        case ISM330DHCX_GYRO_RANGE_4000_DPS:
            break; // unsupported range for the DSOX
    }

    // sox.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    Serial.print("Accelerometer data rate set to: ");
    switch (sox.getAccelDataRate()) {
        case LSM6DS_RATE_SHUTDOWN:
            Serial.println("0 Hz");
            break;
        case LSM6DS_RATE_12_5_HZ:
            Serial.println("12.5 Hz");
            break;
        case LSM6DS_RATE_26_HZ:
            Serial.println("26 Hz");
            break;
        case LSM6DS_RATE_52_HZ:
            Serial.println("52 Hz");
            break;
        case LSM6DS_RATE_104_HZ:
            Serial.println("104 Hz");
            break;
        case LSM6DS_RATE_208_HZ:
            Serial.println("208 Hz");
            break;
        case LSM6DS_RATE_416_HZ:
            Serial.println("416 Hz");
            break;
        case LSM6DS_RATE_833_HZ:
            Serial.println("833 Hz");
            break;
        case LSM6DS_RATE_1_66K_HZ:
            Serial.println("1.66 KHz");
            break;
        case LSM6DS_RATE_3_33K_HZ:
            Serial.println("3.33 KHz");
            break;
        case LSM6DS_RATE_6_66K_HZ:
            Serial.println("6.66 KHz");
            break;
    }

    // sox.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
    Serial.print("Gyro data rate set to: ");
    switch (sox.getGyroDataRate()) {
        case LSM6DS_RATE_SHUTDOWN:
            Serial.println("0 Hz");
            break;
        case LSM6DS_RATE_12_5_HZ:
            Serial.println("12.5 Hz");
            break;
        case LSM6DS_RATE_26_HZ:
            Serial.println("26 Hz");
            break;
        case LSM6DS_RATE_52_HZ:
            Serial.println("52 Hz");
            break;
        case LSM6DS_RATE_104_HZ:
            Serial.println("104 Hz");
            break;
        case LSM6DS_RATE_208_HZ:
            Serial.println("208 Hz");
            break;
        case LSM6DS_RATE_416_HZ:
            Serial.println("416 Hz");
            break;
        case LSM6DS_RATE_833_HZ:
            Serial.println("833 Hz");
            break;
        case LSM6DS_RATE_1_66K_HZ:
            Serial.println("1.66 KHz");
            break;
        case LSM6DS_RATE_3_33K_HZ:
            Serial.println("3.33 KHz");
            break;
        case LSM6DS_RATE_6_66K_HZ:
            Serial.println("6.66 KHz");
            break;
    }

    // Initialise Microphone
    mic.attach(MIC_PIN);
    Serial.println("Initialised Microphone");

    // Initialise Sprite & Display

    // Make it the size of your display
    spr.createSprite(128, 128);

    // Initialise TFT Display
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    Serial.println("Initialised TFT Display");

    spr.fillScreen(TFT_BLACK);
    spr.setTextSize(1);
    spr.setTextColor(TFT_WHITE);
    spr.setCursor(0, 0);

    spr.fillRect(0, 0, 128, 128, TFT_GOLD);
    spr.fillRect(4, 4, 120, 120, TFT_BLACK);
    
    spr.setTextDatum(TC_DATUM);
    spr.setTextSize(2);
    spr.drawString("Digiching", tft.width() / 2, 30);
    spr.setTextSize(1);
    spr.drawString("Shake or Blow", tft.width() / 2, 60);
    spr.pushSprite(0, 0);
    // LittleFS Setup

    // Mount LittleFS before opening any files
    if (!LittleFS.begin(true)) {
        Serial.println("An error occurred while mounting LittleFS");
        return;
    }



    Serial.println("Done Initialisation");
    delay(50);
}

void loop() {
    //Get a new normalized sensor event 
    // values for acceleration and rotation:
    float xAcc, yAcc, zAcc;
    float xGyro, yGyro, zGyro;

    // values for orientation:
    float roll, pitch, heading;
    // check if the IMU is ready to read:
    if (sox.accelerationAvailable() &&
        sox.gyroscopeAvailable()) {
        // read accelerometer &and gyrometer:
        sox.readAcceleration(xAcc, yAcc, zAcc);
        sox.readGyroscope(xGyro, yGyro, zGyro);

        bool Shaken = abs(xGyro) > 400.0 || abs(yGyro) > 400.0 || abs(zGyro) > 400.0;
        bool Blown = mic.readVoltage() > 0.7 && mic.readVoltage() < 2; // Adjust threshold as needed
        bool HeavyBlow = mic.readVoltage() >= 2; // Adjust threshold as needed

        if (Shaken || Blown || HeavyBlow) {
            // Handle shake event
            if (Shaken) {
                Serial.println("Shake Detected, Gyro Data: " + String(xGyro) + ", " + String(yGyro) + ", " + String(
                    zGyro));
            } else if (Blown) {
                Serial.println("Blow Detected, Mic Voltage: " + String(mic.readVoltage()));
            } else if (HeavyBlow) {
                Serial.println("Heavy Blow Detected, Mic Voltage: " + String(mic.readVoltage()));
            }

            bool split = random(0, 2) == 1; // Randomly decide to split the screen or not
            bool draw = false; // Set to true if you want to draw the rectangles, false to just print the debug info


            if (split) {
                trueLines += "0"; // Add a broken line
                Serial.println("Screen will be split.");
            } else {
                trueLines += "1"; // Add a solid line
                Serial.println("Screen will not be split.");
            }

            if (!draw && shakeyshake >= 6) {
                Serial.println("Hexagram complete, resetting screen.");
                spr.fillScreen(TFT_BLACK);
            }

            int y;

            switch (shakeyshake) {
                case 0:
                    y = 10;
                    shakeyshake++;
                    draw = true;
                    break;
                case 1:
                    y = 30;
                    shakeyshake++;
                    draw = true;
                    break;
                case 2:
                    y = 50;
                    shakeyshake++;
                    draw = true;
                    break;
                case 3:
                    y = 70;
                    shakeyshake++;
                    draw = true;
                    break;
                case 4:
                    y = 90;
                    shakeyshake++;
                    draw = true;
                    break;
                case 5:
                    y = 110;
                    shakeyshake++;
                    draw = true;
                    break;
                case 6:
                    y = 0;
                    shakeyshake = 0;
                    trueLines = ""; // Reset for next hexagram
                    draw = false;
                    break;
            }

            Serial.println("Drawing rectangle at y: " + String(y) + ", split: " + String(split));
            Serial.println("Current trueLines: " + trueLines);

            if (draw) {
                switch (split) {
                    case false:
                        spr.drawRect(12, y, 100, 10, TFT_WHITE);
                        break;
                    case true:
                        spr.drawRect(12, y, 40, 10, TFT_WHITE);
                        spr.drawRect(72, y, 40, 10, TFT_WHITE);
                        break;
                }
            } else {
                Serial.println("Hexagram complete, resetting screen.");
                spr.fillScreen(TFT_BLACK);
            }
            
            if (trueLines.length() == 6) {
                Serial.println("Hexagram complete with lines: " + trueLines);
                Hexagram hx;
                file = LittleFS.open("/hexagrams.json", "r");

                if (!file) {
                    Serial.println("Failed to open file");
                    return;
                }

                if (!getHexagramById(file, trueLines, hx)) {
                    Serial.println("Hexagram not found");
                    file.close();
                    return;
                }

                Serial.println("Hexagram found: " + hx.name);

                file.close();

                delay(3000); // Pause before loading the hexagram image

                if (!loadRgb565Bin(hx.image_file.c_str(), imageBuffer, IMAGE_WIDTH * IMAGE_HEIGHT)) {
                    Serial.println("Failed to load image data");
                    return;
                }

                printHexagram(hx, imageBuffer, spr);
            }

            spr.pushSprite(0, 0);
            delay(1000); // Add a delay to prevent multiple triggers in quick succession

            /*Serial.println("Loading Hexagram...");

            int DasID = random(1, 65); // Generate a random ID between 1 and 64
            Hexagram hx;
            file = LittleFS.open("/hexagrams.json", "r");

            if (!file) {
                Serial.println("Failed to open file");
                return;
            }

            if (!getHexagramById(file, DasID, hx)) {
                Serial.println("Hexagram not found");
                file.close();
                return;
            }

            file.close();

            if (!loadRgb565Bin(hx.image_file.c_str(), imageBuffer, IMAGE_WIDTH * IMAGE_HEIGHT)) {
                Serial.println("Failed to load image data");
                return;
            }

            printHexagram(hx, imageBuffer, spr);

            spr.pushSprite(0, 0);

            file = fs::File(); // Clear File Data
            delay(2000);*/
        }

        delay(50);
    }
}