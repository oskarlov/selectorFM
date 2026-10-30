

#include "SampleRecorder.h"

SampleRecorder::SampleRecorder()
{
    writerThread.startThread();
}


SampleRecorder::~SampleRecorder()
{
    stop();
    writerThread.stopThread(2000);
}


bool SampleRecorder::start(const juce::File& file, double sampleRate)
{
    stop();

    file.deleteFile();

    std::unique_ptr<juce::OutputStream> fileStream = file.createOutputStream();

    if (fileStream == nullptr)
        return false;

    juce::WavAudioFormat wavFormat;


    juce::AudioFormatWriterOptions options;

    options = options
        .withSampleRate(sampleRate)
        .withNumChannels(2)
        .withBitsPerSample(24);

    auto writer = wavFormat.createWriterFor(fileStream, options);

    if (writer == nullptr)
        return false;

    threadedWriter = std::make_unique< juce::AudioFormatWriter::ThreadedWriter >(
        writer.release(),
        writerThread,
        32768
    );
    {
        const juce::ScopedLock lock(writerLock);

        activeWriter.store(
            threadedWriter.get()
        );
    }

    return true;
}


void SampleRecorder::pushAudio(
    const juce::AudioBuffer<float>& buffer)
    {
        const juce::ScopedLock lock(writerLock);

        if (auto* writer = activeWriter.load())
        {
            writer->write(
                buffer.getArrayOfReadPointers(),
                buffer.getNumSamples()
            );
        }
    }

void SampleRecorder::stop()
{
    {
    const juce::ScopedLock lock(writerLock);

    activeWriter.store(nullptr);
    }
    threadedWriter.reset();
}

bool SampleRecorder::isRecording() const
{
    return activeWriter.load() != nullptr;
}