// MPU Libraries
#include <Adafruit_LSM6DSOX.h>

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

// Define Things
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite( & tft);
ESP32AnalogRead mic;
Adafruit_LSM6DSOX sox;

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

bool getHexagramById(Stream & input, int wantedId, Hexagram & out) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }

    for (JsonObject hexagram: doc["hexagrams"].as < JsonArray > ()) {
        int id = hexagram["id"] | -1;

        if (id == wantedId) {
            out.id = id;
            out.name = hexagram["name"].as <
                const char * > ();
            out.character = hexagram["character"].as <
                const char * > ();
            out.image_file = hexagram["image_file"].as <
                const char * > ();
            out.phrase = hexagram["phrase"].as <
                const char * > ();
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
    if (!Serial)
    {  
        delay(10);
    }
    Serial.print("Digiching - Main Screen");

    

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

    // LittleFS Setup

    // Mount LittleFS before opening any files
    if (!LittleFS.begin(true)) {
        Serial.println("An error occurred while mounting LittleFS");
        return;
    }

    // Create and open a file for writing


    // ArduinoJson Setup

    /*JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);

    if (!error) {
        int id = doc["id"];
        const char * name = doc["name"];
        const char * character = doc["character"];
        const char * phrase = doc["phrase"];

        JsonArray data = doc["data"];
    } else {
        Serial.println("Failed to parse JSON");
    }*/

    Serial.println("Done Initialisation");
    delay(50);
    // 
}

void loop() {
    //  /* Get a new normalized sensor event */
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent( & accel, & gyro, & temp);

    Serial.print("\t\tTemperature ");
    Serial.print(temp.temperature);
    Serial.println(" deg C");

    /* Display the results (acceleration is measured in m/s^2) */
    Serial.print("\t\tAccel X: ");
    Serial.print(accel.acceleration.x);
    Serial.print(" \tY: ");
    Serial.print(accel.acceleration.y);
    Serial.print(" \tZ: ");
    Serial.print(accel.acceleration.z);
    Serial.println(" m/s^2 ");

    /* Display the results (rotation is measured in rad/s) */
    Serial.print("\t\tGyro X: ");
    Serial.print(gyro.gyro.x);
    Serial.print(" \tY: ");
    Serial.print(gyro.gyro.y);
    Serial.print(" \tZ: ");
    Serial.print(gyro.gyro.z);
    Serial.println(" radians/s ");
    Serial.println();

    delay(100);


    //printHexagram(1);
    /*
    if (Serial.available() > 0) {
        Serial.print("Enter ID: ");
        String command = Serial.readString();
        int DasID = command.toInt();

        Serial.println("You entered: " + command);

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

        hx = Hexagram(); // Clear Hexagram Data
        file = fs::File(); // Clear File Data
    }

    delay(50);*/


    /*sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    

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
    spr.println(String(mic.readVoltage()));*/

}