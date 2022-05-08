/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Reverb.h"

//==============================================================================
/**
*/
class MalgukiAudioProcessor  : public juce::AudioProcessor
{
public:
    float mix;
    float preGain;
    float postGain;
    float delayTime;
    float feedback;
    std::vector<SpringArray> springArrays;
    //==============================================================================
    MalgukiAudioProcessor();
    ~MalgukiAudioProcessor() override;

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
    //==============================================================================
    juce::AudioBuffer<float> delayBuffer;
    juce::AudioBuffer<float> auxBuffer;
    void fillBuffer(int channel, int delayBufferSize, const float* channelData, int bufferSize);
    void readFromBuffer(int channel, int delayBufferSize, juce::AudioBuffer<float>& buffer, int bufferSize, float delayTime, float feedback);
    int writePosition {0};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MalgukiAudioProcessor)
};
