#pragma once

#include "Oscillator.h" //for Sinusoidal oscillator

/**
 Stereo filter synth. Call setSampleRate() and setFrequency() before use.
 */

class FilterSynth
{
public:
    
    /// Set the sample rate before calling setFrequency()
    void setSampleRate(float sr)
    {
    sampleRate = sr;
    lfo02.setSampleRate(sr);
    }
    
    void setFrequency (float freq)
    {
    lfo02.setFrequency(freq);
    }
    
    
    float processL(float inputSample)
    {
       
          float modulation2 = lfo02.process();
          float cutoff1 = 440.0 + modulation2 * 200.0; 
            // use LFO to set filter cutoff
          filter.setCoefficients( juce::IIRCoefficients::makeLowPass(sampleRate, cutoff1,5.0));
          float leftOutput = filter.processSingleSampleRaw(inputSample);
          return leftOutput;
        
      

    }
    

    float processR (float inputSample)
    {
        float modulation2 = lfo02.process();
        float cutoff2 = 440.0 + modulation2 * 200.0;
        filter2.setCoefficients( juce::IIRCoefficients::makeHighPass(sampleRate, cutoff2,5.0));
        float rightOutput= filter2.processSingleSampleRaw(inputSample);
        return rightOutput;
    }
    
     
private:
  juce::IIRFilter filter; //left channel
  juce::IIRFilter filter2; //right channel
  SinOsc lfo02; //for cutoff function
  float sampleRate;
};


