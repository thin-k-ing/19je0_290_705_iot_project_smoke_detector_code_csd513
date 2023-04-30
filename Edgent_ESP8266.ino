// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPL1DqriC2y"
#define BLYNK_TEMPLATE_NAME "IoT Smoke and Gas Detector"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#include "BlynkEdgent.h"
#include <MQ2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUZZ      12 //D6 
#define LED       14 //D5

// change this with the pin that you use
int pin = A0;
float lpg, co, smoke;
MQ2 mq2(pin);
int button1 = 0;
int button2 = 0;
SimpleTimer timer;

void setup() {
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  pinMode(BUZZ, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(BUZZ, LOW);
  digitalWrite(LED, LOW);

  // calibrate the device
  mq2.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);
  display.println(" IoT Smoke ");
  display.setCursor(0, 20);
  display.println("  Detector  ");
  display.display();
  delay(1000);
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  timer.run(); // Initiates SimpleTimer
  BlynkEdgent.run();
}

void sendSensorData() {
  float* values = mq2.read(true); //set it false if you don't want to print the values to the Serial
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  lpg = mq2.readLPG();

  if (button1 == 1) {
    // display LPG
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("    LPG   ");
    display.setCursor(10, 30);
    display.print(lpg);
    display.setTextSize(1);
    display.print(" PPM");
    delay(5000);
    display.display();
  } else if (button2 == 1) {
    // display CO
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("    CO    ");
    display.setCursor(10, 30);
    display.print(co);
    display.setTextSize(1);
    display.print(" PPM");
    delay(5000);
    display.display();
  } else {
    // display Smoke
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("   SMOKE   ");
    display.setCursor(10, 30);
    display.print(smoke);
    display.setTextSize(1);
    display.print(" PPM");
    delay(5000);
    display.display();
  }

  Blynk.virtualWrite(V1, smoke);
  Blynk.virtualWrite(V2, lpg);
  Blynk.virtualWrite(V3, co);

  if (smoke > 1 ) {
    Blynk.logEvent("smoke", "Smoke Detected!");
    digitalWrite(BUZZ, HIGH);
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(BUZZ, LOW);
    digitalWrite(LED, LOW);
  }
}


// in Blynk app writes values to the Virtual Pin 4
BLYNK_WRITE(V4) {
  // assigning incoming value from pin V4 to a variable
  button1 = param.asInt();
}

// in Blynk app writes values to the Virtual Pin 5
BLYNK_WRITE(V5) {
  // assigning incoming value from pin V5 to a variable
  button2 = param.asInt();
}