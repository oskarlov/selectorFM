# selectorFM

selectorFM is an experimental JUCE/C++ internet radio player exploring turntable-style manipulation of live radio streams.

## Current functionality

- Select between a small set of hard-coded internet radio stations.
- Stream and decode radio using FFmpeg.
- Store decoded PCM audio in a rolling circular buffer.
- Play from an independent playback head behind the live stream.
- Rotary platter UI that spins during playback.
- Holding the platter pauses the playback head while the live stream continues buffering.
- Display current time behind the live edge.
- Basic DSP/FX controls.
- Record the current audio output to a WAV file.

## Architecture

```text
Internet radio
    ↓
FFmpeg
    ↓
PCM audio
    ↓
RollingAudioBuffer
    ↓
Playback head / platter
    ↓
DSP
    ↓
Output / SampleRecorder
```

## Built with

- C++ / JUCE
- CMake
- FFmpeg

Currently an early standalone prototype focused on testing live-radio buffering and turntable-style interaction.