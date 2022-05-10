#pragma once

#include <cmath>  // for sin function
#define TWOPI 6.283185307179586476925286766559

/**
 Base class for oscillators (0-1 phasor). Call setSampleRate() before use
 */
class Phasor02
{
public:
    Phasor02()  {};
    
    virtual ~Phasor02() {};
    
    /// Note: 0-1
    double process02()
    {
        phase += phaseDelta;
        
        if (phase > 1.0)
            phase -= 1.0;
        
        return output(phase);
    }
    
    /// Override for the actual output based on the 0-1 phase
    virtual float output(float p)
    {
        return p;
    }
    
    /// Set the sample rate before calling setFrequency()
    void setSampleRate(float SR)
    {
        sampleRate = SR;
    }
    
  
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }
    
    /// set phase (0-1)
    void setPhase(float p)
    {
        phase = p;
    }
    
    /// offset phase (e.g. for phase modulation)
    void offsetPhase(float o)
    {
        phase += o;
    }
    
protected:
    double phaseDelta;
    
private:
    float frequency;
    float sampleRate;
    double phase = 0.0;
};




// Child classes

/// Sinusoidal oscillator based on Phasor02 parent
class SinOsc02 : public Phasor
{
    float output(float p) override
    {
        return std::sin(p * TWOPI);
    }
};

