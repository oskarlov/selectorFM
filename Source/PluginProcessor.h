#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "RadioStream.h"
#include <atomic>
#include "SampleRecorder.h"

class SelectorFMAudioProcessor final : public juce::AudioProcessor
{
public:
    SelectorFMAudioProcessor();
    ~SelectorFMAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>& buffer,
                        juce::MidiBuffer& midiMessages) override;

    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;

    const juce::String getProgramName (int index) override;
    void changeProgramName (int index,
                            const juce::String& newName) override;
    
    void getStateInformation (juce::MemoryBlock& destData) override;

    void setStateInformation (const void* data,
                                int sizeInBytes) override;

                                
        
    
    void play();
    void pause();
    void stop();


    bool loadFile(const juce::File& file);

    bool isPlaying() const;
    bool isFileLoaded() const;
    bool hasFinished() const;

    double getCurrentPosition() const;
    double getLengthInSeconds() const;

    void setPosition(double positionInSeconds);

    void setGain(double gainValueDb);

    juce::String getLoadedFileName() const;



    void startRadioStation(const juce::String& streamUrl);
    void stopRadioStation();
    bool radioPrimed = false;

    void setPlatterHeld(bool held);

    std::atomic<int64_t> radioPlaybackPosition = { 0 };

    double getSecondsBehindLive() const;

    void startSampling();
    void stopSampling();
    bool isSampling() const;
    

private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
            SelectorFMAudioProcessor
        )

        juce::AudioFormatManager formatManager;

        std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

        juce::AudioTransportSource transportSource;

        juce::String loadedFileName;

        RadioStream radioStream;

        SampleRecorder sampleRecorder;
        

        double currentSampleRate = 48000.0;

        std::atomic<bool> platterHeld {false};

};

