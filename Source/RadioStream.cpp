#include "RadioStream.h"
#include <array>
#include <cstring>


RadioStream::RadioStream()
    : juce::Thread("Radio Stream Thread")
{
}


RadioStream::~RadioStream()
{
    stop();
}

void RadioStream::start(const juce::String& streamUrl, int sampleRate)
{
    stop();

    url = streamUrl;
    outputSampleRate = sampleRate;

    rollingBuffer.prepare(
        sampleRate,
        30.0
    );

    startThread();
}


void RadioStream::stop()
{
    signalThreadShouldExit();

    if (ffmpegProcess.isRunning())
        ffmpegProcess.kill();

    stopThread(3000);
}


void RadioStream::run()
{
    juce::StringArray arguments;

    arguments.add("/usr/local/bin/ffmpeg");

    arguments.add("-hide_banner");
    arguments.add("-loglevel");
    arguments.add("error");

    arguments.add("-nostdin");

    arguments.add("-i");
    arguments.add(url);

    arguments.add("-vn");

    arguments.add("-f");
    arguments.add("f32le");

    arguments.add("-acodec");
    arguments.add("pcm_f32le");

    arguments.add("-ac");
    arguments.add("2");

    arguments.add("-ar");
    arguments.add(juce::String(outputSampleRate));

    arguments.add("pipe:1");



    const bool started =
    ffmpegProcess.start(
        arguments,
        juce::ChildProcess::wantStdOut
    );


    if (!started)
    {
        DBG("Could not start FFmpeg");
        return;
    }

    DBG("FFmpeg started");







    constexpr int networkBufferSize = 8192;

    std::array<char, networkBufferSize> byteBuffer;

    std::vector<char> pendingBytes;

    while (!threadShouldExit()
           && ffmpegProcess.isRunning())
    {
        const int bytesRead =
            ffmpegProcess.readProcessOutput(
                byteBuffer.data(),
                networkBufferSize
            );

        if (bytesRead <= 0)
            continue;

        pendingBytes.insert(
            pendingBytes.end(),
            byteBuffer.begin(),
            byteBuffer.begin() + bytesRead
        );


        constexpr int bytesPerFrame =
            2 * sizeof(float);

        const int completeFrames =
            static_cast<int>(
                pendingBytes.size()
                / bytesPerFrame
            );


        std::vector<StereoFrame> frames(
            static_cast<size_t>(completeFrames)
        );

        for (int i = 0;
            i < completeFrames;
            ++i)
        {
            const auto byteOffset =
                static_cast<size_t>(
                    i * bytesPerFrame
                );

            float left = 0.0f;
            float right = 0.0f;

            std::memcpy(
                &left,
                pendingBytes.data() + byteOffset,
                sizeof(float)
            );

            std::memcpy(
                &right,
                pendingBytes.data()
                    + byteOffset
                    + sizeof(float),
                sizeof(float)
            );

            frames[
                static_cast<size_t>(i)
            ] = { left, right };
        }

        if (completeFrames > 0)
        {
            rollingBuffer.write(
                frames.data(),
                completeFrames
            );
        }

        const auto consumedBytes =
            static_cast<size_t>(
                completeFrames * bytesPerFrame
            );

        pendingBytes.erase(
            pendingBytes.begin(),
            pendingBytes.begin()
                + static_cast<std::ptrdiff_t>(
                    consumedBytes
                )
        );



    }



    DBG("FFmpeg stream stopped");
}


bool RadioStream::isStreaming() const
{
    return isThreadRunning();
}

int64_t RadioStream::getWritePosition() const
{
    return rollingBuffer.getWritePosition();
}

int64_t RadioStream::getOldestReadablePosition() const
{
    return rollingBuffer.getOldestReadablePosition();
}

int RadioStream::readAudioAt(
    juce::AudioBuffer<float>& destination,
    int64_t position,
    int numFrames)
{
    return rollingBuffer.read(
        destination,
        position,
        numFrames
    );
}


