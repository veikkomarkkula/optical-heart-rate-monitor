// AVR includes
#include <avr/io.h>
#include <util/delay.h>
// Oled includes.
// #include <Arduino.h>
// #include <SPI.h>
#include <U8x8lib.h>
// #include <Wire.h>
//  Other includes.
#include <ADC.h>
#include <signalProcessing.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// My headers
#include "helperFunctions.h"
#include "ssd1306.h"

// External clock pins
#define XTAL1 PB6
#define XTAL2 PB7
// Data pins:
#define SENSOR_DATA PC0  // Analog sensor data
// Display pins:
#define SDA PC4  // OLED data
#define SCL PC5  // OLED clock
// Programming pins
#define NRST PC6  // Reset pin
#define MOSI PB3
#define MISO PB4
#define SCK PB5

// 50Hz sampler rate
// Parameters
const uint16_t mainWindowLength = 250;
const uint16_t MF_winLen = 3;  // Median filter 1 window size.
const uint16_t MAF_winLen_wide =
    50;  // Moving average filter 1 window size (wide window).
const uint16_t MAF_winLen_thin =
    5;  // Moving average filter 2 window size (thin window).
const uint16_t pointTimeInterval = 20;  // ms
const uint16_t zeroCrossingWindowLength =
    5;  // From how many data points zero crossings are determined.

// Display function.
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(
    /* clock=*/SCL, /* data=*/SDA,
    /* reset=*/U8X8_PIN_NONE);  // OLEDs without Reset of the Display
// U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /*
// reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards +
// FeatherWing OLED

uint16_t* ADCreadWindow() {
  // ADC windowing here.
  uint16_t* dataWindow =
      (uint16_t*)calloc((mainWindowLength + 1), sizeof(uint16_t));
  for (uint16_t i = 0; i < mainWindowLength; i++) {
    uint16_t sensorValue = ReadADC(SENSOR_DATA);
    dataWindow[i] = sensorValue;
    _delay_ms(pointTimeInterval);
  }
  return dataWindow;
}

// Draw output
// WITH THIS FUNCTION HERE "ssd1306.cpp" IS NOT NEEDED AND "ssd1306.h" IS
// COMMENTED.
void u8g2_draw(int BPM) {
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(1, 0, "BPM");
  //u8x8.print(BPM);
  u8x8.drawString(1, 1, String(BPM).c_str());
}
void u8g2_test_draw(int BPM) {
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(1, 0, "test");
  //u8x8.print(BPM);
  u8x8.drawString(1, 1, String(BPM).c_str());
}

int main() {
  initADC();
  // OLED setup
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8g2_test_draw(1234);   
  while (1) {
    uint16_t* signal = ADCreadWindow();
    uint16_t* MFsignal = medianFilter(signal, mainWindowLength, MF_winLen);
    uint16_t* MAFsignal = movingAverageFilter(
        MFsignal, (mainWindowLength - MF_winLen), MAF_winLen_wide);
    uint16_t* peakIndices =
        findPeaks(MAFsignal, (mainWindowLength - MF_winLen - MAF_winLen_wide));
    int BPM = calculateBPM(peakIndices, findArrayLength(peakIndices),
                           pointTimeInterval);
    u8g2_draw(BPM);

    // Alternative implementation with more signal processing.
    /*
    // Read signal (ADC)
    uint16_t* signal =            ADCreadWindow();
    // Median filtering. Window length decreases by MF_winLen.
    uint16_t* MFsignal =          medianFilter(signal, mainWindowLength,
    MF_winLen);
    // Moving average filter. Window length further decreases by
    MAF_winLen_wide. uint16_t* MAFsignal =         movingAverageFilter(MFsignal,
    (mainWindowLength-MF_winLen), MAF_winLen_wide);     // Do not free memory
    yet.
    // Create impulse signal from shifted and smoothed MAFsignal and MFsignal.
    Window length is preserved. short* impulseSignal =
    signalSubstraction(MFsignal, MAFsignal,
    (mainWindowLength-MF_winLen-MAF_winLen_wide));    // -MF_winLen from
    everything?
    // Second moviing average filter. Window length decreases by MAF_winLen.
    uint16_t* MAFimpulseSignal =  movingAverageFilter(impulseSignal,
    (mainWindowLength-MF_winLen-MAF_winLen_wide), MAF_winLen_thin);
    // Signal is differentiated. Window length decrases by 1.
    short* diffSignal =        differentiation(MAFimpulseSignal,
    (mainWindowLength-MAF_winLen_wide-MAF_winLen_thin));
    // Validate onsets and peaks and calculate the BPM from period length.
    int BPM = crossingPoints(MAFimpulseSignal, diffSignal,
    (mainWindowLength-MAF_winLen_wide-MAF_winLen_thin),
    zeroCrossingWindowLength, pointTimeInterval);
    // Output
    */
  }
}