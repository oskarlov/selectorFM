#include "PluginEditor.h"

#include "RadioStation.h"

SelectorFMAudioProcessorEditor::
SelectorFMAudioProcessorEditor (
    SelectorFMAudioProcessor& p)
    : AudioProcessorEditor (&p),
    processor(p)
{
    setSize (500, 400);

    addAndMakeVisible(playPauseButton);
    addAndMakeVisible(sampleButton);

    addAndMakeVisible(fileNameLabel);
    addAndMakeVisible(liveDelayLabel);
    addAndMakeVisible(stationNameLabel);
    addAndMakeVisible(positionSlider);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(stationSlider);

    fileNameLabel.setText("No audio file loaded", juce::dontSendNotification);

    gainSlider.setEnabled(false);


    playPauseButton.onClick = [this]
    {
        if (processor.isPlaying())
            processor.stopRadioStation();
        else    
        {
        const int index = static_cast<int>(stationSlider.getValue());

        const auto& station = stations[static_cast<size_t>(index)];

        processor.startRadioStation(station.streamUrl);
        }
        
        updateControls();
    };

    positionSlider.onDragStart = [this]

    {   
        platterBeingHeld = true;
        processor.setPlatterHeld(true);
    };

    positionSlider.onDragEnd = [this]
    {
        platterBeingHeld = false;
        processor.setPlatterHeld(false);
    };


    gainSlider.onValueChange = [this]()
    {
        processor.setGain(gainSlider.getValue());

    };

    stationSlider.onValueChange = [this]
    {
        const int index = static_cast<int>(stationSlider.getValue());

        const auto& station = stations[static_cast<size_t>(index)];

        stationNameLabel.setText(
            station.name,
            juce::dontSendNotification
        );
    };

    sampleButton.onClick = [this]
    {
        if (processor.isSampling())
            processor.stopSampling();
        else
            processor.startSampling();

        updateControls();
    };



    startTimerHz(60);

    updateControls();




}



void SelectorFMAudioProcessorEditor::paint (
    juce::Graphics& g)
{
    g.fillAll (
        getLookAndFeel().findColour (
            juce::ResizableWindow::backgroundColourId
        )
    );

    g.setColour (juce::Colours::white);
    g.setFont (22.0f);

    g.drawFittedText (
        "selectorFM",
        getLocalBounds(),
        juce::Justification::bottomRight,
        1
    );

}

void SelectorFMAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    auto headerSection = area.removeFromTop(120);
    auto headerControls = headerSection.withSizeKeepingCentre(240, 40);
    gainSlider.setBounds(headerSection.removeFromRight(120));
    stationSlider.setBounds(headerSection.removeFromLeft(120));
    stationNameLabel.setBounds(headerSection.removeFromLeft(150));

    auto transportSection = area.removeFromBottom(70);
    area.removeFromBottom(15);
    playPauseButton.setBounds(transportSection.removeFromLeft(150));
    sampleButton.setBounds(transportSection.removeFromRight(150));


    auto mainSection = area;
    auto timeArea = mainSection.removeFromBottom(30);
    liveDelayLabel.setBounds(timeArea);

    positionSlider.setBounds(mainSection.withSizeKeepingCentre(180, 180));


    
}



void SelectorFMAudioProcessorEditor::openFile()
    {
        fileChooser = std::make_unique<juce::FileChooser>("Choose an audio file", juce::File {}, "*.wav;*.aif;*.aiff");

        const auto flags = juce::FileBrowserComponent::openMode 
        | juce::FileBrowserComponent::canSelectFiles;

        
        fileChooser->launchAsync(flags, [this](const juce::FileChooser& chooser)
        {
            auto files = chooser.getResults();

            if (files.isEmpty())
                return;

            auto file = files[0];

            if (!file.existsAsFile())
                return;

            if (file == juce::File {})
                return;

            if (processor.loadFile(file))
            {
                fileNameLabel.setText(
                    file.getFileName(),
                    juce::dontSendNotification
                );

            }

            updateControls();


        }
        );
        
    }

void SelectorFMAudioProcessorEditor::timerCallback()
{
    if (processor.isPlaying() && !platterBeingHeld)
    {
        constexpr double rotationsPerSecond = 0.5;

        platterPhase += rotationsPerSecond / 60.0;

        if (platterPhase >= 1.0)
            platterPhase -= 1.0;

        positionSlider.setValue(
            platterPhase,
            juce::dontSendNotification
        );

    }

    const double secondsBehind =
    processor.getSecondsBehindLive();

    liveDelayLabel.setText(juce::String(secondsBehind, 1), juce::dontSendNotification);


}

void SelectorFMAudioProcessorEditor::updateControls()
{
    const bool loaded = processor.isFileLoaded();
    const bool playing = processor.isPlaying();
    const bool sampling = processor.isSampling();

    gainSlider.setEnabled(loaded);

    playPauseButton.setButtonText(
        playing ? "Pause" : "Play"
    );

    sampleButton.setButtonText(
        sampling ? "Stop sampling" : "Sample"
    );
}

juce::String SelectorFMAudioProcessorEditor::formatTime(double seconds)
{
    const int totalSeconds =
        static_cast<int>(seconds);

    const int minutes =
        totalSeconds / 60;

    const int remainingSeconds =
        totalSeconds % 60;

    return juce::String::formatted(
        "%02d:%02d",
        minutes,
        remainingSeconds
    );
}