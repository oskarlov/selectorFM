#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"


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

        juce::TextButton openButton {"Open..."};
        juce::TextButton playPauseButton {"Play"};
        juce::TextButton stopButton {"Stop"};
        
        juce::Label fileNameLabel;
        juce::Label timeLabel;

        juce::Slider positionSlider;
        CoolSlider gainSlider{"Gain", juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow};

        std::unique_ptr<juce::FileChooser> fileChooser;

        bool userIsDraggingPosition = false;
        bool userIsDraggingGain = false;

        void openFile();
        void timerCallback() override;
        void updateControls();

        static juce::String formatTime(double seconds);


};


