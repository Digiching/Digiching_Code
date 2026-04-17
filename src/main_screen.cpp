// MPU Libraries
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

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


// Define Things
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite( & tft);
ESP32AnalogRead mic;
Adafruit_MPU6050 mpu = Adafruit_MPU6050();

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

/*
void generateHexagramArray(Stream & input) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
    }



    for (JsonObject hexagram: doc["hexagrams"].as < JsonArray > ()) {
        size_t M = doc["hexagrams"].size();
        for (size_t i = 0; i < M; i++) {
            hexagrams[i].id = hexagram["id"];
            Serial.print("ID: ");
            Serial.println((int) hexagram["id"]);

            hexagrams[i].name = hexagram["name"];
            Serial.print("Name: ");
            Serial.println((const char * ) hexagram["name"]);

            hexagrams[i].character = hexagram["character"];
            Serial.print("Character: ");
            Serial.println((const char * ) hexagram["character"]);

            hexagrams[i].phrase = hexagram["phrase"];
            Serial.print("Phrase: ");
            Serial.println((const char * ) hexagram["phrase"]);

            size_t N = hexagram["image"].size();

            unsigned short values[N] PROGMEM;
            for (size_t j = 0; j < N; j++)
                hexagrams[i].image[j] = hexagram["image"][j];
        }
    }
}*/

/*
void getHexagram(int hexNum) {
    // This function will read the hexagram data from the JSON file and return the corresponding image data
    // For now, we will just return a placeholder image
    // In the future, we can implement the actual reading of the JSON file and returning the correct image data

    fs::File file = LittleFS.open("/hexagrams.json", "r");
    if (!file) {
        Serial.println("Failed to open file");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);

    if (!error) {
        int id = doc["id"];
        const char * name = doc["name"];
        const char * character = doc["character"];
        const char * phrase = doc["phrase"];

        size_t N = doc["image"].size();

        unsigned short values[N] PROGMEM;
        for (size_t i = 0; i < N; i++)
            values[i] = doc["image"][i];
    } else {
        Serial.println("Failed to parse JSON");
    }


}
    */

void printHexagram(Hexagram & hexy, uint16_t image[], TFT_eSprite spritz) {

    spritz.fillScreen(TFT_BLACK);
    spritz.setTextSize(1);
    spritz.setTextColor(TFT_WHITE);
    spritz.setCursor(0, 0);

    spritz.setTextDatum(TC_DATUM);
    spritz.drawString(hexy.name, tft.width() / 2, 0);

    spritz.setSwapBytes(true);
    spritz.pushImage(14, 8, IMAGE_WIDTH, IMAGE_HEIGHT, image);
    //spritz.invertDisplay(true);

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
    delay(200);
    Serial.print("Digiching - Main Screen");

    // Initialise MPU Sensor
    mpu.begin();
    Serial.println("MPU6050 Found!");

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
    //printHexagram(1);

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

    delay(50);


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