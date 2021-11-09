#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "collisionDetection.h"

class player {
  public:
    // Player position.
    int16_t xVal;
    int16_t yVal;
    // Player size.
    int16_t wVal;
    int16_t hVal;
    // Player shape. 0 - rectangle, 1 - circle.
    int8_t shape;

    /*
      Initialization function. This creates the player and its shadow. The shadow's purpose is to make sure that no pixels of the player are left behind while moving.

      Arguments are as follows: Player sprite, shadow sprite, xBorder, yBorder, wBorder, hBorder, bgColor, xVal, yVal, wVal, hVal, shape, playerColor.
    */
    void createPlayer(TFT_eSprite &playerSprite, TFT_eSprite &shadowSprite, int16_t borderX, int16_t borderY, uint16_t borderW, uint16_t borderH, uint32_t backgroundColor, int16_t playerX, int16_t playerY, uint16_t playerW, uint16_t playerH, uint8_t playerShape, uint32_t playerColor) {
      // Borders that the player must stay in.
      xBorder = borderX;
      yBorder = borderY;
      wBorder = borderW - playerW;
      hBorder = borderH - playerH;
      // Position of the player.
      xVal = playerX;
      yVal = playerY;
      // Player size.
      wVal = playerW;
      hVal = playerH;
      // Player shape.
      shape = playerShape;

      // Create the shadow sprite.
      shadowSprite.createSprite(wVal, hVal);
      // Fill the shadow with the color of the background.
      shadowSprite.fillSprite(backgroundColor);
      // Push the sprite onto the screen at the given x and y value.
      shadowSprite.pushSprite(xVal, yVal);

      // Create the player sprite.
      playerSprite.createSprite(wVal, hVal);
      playerSprite.fillSprite(backgroundColor);

      // Based upon the shape of the player, decide whether to draw a circle or a rectangle.
      switch (shape) {
        case 0:
          // Draw a rectangle.
          playerSprite.fillRect(0, 0, wVal, hVal, playerColor);
        case 1:
          // Draw a circle.
          playerSprite.fillCircle((wVal / 2), (wVal / 2), (wVal / 2)-1, playerColor);
      }
      // Push the player sprite onto the screen.
      playerSprite.pushSprite(xVal, yVal);
    }

    /*
      Deconstruction function. This is used when you are finished using the player.

      Arguments are as follow: Player sprite, shadow sprite.
    */
    void destroyPlayer(TFT_eSprite &playerSprite, TFT_eSprite &shadowSprite) {
      // Delete the sprites to save ram when you are done using the player.
      playerSprite.deleteSprite();
      shadowSprite.deleteSprite();
    }

    /*
      Player movement function. Sets the x and y value and pushes the player to that location.

      Arguments are as follow: Player sprite, shadow sprite, x value, and y value.
    */
    void setPos(TFT_eSprite &playerSprite, TFT_eSprite &shadowSprite, int16_t x, int16_t y) {
      // Push the sprite to the previous location of the player.
      shadowSprite.pushSprite(xVal, yVal);

      // Set the x and y value for the player.
      xVal = x;
      yVal = y;

      // Make sure that the player stays within the predefined borders.
      inBorder();

      // Push the player to that location.
      playerSprite.pushSprite(xVal, yVal);
    }

    /*
      Update the player position. Used for controls, x and y change should be either 0, 1, or -1. Speed determines how much to multiply the change by.

      Arguments are as follow: Player sprite, shadow sprite, speed, x change, and y change.
    */
    void updatePos(TFT_eSprite &playerSprite, TFT_eSprite &shadowSprite, int8_t speed, int8_t xChange, int8_t yChange) {

      // Push the sprite to the previous location of the player.
      shadowSprite.pushSprite(xVal, yVal);

      // Update the x and y value.
      xVal = xVal + (xChange * speed);
      yVal = yVal + (yChange * speed);

      // Make sure that the player stays within the predefined borders.
      inBorder();

      // Push the player to that location.
      playerSprite.pushSprite(xVal, yVal);
    }

    /*
      Make sure that the player stays within the defined border.

      There are no arguments for this function, it simply changes xVal and yVal if they are not within the specified border.
    */
    void inBorder() {
      // Check the x value.
      if (xVal < xBorder) {
        xVal = xBorder;
      } else if (xVal > xBorder + wBorder) {
        xVal = xBorder + wBorder;
      }
      // Check the y value.
      if (yVal < yBorder) {
        yVal = yBorder;
      } else if (yVal > yBorder + hBorder) {
        yVal = yBorder + hBorder;
      }
    }

    /*
      Changes the border that the player must stay in.

      Arguments are as follow: x min, y min, border width, and border height.
    */
    void updateBorder(int16_t borderX, int16_t borderY, uint16_t borderW, uint16_t borderH) {
      // Borders that the player must stay in.
      xBorder = borderX;
      yBorder = borderY;
      wBorder = borderW - wVal;
      hBorder = borderH - hVal;

      // Makes sure the player is within the new border.
      inBorder();
    }

    /*
      Checks to see if the player is colliding with another object.

      shape: 0 - rectangle, 1 - circle
    */
    bool objectCollision(int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint8_t shape0) {
      // Depending on the shape of the player, it changes where the player's x and y values are.
      switch (shape) {
        // If rectangle.
        case 0:
          return collision(xVal, yVal, wVal, hVal, shape, x0, y0, w0, h0, shape0);
        // If circle.
        case 1:
          return collision(xVal - (wVal / 2), yVal - (hVal / 2), wVal, hVal, shape, x0, y0, w0, h0, shape0);
      }
    }

  private:
    // Border values.
    int16_t xBorder;
    int16_t yBorder;
    uint16_t wBorder;
    uint16_t hBorder;
};

class item {
  public:
    // Item position.
    int16_t xVal;
    int16_t yVal;
    // Item size.
    uint16_t wVal;
    uint16_t hVal;
    // Item shape.
    uint8_t shape;

    /*
      Initialization of the item. This gives variables that only need to be declared once their values.

      Arguments are as follow: Background color, item width, item height, and item shape.
    */
    void createItem(uint32_t bgColor, uint16_t itemW, uint16_t itemH, uint8_t itemShape) {
      // Item size.
      wVal = itemW;
      hVal = itemH;
      // Item shape.
      shape = itemShape;
      // Background color.
      backgroundColor = bgColor;
    }

    /*
      Spawns the item at the given location.

      Arguments are as follow: Item sprite, x value, y value, function that contains the code to draw the item to the sprite.
    */
    void spawnItem(TFT_eSprite &itemSprite, int16_t itemX, int16_t itemY, void (*drawItemShape)(TFT_eSprite&)) {
      // Item location.
      xVal = itemX;
      yVal = itemY;

      // Create the item sprite.
      itemSprite.createSprite(wVal, hVal);
      // Fill the background of the sprite.
      itemSprite.fillSprite(backgroundColor);
      // Draw the item to the sprite.
      drawItemShape(itemSprite);
      // Push the sprite to the screen at the given location.
      itemSprite.pushSprite(xVal, yVal);
    }

    /*
      Destroys the item that is spawned.

      Arguments are as follow: Item sprite.
    */
    void destroyItem(TFT_eSprite &itemSprite) {
      // Clear the sprite.
      itemSprite.fillSprite(backgroundColor);
      // Push the cleared sprite to the screen.
      itemSprite.pushSprite(xVal, yVal);
      // Delete the sprite to save ram.
      itemSprite.deleteSprite();
    }
  private:
    // Background color.
    uint32_t backgroundColor;
};

/*
  Checks the speed to be sure it is not above max speed or below min speed.

  Arguments are as follow: speed, min speed, and max speed.
*/
int speedCheck(int16_t currentSpeed, uint16_t minSpeed, uint16_t maxSpeed) {
  if (currentSpeed < minSpeed) {
    currentSpeed = minSpeed;
  } else if (currentSpeed > maxSpeed) {
    currentSpeed = maxSpeed;
  }
  return currentSpeed;
}
