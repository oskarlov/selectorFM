#include "PluginEditor.h"

#include "RadioStation.h"

SelectorFMAudioProcessorEditor::
SelectorFMAudioProcessorEditor (
    SelectorFMAudioProcessor& p)
    : AudioProcessorEditor (&p),
    processor(p)
{
    setSize (500, 400);

    addAndMakeVisible(openButton);
    addAndMakeVisible(playPauseButton);
    addAndMakeVisible(stopButton);

    addAndMakeVisible(fileNameLabel);
    addAndMakeVisible(timeLabel);
    addAndMakeVisible(positionSlider);
    addAndMakeVisible(gainSlider);

    fileNameLabel.setText("No audio file loaded", juce::dontSendNotification);

    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);

    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setEnabled(false);

    gainSlider.setEnabled(false);

    playPauseButton.setEnabled(false);
    stopButton.setEnabled(false);



    RadioStation station;

    station.name = "Test_station";
    station.streamUrl = "https.//s";
    station.codec = "MP3";
    station.bitrate = 192;

    DBG("Station: " << station.name);
    DBG("Bitrate: " << station.bitrate);




    juce::URL url("https://de1.api.radio-browser.info/json/stations/search?name=jazz&limit=5");

    DBG(url.toString(true));


    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
    .withConnectionTimeoutMs(5000)
    .withExtraHeaders("User-Agent: selectorFM/0.1\r\n");


    auto stream = url.createInputStream(options);

    if (stream == nullptr)
    {
        DBG("Could not connect");
        return;
    }

    auto response = stream->readEntireStreamAsString();

    DBG(response);






    openButton.onClick = [this]
    {
        openFile();
    };

    playPauseButton.onClick = [this]
    {
        if (processor.isPlaying())
            processor.pause();
        else    
            processor.play();
        
        updateControls();
    };

    stopButton.onClick = [this]
    {
        processor.stop();
        updateControls();
    };

    positionSlider.onDragStart = [this]
    {
        userIsDraggingPosition = true;
    };

    positionSlider.onDragEnd = [this]
    {
        processor.setPosition(positionSlider.getValue());

        userIsDraggingPosition = false;
    };


    gainSlider.onValueChange = [this]()
    {
        processor.setGain(gainSlider.getValue());

    };



    startTimerHz(20);

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
        "Look a JUICE Plugin",
        getLocalBounds(),
        juce::Justification::centred,
        1
    );

}

void SelectorFMAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    auto fileRow = area.removeFromTop(35);

    openButton.setBounds( fileRow.removeFromRight(100));

    fileRow.removeFromRight(10);

    fileNameLabel.setBounds(fileRow);

    area.removeFromTop(20);

    positionSlider.setBounds(area.removeFromTop(30));

    timeLabel.setBounds(area.removeFromTop(25));

    area.removeFromTop(20);

    auto transportRow = area.removeFromTop(40);

    playPauseButton.setBounds(transportRow.removeFromLeft(100));

    stopButton.setBounds(transportRow.removeFromRight(100));

    area.removeFromTop(20);

    auto fxSection = area.removeFromTop(140);

    gainSlider.setBounds(fxSection.removeFromLeft(120));
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

                positionSlider.setRange(
                    0.0,
                    processor.getLengthInSeconds(),
                    0.01
                );

            }

            updateControls();


        }
        );
        
    }

void SelectorFMAudioProcessorEditor::timerCallback()
{
    if (processor.hasFinished())
        processor.stop();
    
    if (!userIsDraggingPosition)
    {
        positionSlider.setValue(processor.getCurrentPosition(), juce::dontSendNotification);
    }

    timeLabel.setText(formatTime(processor.getCurrentPosition())+ " / " + formatTime(processor.getLengthInSeconds()), juce::dontSendNotification);

    updateControls();
}

void SelectorFMAudioProcessorEditor::updateControls()
{
    const bool loaded = processor.isFileLoaded();
    const bool playing = processor.isPlaying();

    playPauseButton.setEnabled(loaded);
    stopButton.setEnabled(loaded);
    positionSlider.setEnabled(loaded);
    gainSlider.setEnabled(loaded);

    playPauseButton.setButtonText(
        playing ? "Pause" : "Play"
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