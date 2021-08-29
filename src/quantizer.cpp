#include "main.h"

// C-Major scale
// const int selectedNotes[] = {
//     1, // A    0
//     0, // A#   1
//     1, // B    2
//     1, // C    3
//     0, // C#   4
//     1, // D    5
//     0, // D#   6
//     1, // E    7
//     1, // F    8
//     0, // F#   9
//     1, // G    10
//     0 // G#    11
// };

const byte majorScale[] = { 0, 2, 3, 5, 7, 8, 10 };


// float interval = 0.08333;
// int inputThresholds[] = {
// };

// const int outputDacValues = {
//     0
//     341
//     683
//     1024
//     ?
//     ?
//     2048
//     ?
//     ?
//     3072
//     ?
//     ?
//     4095
// };

// Test Cases
// 0V -> 0 ADC -> 0 semitones -> 0 DAC -> 0V
// 0.5V -> 102 ADC -> 6 semitones -> 410 DAC -> 0.5V
// 1V -> 205 ADC -> 12 semitones -> 819 DAC -> 1V


uint16_t outputValues[12];

void QuantizerInit() {
    // unsigned long outputMicroVolts[12];
    // // Generate outputMicroVolts
    // for (int i = 0; i < sizeof(majorScale)/sizeof(byte); i++) {
    //     outputMicroVolts[i] = 83333*i;
    //     outputValues[i] = round(outputMicroVolts[i] * 819 / 1e6);
    // }

    // Calculate output DAC values
    // Calculate microvolt midpoints
    // Calculate input ADC threshold values


}

// transform 10-bit ADC output to quantized 12-bit DAC input
// input: 0-5V, 0-1023
// intermediate: 0-5V, 0-59 semitones
// output: 0-5V, 0-4095

// binary search inputThresholds for ADC value (choose value <= ADC)
// use inputThresholds index to lookup appropriate output value

uint16_t quantizeAdcToDac(int input)
{
    // Lazy just to find something that works. Division inside map is bad.
    int semitones = map(input, 0, 1023, 0, 60);
    int semitonesInOctave = semitones;
    int octave = 0;
    while (semitonesInOctave > 11) { octave++; semitonesInOctave -= 12; }
    int semitonesInOctave2;
    // Restrict to selected notes in scale (round down to nearest note)
    for (unsigned int i = sizeof(majorScale) / sizeof(byte) - 1; i >= 0; i--) {
        // Serial.print(i);
        if (semitonesInOctave >= majorScale[i]) {
            semitonesInOctave2 = majorScale[i];
            // Serial.print(i);
            // Serial.print(semitonesInOctave2);
            break;
        }
    }
    int semitonesOut = octave * 12 + semitonesInOctave2;
    uint16_t dacOut = map(semitonesOut, 0, 60, 0, 4095);

    // Serial.print("quant in=");
    // Serial.print(input);
    // Serial.print(" semis=");
    // Serial.print(semitones);
    // Serial.print(" octave=");
    // Serial.print(octave);
    // Serial.print(" +semis=");
    // Serial.print(semitonesInOctave);
    // Serial.print(" octave=");
    // Serial.print(octave);
    // Serial.print(" +semis=");
    // Serial.print(semitonesInOctave2);
    // Serial.print(" quant=");
    // Serial.print(semitonesOut);
    // Serial.print(" dac=");
    // Serial.print(dacOut);
    // Serial.println();

    return dacOut;
}