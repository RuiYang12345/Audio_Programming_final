#pragma once

#include "Oscillator.h" //for Sinusoidal oscillator and Square wave oscillator

/**
 Frequency modulator: three Sinusoidal oscillators and one Square wave oscillator.  The extra two Sinusoidal oscillators is for setting beat frequencies and lfo.
 Call setSampleRate(), setBaseFrequency() and setModFrequency() before use.
 */


class Modulator
{
public:
    
    /// Set the sample rate before calling setFrequency()
    void setSampleRate (float sr)
    {
        squOsc.setSampleRate(sr);
        sinOsc1.setSampleRate(sr);
        sinOsc2.setSampleRate(sr);
        lfo01.setSampleRate(sr);
    }
    
    /// call setFrequency function
    void setFrequency (float freq)
    {
        squOsc.setFrequency (freq);
        sinOsc1.setFrequency(freq);
    }
    
    /// call base frequency function within the process of setFrequency
    void setBaseFrequency(float freq)
    {
        baseFrequency = freq;
    }

    /// set freq02 for rhythmic pulsing of beat frequencies
    void setBeatFrequency (float freq02)
    {
        sinOsc2.setFrequency (freq02);
    }
    
    /// for frequency modulation
    void setModFrequency (float freq)
    {
        lfo01.setFrequency(freq);
    }
      
    // ========================= Frequency modulation process of sine tone and square wave ===================
    float process()
    {
        /// set frequency modulation function
        float modulation = lfo01.process();
        
        /// add frequency modulation weight in base frequency
        sinOsc1.setFrequency(baseFrequency + modulation * 500);
        
        /// beat frequencies modulation
        sinOsc2.setFrequency(baseFrequency + 1.0 + modulation * 500);
        
        /// Square wave oscillator with smaller frequency modulation weight
        squOsc.setFrequency(baseFrequency + modulation * 100);
    
        /// call sample function to mix oscillators above and return
        float sample = (sinOsc1.process() * 0.032 + sinOsc2.process() * 0.024 + squOsc.process() *0.016);
        return sample;
    }
    
private:
    SquOsc squOsc;  // in base frequency
    SinOsc sinOsc1; // in base frequency
    
    SinOsc sinOsc2; // for beat frequencies with sinOsc1
    SinOsc lfo01;   // for frequency modulation
    float baseFrequency;
    float sampleRate;
};


