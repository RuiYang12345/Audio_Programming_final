/*
  ==============================================================================

    RYSampler.h
    Created: 14 Apr 2022 5:25:00pm
    Author:  Rui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class RYSampler : public juce::Synthesiser
{
    
public:
    /**
    * Initialise
    */
    void init()
    {
        // allow us to use WAV files.
        formatManager.registerBasicFormats();
    }
    
    void setSample(const void* sourceData, size_t sourceDataSize)
    {
        formatManager.registerBasicFormats();
        
        auto* reader = formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream> (sourceData,sourceDataSize, false));
        
        // Scaling the range from C2-C7 (36 - 96)
        juce::BigInteger midiNotes;
        midiNotes.setRange(36,60,true);
        
        // Add sound
        addSound( new juce::SamplerSound("Ding", *reader, midiNotes, 60, 0, 0.1, 10.0 ) );
    }
    
 //   void setSampleRate(float sr)
 //  {
 //       sampleRate = sr;
 //  }
    
    void setFrequency(float freq)
   {
        frequency = freq;
    }

    
private:
    juce::AudioFormatManager formatManager;
   float sampleRate;
   float frequency;
    
};

