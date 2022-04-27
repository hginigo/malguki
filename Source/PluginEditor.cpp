/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MalgukiAudioProcessorEditor::MalgukiAudioProcessorEditor (MalgukiAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 200);

    audioVolume.setSliderStyle(juce::Slider::LinearBarVertical);
    audioVolume.setRange(0.0, 1.0, 0.05);
    audioVolume.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    audioVolume.setTextValueSuffix(" Volume");
    audioVolume.setValue(0.5);
    audioVolume.addListener(this);

    addAndMakeVisible(&audioVolume);
}

MalgukiAudioProcessorEditor::~MalgukiAudioProcessorEditor()
{
}

//==============================================================================
void MalgukiAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.fillAll (juce::Colours::white);
    g.setColour (juce::Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("Audio Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void MalgukiAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    audioVolume.setBounds(40, 30, 20, getHeight() - 60);
}

void MalgukiAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.noteOnVel = audioVolume.getValue();
}
