#pragma once

#include <juce_core/juce_core.h>
#include "RollingAudioBuffer.h"

class RadioStream : private juce::Thread
{
public:
    RadioStream();
    ~RadioStream() override;

    void start(
        const juce::String& streamUrl,
        int sampleRate
    );

    void stop();

    bool isStreaming() const;
    int64_t getWritePosition() const;
    int64_t getOldestReadablePosition() const;

    int readAudioAt(
        juce::AudioBuffer<float>& destination,
        int64_t position,
        int numFrames
    );



private:
    void run() override;

    juce::ChildProcess ffmpegProcess;
    juce::String url;

    RollingAudioBuffer rollingBuffer;

    int outputSampleRate = 48000;
};