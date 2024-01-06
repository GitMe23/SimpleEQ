/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    // for-each component in getComps() vector, make visible in GUI...
    
    for( auto* comp : getComps() )
    {
        addAndMakeVisible(comp);
    }
    
    setSize (600, 400);
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // Top 3rd of gui for freq response, bottom 2 thirds for the sliders/components
    auto bounds = getLocalBounds();
    // Top third area reserved for response:
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    
    // Low cut area on the left
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    // Because left third is now reserved, the remainder is two thirds, 66.7%,
    // so halving that to get the right third of overall bounds for highCutArea:
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    
    // Set sliders to left and right area.
    lowCutFreqSlider.setBounds(lowCutArea);
    highCutFreqSlider.setBounds(highCutArea);
    
    // Peak freq slider gets top of the remaining centre column:
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    // After above, we have 2/3rds left of centre column, so half that to get a 3rd for peak gain:
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    // Left over is a third for the Q/bandwidth slider:
    peakQualitySlider.setBounds(bounds);
}

// return vector with all the components...
std::vector<juce::Component*> SimpleEQAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider
    };
}
