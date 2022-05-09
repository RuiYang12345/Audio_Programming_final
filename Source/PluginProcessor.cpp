/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SamplerAudioProcessor::SamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif

// param
    parameters (*this, nullptr , "Parameter Tree", {
    
    std::make_unique<juce::AudioParameterFloat>  ("gain", "Gain", 0.0, 10.0, 5.0),
    
    std::make_unique<juce::AudioParameterFloat> ("lowpass", "Lowpass", 50.0, 350.0, 55.0),
    std::make_unique<juce::AudioParameterFloat>  ("highpass", "Highpass", 350.0, 5500.0, 2480.0),
    
    std::make_unique<juce::AudioParameterFloat> ("ATTACK", "Attack", 0.0, 5.0, 0.0),
    std::make_unique<juce::AudioParameterFloat> ("DECAY", "Decay", 0.0, 3.0, 2.0),
    std::make_unique<juce::AudioParameterFloat> ("SUSTAIN", "Sustain", 0.0, 1.0, 1.0),
    std::make_unique<juce::AudioParameterFloat> ("RELEASE", "Release", 0.0, 5.0, 2.0),

    std::make_unique<juce::AudioParameterFloat>("lfoRate01", "SinLFO Rate", 0.0, 1000.0, 0.25),
    std::make_unique<juce::AudioParameterFloat>("lfoRate02", "TriLFO Rate", 0.0, 20.0, 0.25),
        
    std::make_unique<juce::AudioParameterBool>("bypass", "Bypass",false),
    
    
} )

{
    gainParam = parameters.getRawParameterValue("gain");
    
    lowpassParam = parameters.getRawParameterValue("lowpass");
    highpassParam = parameters.getRawParameterValue("highpass");
    
    attackParam = parameters.getRawParameterValue("attack");
    decayParam = parameters.getRawParameterValue("decay");
    sustainParam = parameters.getRawParameterValue("sustain");
    releaseParam = parameters.getRawParameterValue("release");
    
    lfoRate01Param = parameters.getRawParameterValue("lfoRate01");
    lfoRate02Param = parameters.getRawParameterValue("lfoRate02");
    
    bypassParam = parameters.getRawParameterValue("bypass");
       
    
    //===========================================================

    //voice count
    for (int i=0; i<voiceCount; i++)
    {
        sampler.addVoice( new juce::SamplerVoice() );
    }
    
    // audio setting
    sampler.setSample(BinaryData::RY_Metal_Knock_C4_wav, BinaryData::RY_Metal_Knock_C4_wavSize);
    
}
//==============================================================================

void SamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampler.setCurrentPlaybackSampleRate(sampleRate);
    
   // for (int i=0; i<voiceCount; i++)
   // {
   //     Filter* v = dynamic_cast<Filter*>(sampler.getVoice(i));
   //     v->setSampleRate(sampleRate);
   // }

    for (int i=0; i<voiceCount; i++)
    {
        DreamyVoiceSynthVoice* v = dynamic_cast<DreamyVoiceSynthVoice*>(synth.getVoice(i));
        
        v->setParameterPointers (attackParam,decayParam,sustainParam,releaseParam);
    }
    
    /// delay LFO. modulation of delay time
    
    //Sin mod
    delayTimeLFO01.setSampleRate(sampleRate);
    delayTimeLFO01.setFrequency(0.15);
    
    //Tri mod
    delayTimeLFO02.setSampleRate(sampleRate);
    delayTimeLFO02.setFrequency(0.1);
    
    /// delay
    delay.setSize (2.0 * sampleRate); // delay time relative to the sampleRate
    delay.setDelayTime (sampleRate * 0.5); // changeable delay time
    
    
    /// filter
    filter.setSampleRate (sampleRate);
    
}
//===============================================================================
void SamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    int numSamples = buffer.getNumSamples();
    
    //pointers to audio arrays
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);
    
    //check bypass is off
   if (! *bypassParam)
     {
    
    //DSP LOOP
    for (int i=0; i<numSamples; i++)

     {
        //float sample = 0.0f;
        
        //============== filter ===================
        float freq = *lowpassParam + (*highpassParam - *lowpassParam); //scaling
        filter.setFrequency(freq);
        float sample02 = filter.process(freq);
        
        //============== delay ===================
        
        ///set mod rate with plug in  parameter
        delayTimeLFO01.setFrequency(*lfoRate01Param);
        delayTimeLFO02.setFrequency(*lfoRate02Param);
        
        // modulate delay time
        float delayMod = delayTimeLFO01.process() + delayTimeLFO02.process();
        int delayTime = delayMod * 4000 + 5000;
        delay.setDelayTime(delayTime);
        
        float delayedSample =delay.process(sample02);
        //=============== final mix=====================
        left[i] = left[i] * (*gainParam) +  delayedSample;
        right[i] = right[i] *  (*gainParam) +  delayedSample;
        
        
        //left[i] = left[i] * (*gainParam);
        //right[i] = right[i] * (*gainParam) ;
        
     }
     }
}
//================================================================================================








SamplerAudioProcessor::~SamplerAudioProcessor()
{
}

//==============================================================================
const juce::String SamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SamplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SamplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SamplerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SamplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void SamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
} 

//==============================================================================

void SamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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



//==============================================================================
bool SamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SamplerAudioProcessor::createEditor()
{
    //return new SamplerAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SamplerAudioProcessor();
}
