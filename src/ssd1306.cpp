/**
 * u8x8 documentation https://github.com/olikraus/u8g2/tree/master
 * Arduino example for U8X8 "Hello Wold.ino" used as a reference for displaying
 *text on the monochrome sdd1306 ??X?? I2C OLED screen.
 *
 * This requires Arduino library try to get C library to work when time.
 * https://github.com/Matiasus/SSD1306
 **/

// #include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(
//   /* clock=*/SCL, /* data=*/SDA,
//   /* reset=*/U8X8_PIN_NONE);  // OLEDs without Reset of the Display

/*void u8g2_draw(const unsigned short bpm) {
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(1, 0, "BPM");
  u8x8.drawString(1, 0, "1234");
}
*/
