/**
 * sampler class with binary data
 */

#pragma once
#include <JuceHeader.h>
using namespace juce;

class RYSampler : public juce::Synthesiser
{
    
public:

    /**
    * setSample function
    */
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
    
private:
    juce::AudioFormatManager formatManager;
    float frequency;    
};

