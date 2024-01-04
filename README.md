# Simple EQ VST Plugin

**Demonstration**: View the plugin in action, before GUI enhancement, hosted at [Instagram](https://www.instagram.com/p/C1mG1JBRMRO/).

![Plugin Image](https://gitme23.github.io/assets/images/SimpleEQ_plug.png)


## Overview

This is a simple Equalizer (EQ) and filter VST plugin implemented in C++. The plugin is built using the JUCE framework and provides basic audio processing functionalities. It includes low-cut, high-cut, and peak filters to shape the audio output.

## Features

- Low-cut filter with adjustable frequency and slope.
- High-cut filter with adjustable frequency and slope.
- Peak filter with adjustable frequency, gain, and quality.

## How to Use

1. **Installation**: Build the plugin using the provided C++ files and integrate it into your VST host.

2. **Parameters**:
   - **LowCut Freq**: Adjust the frequency of the low-cut filter.
   - **HighCut Freq**: Adjust the frequency of the high-cut filter.
   - **Peak Freq**: Adjust the center frequency of the peak filter.
   - **Peak Gain**: Adjust the gain of the peak filter in decibels.
   - **Peak Quality**: Adjust the quality factor of the peak filter.
   - **LowCut Slope**: Choose the slope of the low-cut filter (12, 24, 36, or 48 dB/Oct).
   - **HighCut Slope**: Choose the slope of the high-cut filter (12, 24, 36, or 48 dB/Oct).

## Building and Integration

1. Clone this repository.
2. Build the project using the provided C++ files.
3. Integrate the generated VST plugin into your audio project or use JUCE's AudioPluginHost

