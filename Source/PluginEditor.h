/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Reverb.h"

//==============================================================================
/**
*/
class MalgukiAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    MalgukiAudioProcessorEditor (MalgukiAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~MalgukiAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    int frameCounter;
    void timerCallback() final;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MalgukiAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider mix;
    juce::Slider preGain;
    juce::Slider postGain;
    juce::Slider delayTime;
    juce::Slider feedback;

    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> preAttachment;
    std::unique_ptr<SliderAttachment> postAttachment;
    std::unique_ptr<SliderAttachment> delayAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MalgukiAudioProcessorEditor)
};
