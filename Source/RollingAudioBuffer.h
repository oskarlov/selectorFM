#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <atomic>
#include <vector>

struct StereoFrame
{
    float left = 0.0f;
    float right = 0.0f;
};

class RollingAudioBuffer
{
public:
    void prepare(
        int sampleRate,
        double lengthSeconds = 30.0
    );

    void write(
        const StereoFrame* frames,
        int numFrames
    );

    int read(
        juce::AudioBuffer<float>& destination,
        int64_t startPosition,
        int numFrames
    ) const;

    int64_t getWritePosition() const;

    int64_t getOldestReadablePosition() const;
    int64_t getLatestReadablePosition() const;

private:
    std::vector<StereoFrame> history;

    std::atomic<int64_t> writePosition { 0 };

    int capacityFrames = 0;
    int safetyFrames = 0;
};