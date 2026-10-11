#pragma once

#include <juce_core/juce_core.h>

struct RadioStation
{
    juce::String name;
    juce::String streamUrl;
    juce::String codec;

    int bitrate = 0;
};

