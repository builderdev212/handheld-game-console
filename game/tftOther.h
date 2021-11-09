#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

class bar {
  public:
    void barSetup(TFT_eSPI &_tft, TFT_eSprite &barSprite, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t thk, uint32_t color, uint32_t bg) {
      barX = x;
      barY = y;
      barW = w;
      barH = h;
      borderThk = thk;
      barColor = color;
      bgColor = bg;
      switch (borderThk) {
        case 0:
          isBorder = 0;
          break;
        default:
          isBorder = 1;
          break;
      }
      // Create sprite for speed bar
      barSprite.createSprite(barW - (borderThk * 3), barH - (borderThk * 3));
      // Make sure that the background of the sprite is the same color as the info bar.
      barSprite.fillScreen(barColor);
      // Push the sprite to the screen.
      barSprite.pushSprite(barX + borderThk + (1 * isBorder), barY + borderThk + (1 * isBorder));
    };
    void barState(TFT_eSprite &barSprite, uint16_t barFill) {
      // Fill bar that is set by barFill in pixels.
      barSprite.fillRect(0, 0, barFill, barH - (borderThk * 3), barColor);
      // Fill rest of bar with background color.
      barSprite.fillRect(barFill, 0, barW, barH - (borderThk * 3), bgColor);
      // Output changes to screen.
      barSprite.pushSprite(barX + borderThk + (1 * isBorder), barY + borderThk + (1 * isBorder));
    };
  private:
    uint16_t barX;
    uint16_t barY;
    uint16_t barW;
    uint16_t barH;
    uint16_t borderThk;
    uint32_t barColor;
    uint32_t bgColor;
    uint32_t borderColor;
    uint8_t isBorder;
};

void createBorder(TFT_eSPI &_tft, uint16_t borderX, uint16_t borderY, uint16_t borderW, uint16_t borderH, uint16_t borderThk, uint32_t borderColor) {
  // Create 2 pixel thick border
  for (int i = 1; i <= borderThk; i++) {
    _tft.drawRect(borderX + (i - 1), borderY + (i - 1), borderW - (i - 1) * 2, borderH - (i - 1) * 2, borderColor);
  }
};

void counter(TFT_eSPI &_tft, uint16_t x, uint16_t y, String count, String max_count, uint32_t textColor, uint32_t bgColor) {
  // Clear the place where the coin count is shown.
  _tft.fillRect(x - 1, y - 1, _tft.textWidth(max_count) + 1, _tft.fontHeight() + 1, bgColor);
  // Set the text color and background color.
  _tft.setTextColor(textColor);
  // Draw the string with the given coin count to the screen at the given location.
  _tft.drawString(count, x, y);
};
