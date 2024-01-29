#include <stdio.h>
#include <stdbool.h>
uint16_t* bubbleSort(uint16_t* arr, uint16_t n);
uint16_t computeSignalAverage(uint16_t* signal, uint16_t signalLen);
uint16_t maxPeakPeakAmplitude(uint16_t* signal, uint16_t windowLength);
bool validateOnset(uint16_t onsetCandidate, uint16_t signalAverage, uint16_t PPAmplitude);
bool validatePeak(uint16_t peakCandidate, uint16_t signalAverage, uint16_t PPAmplitude);

uint16_t findArrayLength(uint16_t* array);