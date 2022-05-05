/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdio.h>

//==============================================================================
MalgukiAudioProcessor::MalgukiAudioProcessor()
    :
#ifndef JucePlugin_PreferredChannelConfigurations
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                     ),
#endif
     // nodes, mass, k, length
     r1(18, 20, 0.1, 20),
     r2(18, 20, 0.1, 20)
     // reverb(48, 50, 0.1, 400)
{
}

MalgukiAudioProcessor::~MalgukiAudioProcessor()
{
}

//==============================================================================
const juce::String MalgukiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MalgukiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MalgukiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MalgukiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MalgukiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MalgukiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MalgukiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MalgukiAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MalgukiAudioProcessor::getProgramName (int index)
{
    return {};
}

void MalgukiAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MalgukiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auxBuffer.setSize(1, samplesPerBlock * 2);
}

void MalgukiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MalgukiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MalgukiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    midiMessages.clear();
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    auto volume = noteOnVel;
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float result, sample;
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        // ..do something to the data...
        auto* inBuffer = buffer.getReadPointer(channel);
        auto* outBuffer = buffer.getWritePointer(channel);
        
        for (auto i = 0; i < buffer.getNumSamples(); ++i) {
            if (channel == 0) {
                sample = inBuffer[i];
                r1.applySample(sample);
                r1.update();
                result = r1.getSample();
            } else if (channel == 1) {
                sample = inBuffer[i];
                r2.applySample(sample);
                r2.update();
                result = r2.getSample();
            } else {
                result = inBuffer[i];
            }
            // if (sample != 0.f)
            //     printf("channel %d: %f\n", channel, result);
            outBuffer[i] = result * volume;
        }

    }
}

//==============================================================================
bool MalgukiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MalgukiAudioProcessor::createEditor()
{
    return new MalgukiAudioProcessorEditor (*this);
}

//==============================================================================
void MalgukiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MalgukiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MalgukiAudioProcessor();
}
