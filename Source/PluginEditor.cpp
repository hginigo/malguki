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

    audioVolume.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    audioVolume.setRange(0.0, 1.0, 0.01);
    audioVolume.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    audioVolume.setTextValueSuffix(" Volume");
    audioVolume.setMouseDragSensitivity(50);
    audioVolume.setValue(0.75);
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

    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Audio Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void MalgukiAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int size = 100;
    int x = (getWidth() - size) / 2;
    int y = (getHeight() - size) / 2;
    audioVolume.setBounds(x, y, size, size);
}

void MalgukiAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.noteOnVel = audioVolume.getValue();
}
