#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * This can be used to collect data to CSV file.
*/
void writeToCSV(uint16_t* signal, const uint16_t signalLen) { 
    FILE *fpt;
    fpt = fopen("MyFile.csv", "w+");
    fprintf(fpt,"Signal Data\n");
    for (uint16_t i=0; i < signalLen; i++) {
        fprintf(fpt,"%d\n", signal[i]);
    }
    fclose(fpt);
}
/**
 * Helper sorting function for medianFilter.
 * Should not be too inefficient with small window sizes. 
 * If longer filtering windows are used, this could be replaced with more effiecion sorting algorithm. 
 * 
 * Returns pointer to the sorted array arr
*/
uint16_t* bubbleSort(uint16_t* arr, uint16_t n) {
    uint16_t temp;
    for (uint8_t i = 0; i < n-1; i++){
        for (uint8_t j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
    return arr;
}

/**
 * Helper function. Computse signal average.
*/
uint16_t computeSignalAverage(uint16_t* signal, uint16_t signalLen) {
    uint32_t signalAverage = 0;
    for (uint16_t i=0; i<signalLen; i++) {
        signalAverage += signal[i];
    }
    signalAverage = signalAverage/signalLen;
    return signalAverage;
}

/**
 * Helper function, that computes maximum peak to peak differenses over entire window.
 * This value is used to calculate the treshold, for peak/onset validation.
*/
uint16_t maxPeakPeakAmplitude(uint16_t* signal, uint16_t windowLength) {
    uint16_t curMin = signal[0];
    uint16_t curMax = signal[0];
    for (uint16_t i = 1; i < windowLength; i++) {
        if (signal[i] < curMin) {
            curMin = signal[i];
        } else if (signal[i] > curMax) {
            curMax = signal[i];
        }
    }
    return curMax-curMin;
}

bool validateOnset(uint16_t onsetCandidate, uint16_t signalAverage, uint16_t PPAmplitude) {
    // Validation treshold for onset "signalAverage-(PPAmplitude/3)".
    if (onsetCandidate < signalAverage-(PPAmplitude/3)) {
        return true;
    }
    return false;
}
bool validatePeak(uint16_t peakCandidate, uint16_t signalAverage, uint16_t PPAmplitude) {
    // Validation treshold "signalAverage+(PPAmplitude/3)".
    if (peakCandidate > signalAverage+(PPAmplitude/3)) {
        return true;
    }
    return false;

}

uint16_t findArrayLength(uint16_t* array) {
    uint16_t length = sizeof(array) / sizeof(array[0]);
    return length;
}