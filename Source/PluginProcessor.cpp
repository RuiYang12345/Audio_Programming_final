/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

//==============================================================================
const juce::String PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
   
    // === vector of triangle wave ===
    vector.setSampleRate(sampleRate);
    
    // === freq modulation of sine and square wave ===
    modulator01.setSampleRate(sampleRate);
    modulator01.setBaseFrequency(440.0);
    modulator01.setModFrequency(8.0); // growl effect of the upper limit of subsonic oscillations
    
    modulator02.setSampleRate(sampleRate);
    modulator02.setBaseFrequency(880.0);
    modulator02.setModFrequency(12.0); // double mod1 frequency and move faster than mod1 with higher Hz
    
    
    // ====== freq modulation of filter ==
    filterSynth.setSampleRate(sampleRate);
    filterSynth.setFrequency(0.01); // a slow filter sweep for smooth filter sounds
    
    //============= panning =========
    panOsc.setSampleRate(sampleRate);
    panOsc.setFrequency(0.025); // very slowly changing
    
    //============== DELAY ==============
    
    delay.setSize (2.0 * sampleRate); // delay time relative to the sampleRate
    
    delay.setDelayTime (sampleRate * 0.5); // changeable delay time
    
    

    
    
}

void PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int numSamples = buffer.getNumSamples();
    
    //pointers to audio arrays
    float* leftchannel = buffer.getWritePointer(0);
    float* rightchannel = buffer.getWritePointer(1);
    
    //DSP LOOP!
    for (int i=0; i<numSamples; i++)
    {
        // ==== mix triangle wave, frequency modulation of sine tone and square wave ====
        float sample = (vector.process() + modulator01.process() + modulator02.process());
        
        // ==== frequency modulation of filter ======
        float sampleL = filterSynth.processL(sample);
        float sampleR = filterSynth.processR(sample);
    
        // ============= panning =================
        // get panning oscillator val ±1
        float panVal = panOsc.process();    // ±1
            
        // derive raw left and right pan values
        float panL = panVal*0.5 + 0.5;      // 0-1
        float panR = 1.0 - panL;            // 1-0
                
        //============== delay ===================
        float delayedSample =delay.process(sample);
    
        // ================ final mix =====================
        leftchannel [i] = (delayedSample * panL) + sampleL;
        rightchannel[i] = (delayedSample * panR) + sampleR;
    }
    
       
}



void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
//actual audio processing




//==============================================================================
bool PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new PluginAudioProcessorEditor (*this);
}

//==============================================================================
void PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}
