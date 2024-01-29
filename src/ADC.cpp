/*
  data sheet for reference: 
    https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
  useful link: 
    https://embedds.com/adc-on-atmega328-part-1/
*/

#include <avr/io.h>

/**
 * AREF     = Vref pin
 * AVCC     = Vcc pin
 * 
 * Datasheet p.206 for reference block schematic. 
 * ADMUX    = ADC Multiplexer Select
 *    REFS0    = fuse in ADMUX register
 * 
 * ADCSRA   = ADC CTRL and Status Register
 *    ADPS_    = ADC prescaler (hifreq signal to lowfreq by integer division)
 *    ADEN     = Enables ADC module when set in ADCSRA
 *    ADSC     = 
 * 
 * ADC      = Completed A/D-conversion
*/

void initADC() {
  // Select Vref = AVcc (AVcc with external capacitor at AREF pin).
  ADMUX |= (1 << REFS0);  
  // With 16MHz clock we set prescaler to 128 scaling factor with 
  // ADPS0, ADPS1, ADPS2 in ADCSRA register. 
  // => 16000000/128 = 125kHz ADC frequency
  // ADC module is enabled by setting ADEN bit in ADCSRA register. 
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
}

/**
 * 
*/
uint16_t ReadADC(uint8_t ADC_ch) {
  // Select ADC channel with safety mask (0xF0 and 0x0F prevents unintentional alteration of ADMUX).
  ADMUX = (ADMUX & 0xF0) | (ADC_ch & 0x0F);
  // Start single bit conversion by setting ADSC bit in ADCRA. 
  ADCSRA |= (1 << ADSC);
  // Wait for ADC to complete. 
  while(ADCSRA & (1 << ADSC));
  return ADC;
}