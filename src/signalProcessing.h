#include <stdio.h>
uint16_t* medianFilter(uint16_t* signal, const uint16_t mainWindowLength, const uint8_t subWindowLength);
uint16_t* movingAverageFilter(uint16_t* signal, const uint16_t mainWindowLength, const uint16_t subWindowLength);
short* signalSubstraction(uint16_t* s1, uint16_t* s2, const uint16_t retLen);
short* differentiation(uint16_t* signal, const uint16_t len);
int crossingPoints(uint16_t* signal, short* diffSignal, const uint16_t len, const uint16_t zeroCrossingWindowLengthm, const uint16_t pointTimeInterval);
int calculateBPM(uint16_t* indices, uint16_t count, const uint16_t pointTimeInterval);
uint16_t* findPeaks(uint16_t* signal, uint16_t signalLen);
