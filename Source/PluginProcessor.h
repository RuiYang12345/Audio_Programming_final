/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"
#include <JuceHeader.h>
#include "FilterSynth.h"
#include "Modulator.h"
#include "Vector.h"
#include "Panning.h"
#include "Delay.h"

//==============================================================================
/**
*/
class PluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

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
    
    ///instances of classes
    
    // === vector of triangle wave ===
    Vector vector;

    // === frequency modulation of sine tone and square wave ===
    Modulator modulator01;
    Modulator modulator02; //second oscillator frequency modulation
    
    // === frequency modulation of filter ===
    FilterSynth filterSynth;
    
    // === panning oscillator ===
    SinOsc02 panOsc;
    
    //=== delay ===
    Delay delay;
    


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)

};
