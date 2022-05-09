/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cassert>

//==============================================================================
MalgukiAudioProcessorEditor::MalgukiAudioProcessorEditor (MalgukiAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), frameCounter(0), valueTreeState(vts)
{
    startTimerHz(60);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 400);

    mix.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mix.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 90, 15);
    mix.setTextValueSuffix(" Mix");
    mix.setMouseDragSensitivity(50);

    preGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    preGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 15);
    preGain.setTextValueSuffix(" Pre Gain");
    preGain.setMouseDragSensitivity(50);

    postGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    postGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 15);
    postGain.setTextValueSuffix(" Post Gain");
    postGain.setMouseDragSensitivity(50);

    delayTime.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delayTime.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 130, 15);
    delayTime.setTextValueSuffix(" Delay Time");
    delayTime.setMouseDragSensitivity(50);

    feedback.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedback.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 15);
    feedback.setTextValueSuffix(" Feedback");
    feedback.setMouseDragSensitivity(50);

    addAndMakeVisible(&mix);
    addAndMakeVisible(&preGain);
    addAndMakeVisible(&postGain);
    addAndMakeVisible(&delayTime);
    addAndMakeVisible(&feedback);

    feedbackAttachment.reset(new SliderAttachment(valueTreeState, "feedback", feedback));
    mixAttachment.reset(new SliderAttachment(valueTreeState, "mix", mix));
    preAttachment.reset(new SliderAttachment(valueTreeState, "preGain", preGain));
    postAttachment.reset(new SliderAttachment(valueTreeState, "postGain", postGain));
    delayAttachment.reset(new SliderAttachment(valueTreeState, "delayTime", delayTime));
}

MalgukiAudioProcessorEditor::~MalgukiAudioProcessorEditor()
{
}

//==============================================================================
void MalgukiAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(getLookAndFeel().findColour(juce::Slider::thumbColourId));

    assert(audioProcessor.springArrays.size() > 0);
    auto& particles = audioProcessor.springArrays[0].getParticles();
 
    float radius = 5.f;
    float scale = getWidth() / 800.f * 30.f;

    float offsetX = (getWidth() - (particles.at(particles.size() - 1).pos().x * scale)) / 2;
    float offsetY = 60;

    for (const Particle& particle : particles) {
        Vector pos = particle.pos();
        juce::Point<float> p (offsetX + particle.pos().x * scale,
                              offsetY - particle.pos().y * scale);
        
        g.fillEllipse(p.x, p.y, radius, radius);
    }
 
    // g.fillAll (juce::Colours::black);
    // g.setColour (juce::Colours::white);
    g.setFont(30.0f);
    // g.drawFittedText ("malguki", 0, 0, getWidth(), 40, juce::Justification::centred, 1);
}

void MalgukiAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int bigSize = 70;
    int smallSize = 50;

    int centerX = getWidth() / 2;
    int centerY = getHeight() / 2;

    int smallX = centerX / 3;

    preGain.setBounds(1 * smallX - smallSize,
                      centerY - smallSize,
                      2 * smallSize,
                      2 * smallSize);

    delayTime.setBounds(2 * smallX - smallSize,
                      centerY - smallSize,
                      2 * smallSize,
                      2 * smallSize);

    mix.setBounds(centerX - bigSize,
                  centerY - bigSize,
                  2 * bigSize,
                  2 * bigSize);

    feedback.setBounds(4 * smallX - smallSize,
                      centerY - smallSize,
                      2 * smallSize,
                      2 * smallSize);

    postGain.setBounds(5 * smallX - smallSize,
                      centerY - smallSize,
                      2 * smallSize,
                      2 * smallSize);

}

void MalgukiAudioProcessorEditor::timerCallback()
{
    frameCounter++;
    repaint();
}
