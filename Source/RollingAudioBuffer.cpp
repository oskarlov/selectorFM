#include "RollingAudioBuffer.h"

void RollingAudioBuffer::prepare(
    int sampleRate,
    double lengthSeconds)
{
    capacityFrames =
        static_cast<int>(
            sampleRate * lengthSeconds
        );

    history.resize(
        static_cast<size_t>(capacityFrames)
    );

    writePosition.store(0);

    safetyFrames =
        static_cast<int>(
            sampleRate * 0.25
        );
}

void RollingAudioBuffer::write(
    const StereoFrame* frames,
    int numFrames)
{
    if (capacityFrames <= 0)
        return;

    const auto startPosition =
        writePosition.load(
            std::memory_order_relaxed
        );

    for (int i = 0;
         i < numFrames;
         ++i)
    {
        const auto absolutePosition =
            startPosition + i;

        const auto bufferIndex =
            static_cast<size_t>(
                absolutePosition
                % capacityFrames
            );

        history[bufferIndex] =
            frames[i];
    }

    writePosition.store(
        startPosition + numFrames,
        std::memory_order_release
    );
}

int64_t
RollingAudioBuffer::getWritePosition() const
{
    return writePosition.load(
        std::memory_order_acquire
    );
}

int64_t
RollingAudioBuffer::getOldestReadablePosition() const
{
    const auto write =
        getWritePosition();

    return juce::jmax<int64_t>(
        0,
        write
        - capacityFrames
        + safetyFrames
    );
}

int64_t
RollingAudioBuffer::getLatestReadablePosition() const
{
    const auto write =
        getWritePosition();

    return juce::jmax<int64_t>(
        0,
        write - safetyFrames
    );
}



int RollingAudioBuffer::read(
    juce::AudioBuffer<float>& destination,
    int64_t startPosition,
    int numFrames) const
{
    jassert(
        destination.getNumChannels() >= 2
    );

    const auto oldest =
        getOldestReadablePosition();

    const auto latest =
        getLatestReadablePosition();

    if (startPosition < oldest
        || startPosition >= latest)
    {
        return 0;
    }

    const auto available =
        latest - startPosition;

    const int readable =
        static_cast<int>(
            juce::jmin<int64_t>(
                numFrames,
                available
            )
        );

    auto* left =
        destination.getWritePointer(0);

    auto* right =
        destination.getWritePointer(1);

    for (int i = 0;
         i < readable;
         ++i)
    {
        const auto absolutePosition =
            startPosition + i;

        const auto index =
            static_cast<size_t>(
                absolutePosition
                % capacityFrames
            );

        const auto& frame =
            history[index];

        left[i] = frame.left;
        right[i] = frame.right;
    }

    return readable;
}