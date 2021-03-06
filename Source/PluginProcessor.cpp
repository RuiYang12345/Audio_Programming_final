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
        
    std::make_unique<juce::AudioParameterChoice>  ("direction", "Direction", StringArray({"high pass", "low pass"}),0),
        
        
    std::make_unique<juce::AudioParameterFloat>  ("detune", "Detune", 0.0, 20.0, 2.0),
            
    std::make_unique<juce::AudioParameterFloat> ("ATTACK", "Attack", 0.0, 5.0, 0.0),
    std::make_unique<juce::AudioParameterFloat> ("DECAY", "Decay", 0.0, 3.0, 2.0),
    std::make_unique<juce::AudioParameterFloat> ("SUSTAIN", "Sustain", 0.0, 1.0, 1.0),
    std::make_unique<juce::AudioParameterFloat> ("RELEASE", "Release", 0.0, 5.0, 2.0),

    std::make_unique<juce::AudioParameterFloat>("lfoRate01", "SinLFO Rate", 0.0, 1.0, 0.0025),
    std::make_unique<juce::AudioParameterFloat>("lfoRate02", "TriLFO Rate", 0.0, 2.0, 0.0025),
        

} )

{
    //========================== constructor =================================
    
    //for whole
    gainParam = parameters.getRawParameterValue("gain");
    
    
    //for synth
    LowpassOrHighpassParam = parameters.getRawParameterValue ("direction");
  
    
    //for synth
    detuneParam = parameters.getRawParameterValue("detune");
   
    //for synth
    attackParam = parameters.getRawParameterValue("attack");
    decayParam = parameters.getRawParameterValue("decay");
    sustainParam = parameters.getRawParameterValue("sustain");
    releaseParam = parameters.getRawParameterValue("release");
    
    //for synth
    lfoRate01Param = parameters.getRawParameterValue("lfoRate01");
    lfoRate02Param = parameters.getRawParameterValue("lfoRate02");
    
    
    // initialize sampler
    for (int i = 0; i < voiceCount; i++) //voice count
    {
       sampler.addVoice(new juce::SamplerVoice());
    }

    sampler.setSample(BinaryData::RY_Metal_Knock_C4_wav, BinaryData::RY_Metal_Knock_C4_wavSize);
    

    // initalize synth
    //voice count
        for (int i=0; i<voiceCount; i++)
        {
            synth.addVoice( new DreamyVoiceSynthVoice() );
        }

        synth.addSound( new DreamyVoiceSynthSound);
    
  
    // set parameters for sampler
    for (int i = 0; i < voiceCount; i++)
    {
        DreamyVoiceSynthVoice* v = dynamic_cast< DreamyVoiceSynthVoice*>(synth.getVoice(i));
        v->setParameterPointers(detuneParam, attackParam, decayParam, sustainParam, releaseParam);
    }
    
}

//==============================================================================

void SamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampler.setCurrentPlaybackSampleRate(sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate) ;

    for (int i=0; i<voiceCount; i++)
    {
        DreamyVoiceSynthVoice* v = dynamic_cast <DreamyVoiceSynthVoice*> (synth.getVoice(i));
        v-> init (sampleRate);
    }

    /// delay
    delay.setSize (2.0 * sampleRate); // delay time relative to the sampleRate
    delay.setFeedback(0.5);
    delay.setDelayTime (sampleRate * 0.25); // changeable delay time
    
    sr = sampleRate;
    
    /// delay LFO. modulation of delay time
    //Sin mod
    delayTimeLFO01.setSampleRate(sampleRate);
    delayTimeLFO01.setFrequency(0.15);
    
    //Tri mod
    delayTimeLFO02.setSampleRate(sampleRate);
    delayTimeLFO02.setFrequency(0.1);
    
    
    //freq modulation of filter
    filterSynth.setSampleRate(sampleRate);
    filterSynth.setFrequency(0.01); // a slow filter sweep for smooth filter sounds

}
//===============================================================================
void SamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
 
     // ========================== sampler render ================================
    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    int numSamples = buffer.getNumSamples();
    
    //dereferencing the gain parameter
    float bufferGain = *gainParam;
    
    //pointers to audio arrays
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);
    

    // sampler DSP LOOP
    for (int i=0; i<numSamples; i++)
    {
            left[i] = left[i] * (bufferGain);
            right[i] = right[i] *  (bufferGain);
    }
     

 
    // ========================== synth render ===============================
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    int numSamples02 = buffer.getNumSamples();
    
    //pointers to audio arrays
    auto* leftchannel  = buffer.getWritePointer(0);
    auto* rightchannel = buffer.getWritePointer(1);
    
    
    // dereferencing the delay parameter
    float lfo1Rate = *lfoRate01Param;
    float lfo2Rate = *lfoRate02Param;
    
    
    //filter param
    float filter = *LowpassOrHighpassParam;
    
    // ==== frequency modulation of filter ======
    //0 = highpass,1 = powpass
    if (filter > 0)
    {
        float filter = filterSynth.lowpassprocess(numSamples02);
    }
    else
    {
        float filter = filterSynth.highpassprocess(numSamples02);
    
    }
    
    // synth DSP LOOP
    for (int y=0; y<numSamples02; y++)
    {
    
        //============== delay ===================
        //set mod rate with plug in parameter
        delayTimeLFO01.setFrequency(lfo1Rate);
        delayTimeLFO02.setFrequency(lfo2Rate);
        
        // modulate delay time
        float delayMod = delayTimeLFO01.process() + delayTimeLFO02.process();
        int delayTime = delayMod * 4000 + 5000;
        delay.setDelayTime(delayTime);
        float delayedSample_l = delay.process (leftchannel[y]);
        float delayedSample_r = delay.process (leftchannel[y]);
        
    
        //=============== mix =====================
        leftchannel [y] = leftchannel[y] + delayedSample_l + filter;
        rightchannel[y] = rightchannel[y] + delayedSample_r + filter;
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
