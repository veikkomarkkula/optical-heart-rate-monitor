#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "signalProcessing.h"
#include "helperFunctions.h"

/**
 *  Carries out 1D-median filtering for input signal.
 *  New memory is allocated for the filtered signal.
 *  Original signal memory is freed with allocated filter window. 
 *  Parameters:
 *      signal                  = the signal for which filtering is performed. 
 *      mainwindowLength        = length of signal
 *      subWindowLength         = length of the filter window
 *  Returns a filtered signal.
*/
uint16_t* medianFilter(uint16_t* signal, const uint16_t mainWindowLength, const uint8_t subWindowLength) {
    // Allocate memory for filtered signal and filter window.
    uint16_t* filteredSignal = (uint16_t*) calloc(mainWindowLength-subWindowLength+1, sizeof(uint16_t));
    uint16_t* subWindow = (uint16_t*) calloc(subWindowLength+1, sizeof(uint16_t));
    const uint16_t newWindowLength = mainWindowLength-subWindowLength;
    // Calculate new filtered values from "signal" and save them in "filteredSignal".
    for (uint16_t i=0; i<newWindowLength; i++) {
        // Fill filter sub window.
        for (uint8_t j = 0; j<subWindowLength; j++){
           subWindow[j] = signal[i+j];
        }
        // Sort subWindow.
        subWindow = bubbleSort(subWindow, subWindowLength);
        // Calculate median.
        filteredSignal[i] = subWindow[subWindowLength/2]; // Integer division in C rounds towards 0 => 3/2 should be 1.
    }
    // Free memory.
    free(signal);
    free(subWindow);
    return filteredSignal;
}

/**
 *  Carries out moving average filtering for signal. 
 *  New memory is allocated for filteredSignal.
 *  SIGNAL MEMORY IS NOT FREED AS IT IS NEEDED IN SUBSTRACTION.
 *  Only filter window memory is freed. 
 *  Parameters:
 *      signal                  = the signal for which filtering is performed
 *      mainWindowLenght        = length of signal
 *      subWindowLength         = length of the filter window
 * 
 *  Returns a filtered signal.
*/
uint16_t* movingAverageFilter(uint16_t* signal, const uint16_t mainWindowLength, const uint16_t subWindowLength) {
    // Allocate memory for filtered signal and filter window.
    uint16_t* filteredSignal = (uint16_t*) calloc(mainWindowLength-subWindowLength+1, sizeof(uint16_t));
    uint16_t* subWindow = (uint16_t*) calloc(subWindowLength+1, sizeof(uint16_t));
    const uint16_t newWindowLength = mainWindowLength-subWindowLength;
    // Calculate new filtered values from "signal" and save them in "filteredSignal".
    for (uint16_t i=0; i<newWindowLength; i++) {
        // Fill filter sub window.
        for (uint8_t j = 0; j<subWindowLength; j++) {
            subWindow[j] = signal[i+j];
        }
        // Calculate average.
        uint16_t sumSubWindow = 0;
        for (uint8_t k = 0; k < subWindowLength; k++){
            sumSubWindow += subWindow[k];
        }
        filteredSignal[i] = sumSubWindow/subWindowLength;           // Integer division rounds towards 0 in C. 
    }
    // Free memory.
    //free(signal);
    free(subWindow);
    return filteredSignal;
}

/**
 *  signalSubstraction substracts signal s1 from signal s2. 
 *  All index values that go over retLen are ignored.
 *  Parameters: 
 *      s1                      = first signal (longer signal)
 *      s2                      = second signal (shorter MAFsignal)
 *      retLen                  = length of the return signal (ideally equal to the length of the shorter signal and should never be more than that). 
 *  returns signal s1 - s2 of length retLen.
*/
short* signalSubstraction(uint16_t* s1, uint16_t* s2, const uint16_t retLen) {
    short* impulseSignal = (short*) calloc(retLen+1, sizeof(short));
    // Substraction values are saved to s2. 
    for (uint16_t i=0; i<retLen; i++) {
        impulseSignal[i] = s1[i] - s2[i];      // Substraction order shouldn't matter.
    }
    free(s1);
    free(s2);
    return impulseSignal;
}

/**
 *  "differentiation" calculates the derivative of signal. 
 *  Memory is allocated for "diffSignal".
 *  NO MEMORY IS FREED. "signal" IS NEEDED ????.
 *  Delta(t) = 1 reperesents distance between 2 adjacent data points. Time interval is irrelevant at this point.
 *  Parameters:
 *      signal          = singal that is differentiated
 *      len             = length of signal
 *  Returns differentiated signal "diffSignal" of length len-1.
*/
short* differentiation(uint16_t* signal, const uint16_t len) {
    // Allocate memory for the "diffSignal".
    short* diffSignal = (short*) calloc(len, sizeof(short));
    // Differentiat by calculating tangent between 2 adjacent datapoints.
    for (uint16_t i=0; i<len-1; i++) {
        diffSignal[i] = (signal[i+1]-signal[i]);          // Delta(v)/Delta(t), where Delta(t) = 1
    }
    return diffSignal;
}

int calculateBPM(uint16_t* indices, uint16_t count, const uint16_t pointTimeInterval) {
    uint16_t averagePeriodLength = 0;
    for (uint16_t i=0; i<count-1; i++){
        averagePeriodLength += indices[i+1]-indices[i];
    }
    averagePeriodLength = averagePeriodLength/(count-1);
    int BPM = averagePeriodLength * pointTimeInterval/60000;
    return BPM;
}

/**
 *  Peaks and onsets are calculated with following steps:
 *      1. Determine diffSignal zero crossing points. 
 *          - Onset candidates are taken from positive zero crossing. 
 *          - Peak candidates are taken from negative zero crossings.
 *      2. Compare zero crossing points index values with impulse signal.
 *      3. Validate peaks and onsets with "peakToPeakTreshold"
*/
int crossingPoints(uint16_t* signal, short* diffSignal, const uint16_t len, const uint16_t zeroCrossingWindowLength, const uint16_t pointTimeInterval) {

    uint16_t* validOnsetIndices = (uint16_t*) calloc(10, sizeof(uint16_t));    // Expected amount is 1-9;
    uint16_t* validPeakIndices = (uint16_t*) calloc(10, sizeof(uint16_t));     // Expected amount is 1-9;

    //short* zeroCrossingWindow = (short*) calloc(zeroCrossingWindowLength+1, sizeof(short));
    bool rising = false; // false when negative, true whan positive. 
    bool validData = true;

    // These should never go over 9.
    uint16_t onsetCount = 0;
    uint16_t peakCount = 0;

    uint16_t signalAverage = computeSignalAverage(signal, len);
    uint16_t PPAmplitude = maxPeakPeakAmplitude(signal, len);

    for (uint16_t i=0; i < len-1; i++) {
        /*
        // For extra rigidity check if sum of a window changes direction. 
        // If this is used change remember to reduce the amoun of iteretions.
        short diffSignalSum = 0;
        for (uint16_t j=0; j<zeroCrossingWindowLength; j++) {
            diffSignalSum = diffSignal[i+j]
        }
        */
        if (rising == false && diffSignal[i] > 0) {
            uint16_t onsetCandidate = signal[i];
            // OPTIMIZE VADIDATION TRESHOLD!!!!
            if (validateOnset(onsetCandidate, signalAverage, PPAmplitude)) {
                validOnsetIndices[onsetCount] = i;
            }
            rising = true;
        } else if (rising == true && diffSignal[i] < 0) {
            uint16_t peakCandidate = signal[i];
            // OPTIMIZE VADIDATION TRESHOLD!!!!
            if (validatePeak(peakCandidate, signalAverage, PPAmplitude)) {
                validPeakIndices[peakCount] = i;
            }
            rising = false;
        }
        // Check if valid data.
        if (PPAmplitude<50 || onsetCount>9 || peakCount>9) {
            validData = false;
            break;
        }
    }
    int BPM = -1;
    if (validData) {
        BPM = calculateBPM(validPeakIndices, peakCount, pointTimeInterval);
    }
    free(validOnsetIndices);
    free(validPeakIndices);
    free(signal);
    free(diffSignal);
    return BPM;
}

/**
 * Finds peak indices of the "signal" and returns array of all peak indices.
*/
uint16_t* findPeaks(uint16_t* signal, uint16_t signalLen) {
    short* diffSignal = differentiation(signal, signalLen);

    uint16_t* peakIndices = (uint16_t*) calloc(10, sizeof(uint16_t));     // Expected amount is 1-9;
    uint16_t peakCounter = 0;
    for (uint16_t i=0; i < signalLen-2; i++) {
        //  If negative zero corssing (where original signal starts decreasing) is found, add index number to peakIndices.
        if (diffSignal[i] > 0 && diffSignal[i+1]<0 ) {
            peakIndices[peakCounter] = i;
            peakCounter++;
        }
    }
    uint16_t* peakIndicesCorrectLength = (uint16_t*) calloc(peakCounter, sizeof(uint16_t));
    for (uint16_t i=0; i<peakCounter; i++) {
        peakIndicesCorrectLength[i] = peakIndices[i];
    }
    free(peakIndices);
    return peakIndicesCorrectLength;
}