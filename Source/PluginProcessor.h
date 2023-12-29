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
    
    void updatePeakFilter(const ChainSettings& chainSettings);
    
    /* create our own alias for Juce's Coefficients class (we use this for easier declaration of helper function to update coefficients)
        No member variables so we'll keep the helper functon as a static member function
     */
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);
    
    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }
    
    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain,
                         const CoefficientType& coefficients,
                         const Slope& lowCutSlope)

                        
    {
        // bypass all links in the chain:
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);
        
        // We want to switch based on the slope setting. We've defined an enum to define slope setting in headers file
        
        /*
         Recall that for slope, we used AudioParameterChoice for choice of 4 settings in db/oct: 12, 24, 36, 48 (not linear slider)

         Check impl of helper method designIIRHighpassHighOrderButterworthMethod
         We need to pass it an order corresponding below:
              
         Slope choice 0: 12 db/oct -> order: 2
         Slope choice 1: 24 db/oct -> order: 4
         Slope choice 2: 36 db/oct -> order: 6
         Slope choice 3: 48 db/oct -> order: 8

         */
        switch( lowCutSlope )
        {
                
            /*
             if order is 2, meaning 12 db/oct slope, the helper method will return an array of 1 coefficient object only
            which we will assign to the first filter in the cut filter chain. Also, stop bypassing that filter chain
             
             Below switch statement is modern C++ design, it processes everything in the array UP TO index
             e.g. indices 0-3 for 48, 0-2 for 36, 0-1 for 24, 0 for 12.
             */
                
            case Slope_48:
            {
                update<3>(chain, coefficients);
            }
            case Slope_36:
            {
                update<2>(chain, coefficients);
            }
            case Slope_24:
            {
                update<1>(chain, coefficients);
            }
            case Slope_12:
            {
                update<0>(chain, coefficients);
            }
        }
    }
    
    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);
    
    void updateFilters();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
