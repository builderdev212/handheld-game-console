#include <TFT_eSPI.h>
#include <SPI.h>

// Setup display library.
TFT_eSPI tft = TFT_eSPI();

// Include the game.
#include "game.h"

// Define the fonts.
#define sans &FreeSans9pt7b
#define sansBold &FreeSansBold9pt7b

void setup() {
  // Begin the serial monitor for debugging.
  Serial.begin(115200);

  // Select the chip used for the TFT display.
  digitalWrite(15, HIGH);
  // Select the chip used for the SD card.
  digitalWrite(5, HIGH);

  // Setup the TFT display.
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(0);
  // Set the default font.
  tft.setFreeFont(sansBold);

  // Setup a random seed for pseudo random number generation.
  randomSeed(analogRead(0));
}

void loop() {
  Game test;
  test.gameplay();
  delay(1000);
}
