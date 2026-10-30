cmake_minimum_required(VERSION 3.22)

project(selectorFM VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(selectorFM
    COMPANY_NAME "YourName"

    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    COPY_PLUGIN_AFTER_BUILD TRUE

    PLUGIN_MANUFACTURER_CODE Ynam
    PLUGIN_CODE SlF1

    FORMATS AU VST3 Standalone

    PRODUCT_NAME "selectorFM"
)

target_sources(selectorFM
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/RadioStream.h
        Source/RadioStream.cpp
        Source/RollingAudioBuffer.h
        Source/RollingAudioBuffer.cpp
        Source/SampleRecorder.h
        Source/SampleRecorder.cpp 
            

)

target_compile_definitions(selectorFM
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(selectorFM
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_dsp

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)