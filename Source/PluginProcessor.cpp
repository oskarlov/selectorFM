#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "RadioStream.h"
#include <iostream>

using namespace std;


SelectorFMAudioProcessor::SelectorFMAudioProcessor()
    : AudioProcessor (
        BusesProperties()
            .withOutput (
                "Output",
                juce::AudioChannelSet::stereo(),
                true
            )
    )
{
    formatManager.registerBasicFormats();
}

const juce::String SelectorFMAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SelectorFMAudioProcessor::acceptsMidi() const
{
    return false;
}

bool SelectorFMAudioProcessor::producesMidi() const
{
    return false;
}

bool SelectorFMAudioProcessor::isMidiEffect() const
{
    return false;
}

double SelectorFMAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SelectorFMAudioProcessor::getNumPrograms()
{
    return 1;
}

int SelectorFMAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SelectorFMAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String SelectorFMAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void SelectorFMAudioProcessor::changeProgramName (
    int index,
    const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void SelectorFMAudioProcessor::prepareToPlay (
    double sampleRate,
    int samplesPerBlock)
{

    currentSampleRate = sampleRate;

    transportSource.prepareToPlay(samplesPerBlock, sampleRate);


}

void SelectorFMAudioProcessor::releaseResources()
{
}

bool SelectorFMAudioProcessor::isBusesLayoutSupported (
    const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    const auto input  = layouts.getMainInputChannelSet();

    if (output != juce::AudioChannelSet::mono()
        && output != juce::AudioChannelSet::stereo())
        return false;

    if (input != output)
        return false;

    return true;
}

void SelectorFMAudioProcessor::processBlock (
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{


    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused (midiMessages);

    buffer.clear();

    if (radioStream.isStreaming())
    {
        if (!radioPrimed)
        {
            const int64_t initialDelay =
                static_cast<int64_t>(
                    currentSampleRate * 0.5
                );

            const auto writePosition =
                radioStream.getWritePosition();
            
            if (writePosition < initialDelay)
                return;

            radioPlaybackPosition.store(writePosition - initialDelay); 
                

            radioPrimed = true;
        }

        const auto oldest =
            radioStream.getOldestReadablePosition();

        if (radioPlaybackPosition < oldest)
        {
            radioPlaybackPosition =
                oldest;
        }


        if (platterHeld.load())
        {
            return;
        }

        const int framesRead =
            radioStream.readAudioAt(
                buffer,
                radioPlaybackPosition,
                buffer.getNumSamples()
            );

            
        if (sampleRecorder.isRecording())
        {
            sampleRecorder.pushAudio(buffer);
        }



        radioPlaybackPosition.store(radioPlaybackPosition.load() + framesRead);

        return;
    }



    if (readerSource != nullptr)
    {
        juce::AudioSourceChannelInfo bufferInfo(
            &buffer,
            0,
            buffer.getNumSamples()
        );

        transportSource.getNextAudioBlock(
            bufferInfo
        );
    }

}

bool SelectorFMAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor*
SelectorFMAudioProcessor::createEditor()
{
    return new SelectorFMAudioProcessorEditor (*this);
}

void SelectorFMAudioProcessor::getStateInformation (
    juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
}

void SelectorFMAudioProcessor::setStateInformation (
    const void* data,
    int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SelectorFMAudioProcessor();
}



/*Audio Player Functionality*/

bool SelectorFMAudioProcessor::loadFile(const juce::File& file)

{
    auto* reader = formatManager.createReaderFor(file);

    if (reader == nullptr)
        return false;

    const auto sourceSampleRate = reader->sampleRate;

    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

    transportSource.stop();

    transportSource.setSource(nullptr);

    readerSource.reset();

    transportSource.setSource(newSource.get(), 0, nullptr, sourceSampleRate);

    readerSource = std::move(newSource);

    loadedFileName = file.getFileName();

    return true;
}


void SelectorFMAudioProcessor::play()
{
    if (readerSource != nullptr)
        transportSource.start();

}

void SelectorFMAudioProcessor::pause()
{
    transportSource.stop();
}

void SelectorFMAudioProcessor::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}



/*queries*/

bool SelectorFMAudioProcessor::isPlaying() const
{
    return radioStream.isStreaming();
}

bool SelectorFMAudioProcessor::isFileLoaded() const
{
    return readerSource != nullptr;
}

bool SelectorFMAudioProcessor::hasFinished() const
{
    return transportSource.hasStreamFinished();
}


/*position*/

double SelectorFMAudioProcessor::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

double SelectorFMAudioProcessor::getLengthInSeconds() const
{
    return transportSource.getLengthInSeconds();
}

void SelectorFMAudioProcessor::setPosition(double positionInSeconds)
{
    transportSource.setPosition(positionInSeconds);
}


/*Gain*/

void SelectorFMAudioProcessor::setGain(double gainValueDb)
{
    auto linearGain = juce::Decibels::decibelsToGain(gainValueDb);
    transportSource.setGain(linearGain);
}






/*filename*/

juce::String SelectorFMAudioProcessor::getLoadedFileName() const
{
    return loadedFileName;
}




void SelectorFMAudioProcessor::startRadioStation(
    const juce::String& streamUrl)
{
    transportSource.stop();

    radioPrimed = false;

    radioStream.start(
        streamUrl,
        static_cast<int>(currentSampleRate)
    );
}

void SelectorFMAudioProcessor::stopRadioStation()
{
    radioStream.stop();
}

void SelectorFMAudioProcessor::setPlatterHeld(bool held)
{
    platterHeld.store(held);
}



double SelectorFMAudioProcessor::getSecondsBehindLive() const
{
    if (!radioStream.isStreaming() || !radioPrimed)
        return 0.0;

    const auto writePosition =
        radioStream.getWritePosition();


    const auto playbackPosition =
        radioPlaybackPosition.load();
    


    const auto framesBehind =
        writePosition - playbackPosition;
    
    

    return juce::jmax(
        0.0,
        static_cast<double>(framesBehind)
            / currentSampleRate
    );
}

void SelectorFMAudioProcessor::startSampling()
{
    auto file =
        juce::File::getSpecialLocation(
            juce::File::userDesktopDirectory
        )
        .getChildFile("selectorFM_sample.wav");

    sampleRecorder.start(
        file,
        currentSampleRate
    );
}

void SelectorFMAudioProcessor::stopSampling()
{
    sampleRecorder.stop();
}

bool SelectorFMAudioProcessor::isSampling() const
{
    return sampleRecorder.isRecording();
}