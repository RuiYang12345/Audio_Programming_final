#pragma once

#include "RYSampler.h"
#include "DreamyVoiceSynth.h"


/**
 ideally it is a filter for sampler
 */

class Filter
{
public:
    

    float process(float inputSample)
        {
              float setSampleRate = sr;
              float setFrequency = freq;
        
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
  RYSampler sampler;
  
  float sr;
  float freq;
    
};
