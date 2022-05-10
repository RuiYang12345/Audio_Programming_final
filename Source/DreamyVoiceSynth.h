/**
 Synth with filter, adsr and detune param.
 initalize with addVoice() and addSound(), set up the ADSR pointers
 setCurrentPlaybackSampleRate before use
 */

#pragma once
#include "Oscillator.h"

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
        osc.setSampleRate(sampleRate);
        detuneOsc.setSampleRate(sampleRate);
        env.setSampleRate(sampleRate);
        
        juce::ADSR::Parameters envParams;
        envParams.attack = 0.5;
        envParams.decay = 0.25;
        envParams.sustain = 0.5;
        envParams.release = 1.0;
    
        env.setParameters (envParams);
    }
    
 
    void setParameterPointers(std::atomic<float>* detuneIn, std::atomic<float>* attackTimeIn, std::atomic<float>* decayTimeIn, std::atomic<float>* sustainTimeIn, std::atomic<float>* releaseTimeIn)
    {
        detuneAmount =detuneIn;
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
        osc.setFrequency (freq);
    
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
             
            detuneOsc.setFrequency (freq - *detuneAmount);
            
            //   ADSR parameter
            envParams.attack = *attackTime;
            envParams.decay = *decayTime;
            envParams.sustain = *sustainTime;
            envParams.release = *releaseTime;
            env.setParameters(envParams);
            
            // DSP iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                
                float envVal = env.getNextSample();
                
                
                //======= mix =========
                float currentSample = (osc.process() + detuneOsc.process()) * 0.5 * envVal;//scales sample volume
                
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
    ///pitch bend
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
    /// set up voice play
    bool playing = false;
    bool ending = false;
    
    /// a random object for use in our test noise function
    juce::Random random;
    
    float freq;
    
    TriOsc osc, detuneOsc;
    
    juce::ADSR env;
    
    
    // memeber variable to a pointer for params.
    
    std::atomic<float>* detuneAmount;
    // ADSR envelope: attack, decay, sustain, release
    juce::ADSR :: Parameters envParams;
    std::atomic<float>* attackTime;
    std::atomic<float>* decayTime;
    std::atomic<float>* sustainTime;
    std::atomic<float>* releaseTime;
    
    // Filter
    std::atomic<float>* lowpassParam; //plugin parameter for lowest frequency
    std::atomic<float>* highpassParam; //plugin parameter for highest frequency
    /// frequency modulation of filter
    Filter filterSynth;
    
};

//=====================================================================================================

//


