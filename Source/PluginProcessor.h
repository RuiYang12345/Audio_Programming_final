/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RYSampler.h"
#include "Filter.h"
#include "DreamyVoiceSynth.h"
#include "Delay.h"
#include "Oscillator.h"


//==============================================================================
/**
*/
class SamplerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SamplerAudioProcessor();
    ~SamplerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //instances of classes

    RYSampler sampler;
    
    /// Delay
    Delay delay;
    
    /// Filter
    Filter filter;
    
    /// ADSR
    juce::Synthesiser synth;
        
    /// Delay LFO
    SinOsc delayTimeLFO01;
    TriOsc delayTimeLFO02;
    
    int voiceCount = 8;
    
    //AudioParameters
    
    juce::AudioProcessorValueTreeState parameters;
    
    /// Gain params
    std::atomic<float>* gainParam;  //plugin parameter for gain
    
    /// Filter params
    std::atomic<float>* lowpassParam; //plugin parameter for lowest frequency
    std::atomic<float>* highpassParam; //plugin parameter for highest frequency
    
    /// ADSR params
    std::atomic<float>* attackParam; //plugin parameter for envolope
    std::atomic<float>* decayParam;
    std::atomic<float>* sustainParam;
    std::atomic<float>* releaseParam;
    
    /// Delay lfo params
    std::atomic<float>* lfoRate01Param; //plugin parameter for rate
    std::atomic<float>* lfoRate02Param;
  
    ///bypassParam
    std::atomic<float>* bypassParam;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerAudioProcessor)
};