/**
  First demo for FT6206 Capactive Touch Screen on Wokwi. Enjoy!

  https://wokwi.com/arduino/projects/311598148845830720
 */

/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341
  captouch shield
  ----> http://www.adafruit.com/products/1947

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>  // this is needed for display
#include <Wire.h> // this is needed for FT6206
#include <XPT2046.h>

XPT2046 touch(/*cs=*/2, /*irq=*/16);

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library with default width and height

#define ILI9341_RED TFT_RED
#define ILI9341_YELLOW TFT_YELLOW
#define ILI9341_GREEN TFT_GREEN
#define ILI9341_CYAN TFT_CYAN
#define ILI9341_BLUE TFT_BLUE
#define ILI9341_MAGENTA TFT_MAGENTA

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

static void calibratePoint(uint16_t x, uint16_t y, uint16_t &vi, uint16_t &vj) {
  // Draw cross
  ucg.setColor(0xff, 0xff, 0xff);
  ucg.drawHLine(x - 8, y, 16);
  ucg.drawVLine(x, y - 8, 16);
  while (!touch.isTouching()) {
    delay(10);
  }
  touch.getRaw(vi, vj);
  // Erase by overwriting with black
  ucg.setColor(0, 0, 0);
  ucg.drawHLine(x - 8, y, 16);
  ucg.drawVLine(x, y - 8, 16);
}

void calibrate() {
  uint16_t x1, y1, x2, y2;
  uint16_t vi1, vj1, vi2, vj2;
  touch.getCalibrationPoints(x1, y1, x2, y2);
  calibratePoint(x1, y1, vi1, vj1);
  delay(1000);
  calibratePoint(x2, y2, vi2, vj2);
  touch.setCalibration(vi1, vj1, vi2, vj2);

  char buf[80];
  snprintf(buf, sizeof(buf), "%d,%d,%d,%d", (int)vi1, (int)vj1, (int)vi2, (int)vj2);
  ucg.setFont(ucg_font_helvR14_tr);
  ucg.setColor(0xff, 0xff, 0xff);
  ucg.setPrintPos(0, 25);
  ucg.print("setCalibration params:");
  ucg.setPrintPos(0, 50);
  ucg.print(buf);
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));

  tft.begin();
  touch.begin(tft.width(), tft.height()); // Must be done before setting rotation

  Serial.println("Capacitive touchscreen started");

  tft.fillScreen(ILI9341_BLACK);

  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);

  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
}

void loop()
{
  // Wait for a touch
  if (touch.isTouching())
  {

    // Retrieve a point
    uint16_t x, y;
    touch.getPosition(x, y);

    /*
     // Print out raw data from screen touch controller
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print(" -> ");
    */

    // flip it around to match the screen.
    x = map(x, 0, 240, 240, 0);
    y = map(y, 0, 320, 320, 0);

    // Print out the remapped (rotated) coordinates
    Serial.print("(");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.println(")");

    if (y < BOXSIZE)
    {
      oldcolor = currentcolor;

      if (x < BOXSIZE)
      {
        currentcolor = ILI9341_RED;
        tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
      }
      else if (x < BOXSIZE * 2)
      {
        currentcolor = ILI9341_YELLOW;
        tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
      }
      else if (x < BOXSIZE * 3)
      {
        currentcolor = ILI9341_GREEN;
        tft.drawRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
      }
      else if (x < BOXSIZE * 4)
      {
        currentcolor = ILI9341_CYAN;
        tft.drawRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
      }
      else if (x < BOXSIZE * 5)
      {
        currentcolor = ILI9341_BLUE;
        tft.drawRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
      }
      else if (x <= BOXSIZE * 6)
      {
        currentcolor = ILI9341_MAGENTA;
        tft.drawRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
      }

      if (oldcolor != currentcolor)
      {
        if (oldcolor == ILI9341_RED)
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
        if (oldcolor == ILI9341_YELLOW)
          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
        if (oldcolor == ILI9341_GREEN)
          tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
        if (oldcolor == ILI9341_CYAN)
          tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
        if (oldcolor == ILI9341_BLUE)
          tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
        if (oldcolor == ILI9341_MAGENTA)
          tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
      }
    }
    if (((y - PENRADIUS) > BOXSIZE) && ((y + PENRADIUS) < tft.height()))
    {
      tft.fillCircle(x, y, PENRADIUS, currentcolor);
    }
  }
}