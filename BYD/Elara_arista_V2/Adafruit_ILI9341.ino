//--------------------------------------------BBK--------------------------------------------------//
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Beyond_Icons.h"

// Regular FreeSans fonts
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

// Bold FreeSans fonts
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

// Custom font you added yourself
#include "FreeSansBold10pt7b.h"
#include "RobotoMono_Regular.h"
//---------------------------------------------------------------------------------------------------//
#define TFT_CS 5    // Chip select pin
#define TFT_DC 4    // Data/Command pin
#define TFT_RST 14  //13                // Reset pin
#define TFT_BL 2    // Backlight pin
#define RGB565(r, g, b) (((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F))
#define ILI9341_CUSTOM_BEIGE 0xE56F
#define ILI9341_CUSTOM_BEIGE_DARKER 0xCC6D
//---------------------------------------------------------------------------------------------------//
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
//-----------------------------------------Declaration-Function--------------------------------------//
void Display_Init_9341();
void display_text_center(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_text(int16_t x, int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value_2_digit(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_frequency(int16_t x, int16_t y, float frequency, uint16_t text_color, uint16_t bg, uint8_t text_size);
void turnOffDisplay();
void turnOnDisplay();
void reinitializeDisplay();
void display_value_float(int16_t x, int16_t y, float value, uint16_t text_color, uint16_t bg, uint8_t text_size, uint8_t decimal_places);
void display_value_center(int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void wipeScreenFromCenter(uint16_t color);
void display_text_center_2(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value_2(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_text_2(int16_t x, int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_value_2_digit_2(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, const GFXfont *font);
void display_frequency_2(int16_t x, int16_t y, float frequency, uint16_t text_color, uint16_t bg, uint8_t text_size);
void display_text_center_MAC(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size);

//--------------------------------------Function Defnition-------------------------------------------//

void Display_Init_9341() {
  tft.begin();
  tft.setRotation(1);
  wipeScreenFromCenter(ILI9341_BLACK);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  delay(1);
  //  wipeScreenFromCenter(ILI9341_BLACK);
  tft.setFont(&FreeSansBold10pt7b);
  tft.drawRGBBitmap(70, 20, (uint16_t *)ui_2_Logo_3, 200, 200);
  delay(2000);
  wipeScreenFromCenter(ILI9341_BLACK);
  display_text_center(125, "Hey!!", ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
}
//---------------------------------------------------------------------------------------------------//
void display_text_center(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold10pt7b);
  tft.setTextSize(text_size);  // Adafruit uses single-parameter setTextSize
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  x1 = (tft.width() - w) / 2;
  tft.setCursor(x1, y);
  tft.setTextColor(text_color, bg);
  tft.print(text);
}
//---------------------------------------------------------------------------------------------------//
void display_value(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold10pt7b);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(value);
}
//---------------------------------------------------------------------------------------------------//
void display_text(int16_t x, int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold10pt7b);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(text);
}
//---------------------------------------------------------------------------------------------------//
void display_value_2_digit(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold10pt7b);
  char buffer[4];
  sprintf(buffer, "%02u", value);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(buffer);
}
//---------------------------------------------------------------------------------------------------//
void display_frequency(int16_t x, int16_t y, float frequency, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold10pt7b);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(frequency, 1);
  tft.print(" MHz");
}
//---------------------------------------------------------------------------------------------------//
void turnOffDisplay() {
  tft.writeCommand(ILI9341_DISPOFF);  // Turn off the display
}
//---------------------------------------------------------------------------------------------------//
void turnOnDisplay() {
  tft.writeCommand(ILI9341_DISPON);  // Turn on the display
}
//---------------------------------------------------------------------------------------------------//
void reinitializeDisplay() {
  // Reset the display hardware
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(10);
  digitalWrite(TFT_RST, HIGH);
  delay(10);

  // Reinitialize the display
  tft.begin();
  tft.setRotation(1);          // Set the desired orientation
  pinMode(TFT_BL, OUTPUT);     // Backlight control
  digitalWrite(TFT_BL, HIGH);  // Ensure backlight is on
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSansBold10pt7b);
  machine_state = home_screen_init;
}
//---------------------------------------------------------------------------------------------------//
void display_value_float(int16_t x, int16_t y, float value, uint16_t text_color, uint16_t bg, uint8_t text_size, uint8_t decimal_places) {
  tft.setFont(&FreeSansBold10pt7b);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(value, decimal_places);  // Specify decimal places
}
//---------------------------------------------------------------------------------------------------//
void display_value_center(int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold10pt7b);
  char buffer[10];               // Adjust size if you're displaying larger numbers
  sprintf(buffer, "%u", value);  // Convert value to string

  tft.setTextSize(text_size);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (tft.width() - w) / 2;

  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.print(buffer);
}
//---------------------------------------------------------------------------------------------------//
void wipeScreenFromCenter(uint16_t color) {
  tft.setFont(&FreeSansBold10pt7b);
  int16_t mid = tft.width() / 2;
  for (int16_t offset = 0; offset <= mid; offset++) {
    // draw to the right side
    tft.drawFastVLine(mid + offset, 0, tft.height(), color);
    // draw to the left side
    tft.drawFastVLine(mid - offset, 0, tft.height(), color);
    delay(1);  // adjust/remove for speed
  }
}
//---------------------------------------------------------------------------------------------------//
void display_text_center_2(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold12pt7b);
  tft.setTextSize(text_size);  // Adafruit uses single-parameter setTextSize
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  x1 = (tft.width() - w) / 2;
  tft.setCursor(x1, y);
  tft.setTextColor(text_color, bg);
  tft.print(text);
}
//---------------------------------------------------------------------------------------------------//
void display_value_2(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(value);
}
//---------------------------------------------------------------------------------------------------//
void display_text_2(int16_t x, int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(text);
}
//---------------------------------------------------------------------------------------------------//
void display_value_2_digit_2(int16_t x, int16_t y, uint16_t value, uint16_t text_color, uint16_t bg, const GFXfont *font) {
  // Set custom font
  tft.setFont(font);

  // Format number as 2 digits (e.g., 05, 12)
  char buffer[4];
  sprintf(buffer, "%02u", value);

  // Draw text
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.print(buffer);

  // Reset font if needed (optional)
  // tft.setFont(); // restores built-in small font
}
//---------------------------------------------------------------------------------------------------//
void display_frequency_2(int16_t x, int16_t y, float frequency, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.setTextSize(text_size);
  tft.print(frequency, 1);
  tft.print(" MHz");
}
//---------------------------------------------------------------------------------------------------//
void display_text_center_MAC(int16_t y, const char *text, uint16_t text_color, uint16_t bg, uint8_t text_size) {
  char buffer[32];
  strncpy(buffer, text, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  for (int i = 0; buffer[i]; i++) {
    buffer[i] = toupper((unsigned char)buffer[i]);
  }
  tft.setFont(&RobotoMono_Regular12pt7b);
  tft.setTextSize(text_size);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (tft.width() - w) / 2;
  tft.setCursor(x, y);
  tft.setTextColor(text_color, bg);
  tft.print(buffer);
}
//---------------------------------------------------------------------------------------------------//
