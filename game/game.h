#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

// Include other functions created to be used with the TFT_eSPI library.
#include "tftOther.h"

// Include the main game engine.
#include "engine/objects.h"
#include "engine/controls.h"

class Game {
  public:
    void gameplay() {
      gameSetup();
      //mainloop();
      gameOver();
      endLoop();
    }
  private:
    // Background variables.
    const uint32_t backgroundColor = TFT_LIGHTGREY;

    // Info bar variables.
    const uint32_t infoBarColor = TFT_DARKGREY;
    const uint16_t infoBarX = 0;
    const uint16_t infoBarY = 210;
    const uint16_t infoBarW = 320;
    const uint16_t infoBarH = 30;

    // Border variables.
    const uint32_t borderColor = TFT_LIGHTGREY;
    const uint16_t borderThk = 2;

    // Speed bar variables.
    TFT_eSprite speedBarSprite = TFT_eSprite(&tft);
    const uint32_t speedBarColor = TFT_ORANGE;
    const uint16_t speedBarX = 2;
    const uint16_t speedBarY = 212;
    const uint16_t speedBarW = 66;
    const uint16_t speedBarH = 26;
    bar speedBar;

    // Time bar variables.
    TFT_eSprite timeBarSprite = TFT_eSprite(&tft);
    const uint32_t timeBarColor = TFT_BLUE;
    const uint16_t timeBarX = 252;
    const uint16_t timeBarY = 212;
    const uint16_t timeBarW = 66;
    const uint16_t timeBarH = 26;
    bar timeBar;

    // Time variables.
    unsigned long startTime;
    unsigned long currentTime;
    unsigned long timeLeft;

    // Coin counter variables.
    const uint32_t counterColor = TFT_BLACK;
    const uint16_t counterX = speedBarX + speedBarW + speedBarH + 2;
    const uint16_t counterY = speedBarY + 6;
    const uint16_t counterW = 30;
    const uint16_t counterH = 26;

    // Round counter variables.
    const uint32_t roundColor = TFT_BLACK;
    const uint16_t roundX = 160;
    const uint16_t roundY = counterY;

    // Round variables.
    uint16_t roundLength;
    uint8_t roundNumber;
    bool isSpeedRound;

    // Game area variables.
    const uint16_t xMin = 0;
    const uint16_t yMin = 0;
    const uint16_t xMax = 320;
    const uint16_t yMax = infoBarY;

    // Character variables.
    TFT_eSprite charSprite = TFT_eSprite(&tft);
    TFT_eSprite shadowSprite = TFT_eSprite(&tft);
    const uint16_t charDia = 11;
    uint16_t playerColor = TFT_BLUE;
    player player1;

    // Coin variables.
    const uint32_t coinColor = TFT_YELLOW;
    const uint16_t coinDia = 9;
    uint16_t coinMultiplier;
    uint16_t totalCoins;
    uint16_t coinsCollected;
    uint16_t enemyCollected;

    // Coin sprite variables.
    TFT_eSprite coinASprite = TFT_eSprite(&tft);
    TFT_eSprite coinBSprite = TFT_eSprite(&tft);
    TFT_eSprite coinCSprite = TFT_eSprite(&tft);
    TFT_eSprite coinDSprite = TFT_eSprite(&tft);

    // Coin isCollected variables.
    bool isACollected;
    bool isBCollected;
    bool isCCollected;
    bool isDCollected;

    // Coin respawn variables.
    int16_t coinARespawnTime;
    int16_t coinBRespawnTime;
    int16_t coinCRespawnTime;
    int16_t coinDRespawnTime;

    // Coin item class.
    item coinA;
    item coinB;
    item coinC;
    item coinD;

    // Powerup sprite.
    TFT_eSprite powerupSprite = TFT_eSprite(&tft);
    bool isPowerupCollected;
    int16_t powerupRespawn;
    int16_t powerupDespawn;
    item powerup;

    // Powerup variables.
    // 0 - ice, 1 - inverse, 2 - speed, 3 - slowdown, 4 - coin boost
    uint8_t itemType;
    bool isIce;
    bool isInv;
    bool isSpeed;
    bool isSlow;
    bool isBoost;
    int16_t powerupEndTime;
    uint8_t itemTypeChance;

    // Speed Variables.
    uint16_t baseSpeed = 1;
    uint16_t maxSpeed = 6;
    uint16_t minSpeed = 1;
    int16_t currentSpeed;
    int16_t lastSpeed;

    // Game loop variables.
    bool roundMax;
    bool maxCoin;
    bool isWin;

    // End screen.
    uint32_t screenColor;
    bool isEnd;

    void gameSetup() {
      // Set the default values.
      coinsCollected = 0;
      enemyCollected = 0;
      roundLength = 60;
      roundNumber = 1;
      timeLeft = roundLength;
      roundMax = false;
      maxCoin = false;
      isWin = true;
      isSpeedRound = false;

      // Setup player and info bar.
      setupInfoBar();
      player1.createPlayer(charSprite, shadowSprite, xMin, yMin, xMax, yMax, backgroundColor, 0, (yMax / 2) - (charDia / 2), charDia, charDia, 1, playerColor);

      coinA.createItem(backgroundColor, coinDia, coinDia, 1);
      coinB.createItem(backgroundColor, coinDia, coinDia, 1);
      coinC.createItem(backgroundColor, coinDia, coinDia, 1);
      coinD.createItem(backgroundColor, coinDia, coinDia, 1);
      powerup.createItem(backgroundColor, coinDia, coinDia, 0);
    }
    void mainloop() {
      do {
        // Clear the background.
        tft.fillRect(xMin, yMin, xMax, yMax, backgroundColor);

        // Start the timer.
        startTime = millis();
        timeLeft = roundLength;

        // Update round counter.
        roundUpdate(roundNumber);

        // Set the base speed.
        switch (roundNumber % 5) {
          case 0:
            // If speed round, aka every 5th round.
            currentSpeed = maxSpeed;
            isSpeedRound = true;
            break;
          default:
            currentSpeed = baseSpeed;
            isSpeedRound = false;
            break;
        }

        // Update the coin counter.
        coinUpdate(coinsCollected);

        // Set player location to start location.
        player1.setPos(charSprite, shadowSprite, 0, (yMax / 2) - (charDia / 2));

        // Coin setup.
        coinSetup();

        isACollected = false;
        isBCollected = false;
        isCCollected = false;
        isDCollected = false;

        coinMultiplier = 1;

        // Powerup setup.
        powerupSetup();

        isPowerupCollected = false;

        isIce = false;
        isInv = false;
        isSpeed = false;
        isSlow = false;
        isBoost = false;

        powerupDespawn = timeLeft - random(8, 11);

        // Round countdown.
        roundCountdown();

        // Loop for the round.
        while (timeLeft > 0) {
          randomSeed(analogRead(0));
          // Update time bar.
          timeUpdate();
          // Update speed bar.
          currentSpeed = speedCheck(currentSpeed, minSpeed, maxSpeed);
          speedBar.barState(speedBarSprite, currentSpeed * 10);

          // Check for coin collision.
          coinCollisionAction();

          // Check for powerup collision only if it is the speed round.
          if (isSpeedRound == false) {
            powerupCollection();
            isPowerupUse();
          }

          // Update the player position.
          player1.updatePos(charSprite, shadowSprite, currentSpeed, readJoy(0), readJoy(1));

          // If the max coin amount has been reached.
          if (coinsCollected == 300) {
            maxCoin = true;
            break;
          }
          delay(10);
        }

        // If the enemy has collected more coins than you, you lose. Otherwise, continue.
        if (enemyCollected > coinsCollected) {
          isWin = false;
        } else if (enemyCollected <= coinsCollected) {
          isWin = true;
          roundNumber++;
        }

        // If the maximum amount of games have been played, end the the game.
        if (roundNumber > 9) {
          roundMax = true;
        }
        delay(10);
      } while (isWin && !maxCoin && !roundMax);
    }

    // Ran after the time runs out.
    void gameOver() {
      // Delete the sprites when finished to save RAM.
      speedBarSprite.deleteSprite();
      timeBarSprite.deleteSprite();
      coinA.destroyItem(coinASprite);
      coinB.destroyItem(coinBSprite);
      coinC.destroyItem(coinCSprite);
      coinD.destroyItem(coinDSprite);
      powerup.destroyItem(powerupSprite);
      player1.destroyPlayer(charSprite, shadowSprite);
      // End of game animation.
      gameEndAnimation();
    }

    int8_t menuSelected; // 0 - restart, 1 - exit
    int8_t newSelected;

    void drawEndControls() {
      newSelected = menuSelected + readJoy(1);
      if (newSelected > 1) {
        newSelected = 1;
      }
      if (newSelected < 0) {
        newSelected = 0;
      }
      if (newSelected != menuSelected) {
        switch (menuSelected) {
          case 0:
            createBorder(tft, 160 - (tft.textWidth("Respawn") / 2) - 3, (tft.fontHeight() * 2) + 5, tft.textWidth("Respawn") + 6, tft.fontHeight() + 4, 2, TFT_BLACK);
          case 1:
            createBorder(tft, 160 - (tft.textWidth("Menu") / 2) - 3, (tft.fontHeight() * 3) + 13, tft.textWidth("Menu") + 6, tft.fontHeight(), 2, TFT_BLACK);
        }
        delay(5);
        menuSelected = newSelected;
        switch (menuSelected) {
          case 0:
            createBorder(tft, 160 - (tft.textWidth("Respawn") / 2) - 3, (tft.fontHeight() * 2) + 5, tft.textWidth("Respawn") + 6, tft.fontHeight() + 4, 2, TFT_DARKGREY);
          case 1:
            createBorder(tft, 160 - (tft.textWidth("Menu") / 2) - 3, (tft.fontHeight() * 3) + 13, tft.textWidth("Menu") + 6, tft.fontHeight(), 2, TFT_DARKGREY);
        }
      } 
    }

    void endLoop() {
      switch (menuSelected) {
        case 0:
          createBorder(tft, 160 - (tft.textWidth("Respawn") / 2) - 3, (tft.fontHeight() * 2) + 5, tft.textWidth("Respawn") + 6, tft.fontHeight() + 4, 2, TFT_DARKGREY);
        case 1:
          createBorder(tft, 160 - (tft.textWidth("Menu") / 2) - 3, (tft.fontHeight() * 3) + 13, tft.textWidth("Menu") + 6, tft.fontHeight(), 2, TFT_DARKGREY);
      }
      newSelected = menuSelected;

      while (isEnd == true) {
        drawEndControls();

        delay(10);
      }
    }

    void gameEndAnimation() {
      if (isWin == true) {
        screenColor = TFT_BLUE;
      } else if (isWin == false) {
        screenColor = TFT_RED;
      }
      for (int y = 0; y < 121; y += 2) {
        tft.fillRect(0, 0, 320, y, screenColor);
        tft.fillRect(0, 240 - y, 320, y, screenColor);
        delay(10);
      }
      tft.setTextColor(TFT_BLACK);
      if (isWin == true) {
        tft.drawString(String("You Win!"), 160 - (tft.textWidth("You Win!") / 2), 1);
      } else if (isWin == false) {
        tft.drawString(String("You Lose..."), 160 - (tft.textWidth("You Lose...") / 2), 1);
      }
      tft.drawString(String("Coins Collected: "), 160 - (tft.textWidth("Coins Collected: ")), 1 + tft.fontHeight() + 2);
      tft.drawString(String(coinsCollected), 164, 1 + tft.fontHeight() + 2);
      tft.drawString(String("Respawn"), 160 - (tft.textWidth("Respawn") / 2), (tft.fontHeight() * 2) + 8);
      tft.drawString(String("Menu"),  160 - (tft.textWidth("Menu") / 2), (tft.fontHeight() * 3) + 16);

      createBorder(tft, 160 - (tft.textWidth("Respawn") / 2) - 3, (tft.fontHeight() * 2) + 5, tft.textWidth("Respawn") + 6, tft.fontHeight() + 4, 2, TFT_BLACK);
      createBorder(tft, 160 - (tft.textWidth("Menu") / 2) - 3, (tft.fontHeight() * 3) + 13, tft.textWidth("Menu") + 6, tft.fontHeight(), 2, TFT_BLACK);
      isEnd = true;
      menuSelected = 0;
    }

    // Setup the info bar.
    void setupInfoBar() {
      // Create info bar background.
      tft.fillRect(infoBarX, infoBarY, infoBarW, infoBarH, infoBarColor);
      // Create speed bar.
      createBorder(tft, speedBarX, speedBarY, speedBarW, speedBarH, borderThk, borderColor);
      speedBar.barSetup(tft, speedBarSprite, speedBarX, speedBarY, speedBarW, speedBarH, borderThk, speedBarColor, infoBarColor);
      // Set the bar state to the base speed.
      speedBar.barState(speedBarSprite, baseSpeed);
      // Create coin icon.
      tft.fillCircle(speedBarX + speedBarW + 2 + speedBarH / 2, speedBarY + speedBarH / 2, speedBarH / 3, TFT_YELLOW);
      // Create coin counter.
      coinUpdate(coinsCollected);
      // Create round counter.
      roundUpdate(roundNumber);
      // Create time bar.
      createBorder(tft, timeBarX, timeBarY, timeBarW, timeBarH, borderThk, borderColor);
      timeBar.barSetup(tft, timeBarSprite, timeBarX, timeBarY, timeBarW, timeBarH, borderThk, timeBarColor, infoBarColor);
      // Fill time bar.
      timeBar.barState(timeBarSprite, timeLeft / (roundLength / 60));
    }

    // Round Counter Functions.
    void roundCountdown() {
      // Make sure that the coin count shown will not exceed the amount able to be displayed.
      for (int count = 3; count > 0; count--) {
        counter(tft, (xMax / 2) - (tft.textWidth(String(count)) / 2), (yMax / 2) - (tft.fontHeight() / 2), String(count), String("0"), TFT_RED, backgroundColor);
        delay(1000);
      }
      tft.fillRect((xMax / 2) - (tft.textWidth(String(1)) / 2), (yMax / 2) - (tft.fontHeight() / 2), tft.textWidth(String("0")), tft.fontHeight(), backgroundColor);
      counter(tft, (xMax / 2) - (tft.textWidth(String("999999")) / 2), (yMax / 2) - (tft.fontHeight() / 2), String("Start!"), String("99"), TFT_RED, backgroundColor);
      delay(500);
      tft.fillRect((xMax / 2) - (tft.textWidth(String("999999")) / 2), (yMax / 2) - (tft.fontHeight() / 2), tft.textWidth("Start!"), tft.fontHeight(), backgroundColor);
    }

    void roundUpdate(uint16_t roundCount) {
      // Make sure the round count can't exceed the max amount of rounds.
      if (roundCount > 9) {
        roundCount = 9;
      }
      counter(tft, roundX - (tft.textWidth(String(roundCount)) / 2), roundY, String(roundCount), String("9"), roundColor, infoBarColor);
    }

    // Time Bar functions.
    void timeUpdate() {
      // Get the current time.
      currentTime = millis();
      // Calculate the time left.
      timeLeft = roundLength - ((currentTime - startTime) / 1000);
      // Update the bar state shown.
      timeBar.barState(timeBarSprite, timeLeft / (roundLength / 60));
    }

    //Coin functions.
    void coinSetup() {
      // Destroy existing coins.
      coinA.destroyItem(coinASprite);
      coinB.destroyItem(coinBSprite);
      coinC.destroyItem(coinCSprite);
      coinD.destroyItem(coinDSprite);
      // Spawn coins.
      coinA.spawnItem(coinASprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawCoin);
      coinB.spawnItem(coinASprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawCoin);
      coinC.spawnItem(coinASprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawCoin);
      coinD.spawnItem(coinASprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawCoin);
    }

    void coinUpdate(uint16_t coinCount) {
      // Make sure that the coin count shown will not exceed the amount able to be displayed.
      if (coinCount > 999) {
        coinCount = 999;
      }
      counter(tft, counterX, counterY, String(coinCount), String("999"), counterColor, infoBarColor);
    }

    void coinCollected(bool &isCollected, int16_t &respawnTime, player &player, item &coin, TFT_eSprite &coinSprite) {
      if (isCollected == false) {
        // Check to see if the player is touching the object.
        if (player.objectCollision(coin.xVal, coin.yVal, coinDia + (3 * currentSpeed), coinDia + (3 * currentSpeed), 1) == true) {
          // Add to the coin counter.
          coinsCollected += (1 * coinMultiplier);
          // Update the coin counter.
          coinUpdate(coinsCollected);
          // Set collected to true.
          isCollected = true;
          // Destroy the item.
          coin.destroyItem(coinSprite);
          // Set the respawn time.
          respawnTime = timeLeft - random(1, 3);
        }
      } else if (isCollected == true) {
        // When time to respawn.
        if (timeLeft == respawnTime) {
          // Set collected to false.
          isCollected = false;
          // Spawn the coin.
          coin.spawnItem(coinSprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawCoin);
        }
      }
    }

    void coinCollisionAction() {
      coinCollected(isACollected, coinARespawnTime, player1, coinA, coinASprite);
      coinCollected(isBCollected, coinBRespawnTime, player1, coinB, coinBSprite);
      coinCollected(isCCollected, coinCRespawnTime, player1, coinC, coinCSprite);
      coinCollected(isDCollected, coinDRespawnTime, player1, coinD, coinDSprite);
    }


    // Powerup functions.
    void powerupSetup() {
      powerup.destroyItem(powerupSprite);
      if (isSpeedRound == false) {
        spawnRandomItem();
      }
    }

    void powerupCollection() {
      if (isPowerupCollected == false) {
        // If touching the powerup.
        if (player1.objectCollision(powerup.xVal, powerup.yVal, coinDia + (3 * currentSpeed), coinDia + (3 * currentSpeed), 0) == true) {
          switch (itemType) {
            case 0:
              // If ice powerup is in use.
              isIce = true;
              lastSpeed = currentSpeed;
              // Set end time.
              powerupEndTime = timeLeft - random(1, 4);
              break;
            case 1:
              // If inverse powerup is in use.
              isInv = true;
              lastSpeed = currentSpeed;
              // Set end time.
              powerupEndTime = timeLeft - random(4, 7);
              break;
            case 2:
              // If speed powerup is activated.
              isSpeed = true;
              break;
            case 3:
              // If slowdown powerup is activated.
              isSlow = true;
              break;
            case 4:
              // If coin boost powerup is activated.
              isBoost = true;
              lastSpeed = currentSpeed;
              // Set end time.
              powerupEndTime = timeLeft - random(5, 9);
              break;
            default:
              break;
          }
          isPowerupCollected = true;
          // Destroy item.
          powerup.destroyItem(powerupSprite);
          // Set powerup respawn time.
          powerupRespawn = timeLeft - random(5, 8);
        }
      } else if (isPowerupCollected == true) {
        if (timeLeft == powerupRespawn) {
          isPowerupCollected = false;
          // Spawn item.
          spawnRandomItem();
          powerupDespawn = timeLeft - random(8, 11);
        }
      }
      if (timeLeft == powerupDespawn) {
        isPowerupCollected = true;
        // Destroy item.
        powerup.destroyItem(powerupSprite);
        // Set powerup respawn time.
        powerupRespawn = timeLeft - random(5, 8);
      }
    }

    void isPowerupUse() {
      if (isIce == true) {
        if (powerupEndTime == timeLeft) {
          currentSpeed = lastSpeed;
          isIce = false;
        } else {
          currentSpeed = 0;
        }
      }
      if (isInv == true) {
        if (powerupEndTime == timeLeft) {
          currentSpeed = lastSpeed;
          isInv = false;
        } else {
          currentSpeed = -1;
        }
      }
      if (isSpeed == true) {
        currentSpeed++;
        isSpeed = false;
      }
      if (isSlow == true) {
        currentSpeed--;
        if (currentSpeed < 1) {
          currentSpeed = 1;
        }
        isSlow = false;
      }
      if (isBoost == true) {
        if (powerupEndTime == timeLeft) {
          coinMultiplier = 1;
          currentSpeed = lastSpeed;
          isBoost = false;
        } else {
          coinMultiplier = 2;
          currentSpeed /= 2;
          if (currentSpeed < 1) {
            currentSpeed = 1;
          }
        }
      }
    }

    void spawnRandomItem() {
      itemTypeChance = random(0, 20);
      if (itemTypeChance < 5) {
        powerup.spawnItem(powerupSprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawIce);
        itemType = 0;
      } else if (itemTypeChance < 10 && itemTypeChance > 4) {
        powerup.spawnItem(powerupSprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawInv);
        itemType = 1;
      } else if (itemTypeChance > 9 && itemTypeChance < 12) {
        powerup.spawnItem(powerupSprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawSpeedBoost);
        itemType = 2;
      } else if (itemTypeChance > 11 && itemTypeChance < 17) {
        powerup.spawnItem(powerupSprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawSlow);
        itemType = 3;
      } else if (itemTypeChance > 16 && itemTypeChance < 20) {
        powerup.spawnItem(powerupSprite, random(xMin, xMax - coinDia), random(yMin, yMax - coinDia), drawCoinBoost);
        itemType = 4;
      }
    }

    static void drawCoin(TFT_eSprite &itemSprite) {
      itemSprite.fillCircle(9 / 2, 9 / 2, 9 / 2 - 1, TFT_YELLOW);
    }
    static void drawIce(TFT_eSprite &itemSprite) {
      itemSprite.fillRect(0, 3, 7, 9, TFT_BLUE);
      itemSprite.fillRect(1, 1, 7, 7, TFT_SKYBLUE);
    }
    static void drawInv(TFT_eSprite &itemSprite) {
      itemSprite.fillTriangle(0, 4, 4, -1, 4, 9, TFT_RED);
      itemSprite.fillTriangle(4, -1, 8, 4, 4, 9, TFT_BLUE);
      itemSprite.drawFastVLine(4, 0, 9, TFT_BLACK);
    }
    static void drawSpeedBoost(TFT_eSprite &itemSprite) {
      itemSprite.fillRect(0, 3, 9, 3, TFT_GREEN);
      itemSprite.fillRect(3, 0, 3, 9, TFT_GREEN);
    }
    static void drawSlow(TFT_eSprite &itemSprite) {
      itemSprite.fillRect(0, 3, 9, 3, TFT_RED);
    }
    static void drawCoinBoost(TFT_eSprite &itemSprite) {
      itemSprite.fillCircle(9 / 2, 9 / 2, 9 / 2, TFT_GREEN);
    }
};
