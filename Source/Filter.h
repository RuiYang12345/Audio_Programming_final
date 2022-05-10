#pragma once

#include "Oscillator.h" //for Sinusoidal oscillator

/**
 Stereo filter synth. Call setSampleRate() and setFrequency() before use.
 */

class Filter
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
    
    // ================ frequency modulation process of lowpass filter ===================
    float lowpassprocess(float inputSample)
    {
       
          float modulation2 = lfo02.process();
          float cutoff1 = 440.0 + modulation2 * 200.0;
            // use LFO to set filter cutoff
          filter.setCoefficients( juce::IIRCoefficients::makeLowPass(sampleRate, cutoff1,5.0));
          float lowpassOutput = filter.processSingleSampleRaw(inputSample);
          return lowpassOutput;

    }
    
    // ================ frequency modulation process of highpass filter ===================
    float highpassprocess (float inputSample)
    {
        float modulation2 = lfo02.process();
        float cutoff2 = 440.0 + modulation2 * 200.0;
        filter2.setCoefficients( juce::IIRCoefficients::makeHighPass(sampleRate, cutoff2,5.0));
        float highpassOutput= filter2.processSingleSampleRaw(inputSample);
        return highpassOutput;
    }
    
     
private:
  juce::IIRFilter filter; //left channel
  juce::IIRFilter filter2; //right channel
  SinOsc lfo02; //for cutoff function
  float sampleRate;
};

