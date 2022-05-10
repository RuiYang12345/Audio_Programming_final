#pragma once

#include <vector> // for setting vactor of Triangular wave oscillator
#include "Oscillator.h"

/**
 The vector of Triangular wave oscillator generates pitches in circle of fourth. Call setSampleRate() before use.
 */

/**
 * 1. Circle of fouth MIDI pitches of piano:
 * D (50) - G (55) - C(60) -F (65)- Bb(70)
 * 2. Convert MIDI pitches to frequencies in Hz in std::vector
 * 50 = 146.832 Hz
 * 55 = 195.998 Hz
 * 60 = 261.626 Hz
 * 65 = 349.228 Hz
 * 70 = 466.164 Hz
 */


class Vector
{
    
public:
    
    void setSampleRate(float sr)
    {
        /// add Triangular wave oscillators by calling push_back
        
        /// 50 = 146.832 Hz
        triOscs.push_back(TriOsc());
        triOscs[0].setSampleRate(sr);
        triOscs[0].setFrequency(146.832);
        
        /// 55 = 195.998 Hz
        triOscs.push_back(TriOsc());
        triOscs[1].setSampleRate(sr);
        triOscs[1].setFrequency(195.998);
        
        /// 60 = 261.626 Hz
        triOscs.push_back(TriOsc());
        triOscs[2].setSampleRate(sr);
        triOscs[2].setFrequency(261.626);
        
        /// 65 = 349.228 Hz
        triOscs.push_back(TriOsc());
        triOscs[3].setSampleRate(sr);
        triOscs[3].setFrequency(349.228);
        
        /// 70 = 466.164 Hz
        triOscs.push_back(TriOsc());
        triOscs[4].setSampleRate(sr);
        triOscs[4].setFrequency(466.164);
        
    }
        
    
    float process ()
    {
        /// count the five pitches
        int oscCount = 5;
        
        /// set the value and make sure it exists before the loop
        float sampleVal = 0;
        
        /// loop within loop: looping five pitches in the process of dsp loop
        for (int i = 0; i <oscCount; i++)
        {
            sampleVal += triOscs[i].process()* 0.8; ///0.8 is volume
        }
        /// divide the sample value by the number of oscillators
        /// To guarantee the value will be less than 1. The volume would not accumulate by the number of oscillators
        return sampleVal/oscCount;
    }
        
    
private:
    std::vector <TriOsc> triOscs; /// store TriOsc objects
};

