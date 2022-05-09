#pragma once

#include "RYSampler.h"
#include "DreamyVoiceSynth.h"


/**
/Call setSampleRate() before use.
 */

class Filter
{
public:
    
    void setSampleRate(float sr)
    {
        sr = 0.0;
    }
          
    void setFrequency (float freq)
    {
        dingSampler.setFrequency(freq);
    }
        
            
    float process(float inputSample)
        {
              float sr = 0.0;
              float freq = 0.0;
              filter.setCoefficients( juce::IIRCoefficients::makeLowPass(sr, freq,5.0));
              filter.setCoefficients( juce::IIRCoefficients::makeHighPass(sr, freq,5.0));
              float Output = filter.processSingleSampleRaw(inputSample);
              return Output;
        }
    
private:
  juce::IIRFilter filter;
  juce::AudioFormatManager formatManager;
  RYSampler dingSampler;
  
  float sampleRate;
  float freq;
    
};
