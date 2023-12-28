/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//enum for slope to give us specific settings to switch from in a switch statement in PluginProcessor.cpp prepareToPlay()
enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};
// Extracting params from apvts, use a data structure to represent all of the param values for readability
struct ChainSettings
{
    float peakFreq { 0 }, peakGainInDecibels { 0 }, peakQuality {1.f};
    float lowCutFreq { 0 }, highCutFreq { 0 };
    Slope lowCutSlope { Slope::Slope_12 }, highCutSlope { Slope::Slope_12 };
};

// helper function to return the param values in the data struct
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

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

    /* Declaring a function to provide parameters to apvts below.
    Static as doesn't use any member variables
     */
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    /* AudioProcessorValueTreeState coordinates params on GUI with DSP variables
        Needs to be public so that the GUI can attach buttons, sliders etc to it
     */
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
private:
    /* Concept of DSP namespace - define a chain and pass in a processing context that will run through each element in the chain  automatically. E.g. Four filter elements in a chain to process cut, hi cut, slope etc
        The DSP namespace uses alot of nested namespaces and template meta programming
        Here I'm creating a type aliases to eliminate template and namespace definitions:
     */
    using Filter = juce::dsp::IIR::Filter<float>;
    
    // Cut filters built with their own processor chain instance:
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    
    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
