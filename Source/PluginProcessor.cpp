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
    springArrays{},
    parameters(*this, nullptr, juce::Identifier("malgukivst"),
               {
                   std::make_unique<juce::AudioParameterFloat> ("mix", "Mix",
                                                                juce::NormalisableRange<float>(0.0, 1.0, 0.01), 0.5),
                   std::make_unique<juce::AudioParameterFloat> ("preGain", "Pre Gain",
                                                                juce::NormalisableRange<float>(0.0, 3.0, 0.01), 1.0),
                   std::make_unique<juce::AudioParameterFloat> ("postGain", "Post Gain",
                                                                juce::NormalisableRange<float>(0.0, 3.0, 0.01), 1.0),
                   std::make_unique<juce::AudioParameterFloat> ("delayTime", "Delay Time",
                                                                juce::NormalisableRange<float>(0.02, 0.08, 0.001), 0.03),
                   std::make_unique<juce::AudioParameterFloat> ("feedback", "Feedback",
                                                                juce::NormalisableRange<float>(0.0, 1.0, 0.01), 0.6),
               })
{
    mix = parameters.getRawParameterValue("mix");
    preGain = parameters.getRawParameterValue("preGain");
    postGain = parameters.getRawParameterValue("postGain");
    delayTime = parameters.getRawParameterValue("delayTime");
    feedback = parameters.getRawParameterValue("feedback");
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
    // buffer de 2 segundos
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto delayBufferSize = sampleRate * 2.0;

    delayBuffer.setSize(totalNumInputChannels, (int) delayBufferSize);
    auxBuffer.setSize(totalNumInputChannels, samplesPerBlock);

    for (auto i = 0; i < totalNumInputChannels; ++i) {
        delayBuffer.clear(i, 0, delayBuffer.getNumSamples());
        auxBuffer.clear(i, 0, auxBuffer.getNumSamples());
    }

    springArrays.clear();
    springArrays.reserve(totalNumInputChannels);
    for (int i = 0; i < totalNumInputChannels; ++i) {
        // nodes, mass, k, length
        springArrays.emplace_back(18, 100, 1.0, 20);
    }

    // nodes, mass, k, length
    // r1(32, 100, 1.0, 20),
    // r2(32, 100, 1.0, 20)
    // reverb(48, 50, 0.1, 400)
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
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float result, sample;
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auxBuffer.copyFrom(channel, 0, buffer, channel, 0, bufferSize);

        // 1. aplicamos ganancia de entrada
        auxBuffer.applyGain(*preGain);

        auto* inAuxBuffer = auxBuffer.getReadPointer(channel);
        auto* outAuxBuffer = auxBuffer.getWritePointer(channel);

        auto* inBuffer = buffer.getReadPointer(channel);
        auto* outBuffer = buffer.getWritePointer(channel);
        
        auto& sa = springArrays[channel];

        // 2. pasamos por el difusor
        for (auto i = 0; i < bufferSize; ++i) {
            sample = inAuxBuffer[i];
            if (sample != 0.0)
                sa.applySample(sample);
            
            sa.update();
            result = sa.getSample();

            // 3. aplicamos ganancia de salida
            outAuxBuffer[i] = result * *postGain;
        }

        readFromBuffer(channel, delayBufferSize, auxBuffer, bufferSize, *delayTime, *feedback);
        // fillBuffer(channel, delayBufferSize, inBuffer, bufferSize);
        fillBuffer(channel, delayBufferSize, outAuxBuffer, bufferSize);
        for (auto i = 0; i < bufferSize; ++i) {
            outBuffer[i] = outAuxBuffer[i] * *mix + inBuffer[i] * (1.f - *mix);
        }
    }
    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

//==============================================================================
bool MalgukiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MalgukiAudioProcessor::createEditor()
{
    return new MalgukiAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void MalgukiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MalgukiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void MalgukiAudioProcessor::fillBuffer(int channel, int delayBufferSize, const float* channelData, int bufferSize)
{
    if (delayBufferSize > bufferSize + writePosition) {
        delayBuffer.copyFrom(channel, writePosition, channelData, bufferSize);
    } else {
        auto numSamplesToEnd = delayBufferSize - writePosition;
        delayBuffer.copyFrom(channel, writePosition, channelData, numSamplesToEnd);

        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        delayBuffer.copyFrom(channel, 0, channelData + numSamplesToEnd, numSamplesAtStart);
    }
}

void MalgukiAudioProcessor::readFromBuffer(int channel, int delayBufferSize, juce::AudioBuffer<float>& buffer, int bufferSize, float delayTime, float feedback)
{
    auto readPosition = writePosition - getSampleRate() * delayTime;

    if (readPosition < 0)
        readPosition += delayBufferSize;

    if (readPosition + bufferSize < delayBufferSize) {
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, feedback, feedback);
    } else {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, feedback, feedback);

        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, feedback, feedback);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MalgukiAudioProcessor();
}
