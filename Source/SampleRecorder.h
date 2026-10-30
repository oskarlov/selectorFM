#pragma once

#include <juce_audio_formats/juce_audio_formats.h>

class SampleRecorder
{
public:
    SampleRecorder();
    ~SampleRecorder();

    bool start(
        const juce::File& file,
        double sampleRate
    );

    void stop();

    void pushAudio(
        const juce::AudioBuffer<float>& buffer
    );

    bool isRecording() const;

private:

    juce::TimeSliceThread writerThread {
        "Sample Writer Thread"
    };

    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter > threadedWriter;

    juce::CriticalSection writerLock;

    std::atomic<juce::AudioFormatWriter::ThreadedWriter* > activeWriter {nullptr};

};