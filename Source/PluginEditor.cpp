/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MalgukiAudioProcessorEditor::MalgukiAudioProcessorEditor (MalgukiAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), frameCounter(0)
{
    startTimerHz(60);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 400);

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
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
 
    g.setColour (getLookAndFeel().findColour (juce::Slider::thumbColourId));
 
    float radius = 5.f;
    int offset = 50;

 
    juce::Point<float> p ((float) getWidth()  / 2.0f + 1.0f * (float) radius * std::sin ((float) frameCounter * 0.04f),
                          (float) getHeight() / 2.0f + 1.0f * (float) radius * std::cos ((float) frameCounter * 0.04f));

    auto particles = audioProcessor.r1.getParticles();

    for (const Particle& particle : particles) {
        Vector pos = particle.pos();
        juce::Point<float> p (offset + (float) particle.pos().x * 30,
                              offset - (float) particle.pos().y * 30);
        
        g.fillEllipse(p.x, p.y, radius, radius);
    }
 
    // g.fillAll (juce::Colours::black);
    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Audio Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
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

void MalgukiAudioProcessorEditor::timerCallback()
{
    frameCounter++;
    repaint();
}
