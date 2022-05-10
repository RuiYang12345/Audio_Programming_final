#pragma once

#ifndef Oscillators_h
#define Oscillators_h
#include <cmath> // for sin and fabs function 

/**
Oscillator class with three wave shapes (processSine, processSquare, processTriangle). Call setSampleRate() before use.
 */

//Parent phasor class
class Phasor
{
public:
    /**
    * Our parent oscillator class does the key things required for most oscillators:
    * -- handles phase
    *-- handles setters and getters for frequency and samplerate
     */
     
    /// Note: 0-1
    float process()
    {
        phase += phaseDelta;
        
        if (phase > 1.0)
            phase -= 1.0;
        return output(phase);
    }
    
    /// Override for the actual output based on the 0-1 phase
    virtual float output (float p)
    {
        return p;
    }
    
    /// Set the sample rate before calling setFrequency()
    void setSampleRate(float sr)
    {
        sampleRate = sr;
    }
    
   
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }
    
private:
    float frequency;
    float sampleRate = 44100;
    float phase = 0.0;
    float phaseDelta;
};
//=============================================

//Child class

/// Sinusoidal oscillator based on Phasor parent
class SinOsc : public Phasor
{
    
    float output (float p) override
    {
        return sin(p * 2 * 3.141592653589793);
    }
};
    

/// Square wave oscillator based on Phasor parent
class SquOsc : public Phasor
{
public:
   float output (float p) override
    {
        float outVal = 0.5;
        if (p > 0.5)
            outVal = -0.5;
        return outVal;
    }
    
    /// Width of each square wave
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }
private:
    float pulseWidth = 0.5;
};
    
/// Triangular wave oscillator based on Phasor parent
class TriOsc : public Phasor
{

   float output (float p) override
    {
        return fabs(p - 0.5) - 0.25;
    }
  
};
   

//==========================================



#endif /* Oscillators_h */
