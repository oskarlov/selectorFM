#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"
#include "RadioStation.h"



class CoolSlider : public juce::Slider
{
    public:
        CoolSlider(const juce::String& title,
            juce::Slider::SliderStyle style, 
        juce::Slider::TextEntryBoxPosition textBoxPosition) : juce::Slider(style, textBoxPosition)
        {
            setRange(-60, 0.0, 0.1);
            label.setText(title, juce::dontSendNotification);
            label.attachToComponent(this, false);
    
        }
    private:
        juce::Label label;

};

class SelectorSlider : public juce::Slider
{
    public:
        SelectorSlider(const juce::String& title) : juce::Slider()
        {

            setSliderStyle(juce::Slider::Rotary);
            setRange(0,2,1);

            setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 80, 20);

            label.setText(title, juce::dontSendNotification);
            label.attachToComponent(this, false);
    
        }
    private:
        juce::Label label;

};




class RotaryPositionSlider : public juce::Slider
{
    public:
        RotaryPositionSlider() : juce::Slider()
        {

            setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

            setRange(0.0, 1.0, 0.0);

            setRotaryParameters(
                0.0f,
                juce::MathConstants<float>::twoPi,
                false
            );

            setColour(
                juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack
            );

            setColour(
                juce::Slider::thumbColourId, juce::Colours::orange
            );

    
        }
    private:
        juce::Label label;

};


class SelectorFMAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit SelectorFMAudioProcessorEditor (
        SelectorFMAudioProcessor&);

    ~SelectorFMAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
            SelectorFMAudioProcessorEditor
        )

        SelectorFMAudioProcessor& processor;

        juce::TextButton playPauseButton {"Play"};
        juce::TextButton sampleButton {"Sample"};
  
        
        juce::Label fileNameLabel;
        juce::Label liveDelayLabel;
        juce::Label stationNameLabel;

        RotaryPositionSlider positionSlider;

        CoolSlider gainSlider{"Gain", juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow};

        SelectorSlider stationSlider{"Station"};

        std::unique_ptr<juce::FileChooser> fileChooser;

        bool userIsDraggingPosition = false;
        bool userIsDraggingGain = false;

        void openFile();
        void timerCallback() override;
        void updateControls();


        static juce::String formatTime(double seconds);

        double platterPhase = 0.0;
        bool platterBeingHeld = false;

        bool beingSampled = false;

        std::vector<RadioStation> stations {
        { "Fluid", "https://ice5.somafm.com/fluid-128-mp3", "MP3", 128 },
        { "The Trip", "https://ice5.somafm.com/thetrip-128-mp3", "MP3", 128 },
        { "PopTron", "https://ice5.somafm.com/poptron-128-mp3", "MP3", 128 }
};

        




};


