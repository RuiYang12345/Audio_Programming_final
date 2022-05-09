/*
  ==============================================================================

    DreamyVoiceSynthesiser.h
    Created: 13 April 2022
    Author:  Rui

  ==============================================================================
*/

#pragma once
#include "RYSampler.h"
// ===========================
// ===========================
// SOUND
class DreamyVoiceSynthSound : public juce::SynthesiserSound
{
public:
    ///use IF here!
    bool appliesToNote      (int) override      { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override      { return true; }
};

// =================================
// =================================
//VOICE

/*!
 @class DreamyVoiceSynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple DreamyVoiceSynthVoice objects will be created by the Synthesiser so that it can be played polyphicially
 
 @namespace none
 @updated 2019-06-18
 */
class DreamyVoiceSynthVoice : public juce::SynthesiserVoice
{
public:
    DreamyVoiceSynthVoice() {}
    //--------------------------------------------------------------------------
    
    void init (float sampleRate)
    {
       // rySampler.setSampleRate(sampleRate);
      
        env.setSampleRate(sampleRate);
        
        
        juce::ADSR::Parameters envParams;
        envParams.attack = 0.5;
        envParams.decay = 0.25;
        envParams.sustain = 0.5;
        envParams.release = 1.0;
    
        env.setParameters (envParams);
    }
    
    
    
    void setParameterPointers(std::atomic<float>* attackTimeIn, std::atomic<float>* decayTimeIn, std::atomic<float>* sustainTimeIn, std::atomic<float>* releaseTimeIn)
    {
        attackTime = attackTimeIn;
        decayTime = decayTimeIn;
        sustainTime = sustainTimeIn;
        releaseTime = releaseTimeIn;
    }
    
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        ending = false;
        freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        rySampler.setFrequency (freq);

        	
        env.reset();
        env.noteOn();
        
    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        
        if (allowTailOff)
        {
            env.noteOff();
            ending = true;
        }
        else
        {
            clearCurrentNote();
            playing = false;
        }
    }
            
        
    
    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here
     
     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            
            
            envParams.attack = *attackTime;
            envParams.decay = *decayTime;
            envParams.sustain = *sustainTime;
            envParams.release = *releaseTime;
            env.setParameters(envParams);
            
            
            
            // DSP iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                
                float envVal = env.getNextSample();
                
                //scales sample volume
                float currentSample = 1.0 * envVal;
                
                // your sample-by-sample DSP code here!
                // An example white noise generater as a placeholder - replace with your own code
                
                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan<outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, currentSample * 0.2);
                }
            }
            
            if (ending)
            {
                float envVal = env.getNextSample();
                if (envVal < 0.0001)
                {
                    clearCurrentNote();
                    playing = false;
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    ///pitch bend!
    void pitchWheelMoved(int) override {}

    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of YourSynthSound
     */
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<DreamyVoiceSynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false;
    
    /// a random object for use in our test noise function
    juce::Random random;
    
    float freq;

    RYSampler rySampler;
    
    juce::ADSR env;
    //ADSR envelope: attack, decay, sustain, release
    
    
    juce::ADSR :: Parameters envParams;
    std::atomic<float>* attackTime;
    std::atomic<float>* decayTime;
    std::atomic<float>* sustainTime;
    std::atomic<float>* releaseTime;
};

//=====================================================================================================

//


