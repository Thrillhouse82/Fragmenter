cmake_minimum_required(VERSION 3.22)
project(AudioFragmenter VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(JUCE_TAG 8.0.4 CACHE STRING "JUCE release tag")
FetchContent_Declare(juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        ${JUCE_TAG}
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(juce)

juce_add_plugin(AudioFragmenter
    COMPANY_NAME "Fragmenter"
    BUNDLE_ID com.fragmenter.audiofragmenter
    PLUGIN_MANUFACTURER_CODE Frgm
    PLUGIN_CODE Af01
    FORMATS VST3
    PRODUCT_NAME "AudioFragmenter"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(AudioFragmenter)

target_sources(AudioFragmenter PRIVATE
    src/PluginProcessor.cpp src/PluginProcessor.h
    src/PluginEditor.cpp src/PluginEditor.h)
target_compile_definitions(AudioFragmenter PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(AudioFragmenter PRIVATE juce::juce_audio_utils juce::juce_recommended_config_flags)

if(BUILD_TESTING)
    juce_add_console_app(AudioFragmenterTests
        PRODUCT_NAME "AudioFragmenterTests")
    target_sources(AudioFragmenterTests PRIVATE
        tests/AudioFragmenterTests.cpp
        src/PluginProcessor.cpp src/PluginProcessor.h
        src/PluginEditor.cpp src/PluginEditor.h)
    juce_generate_juce_header(AudioFragmenterTests)
    target_compile_definitions(AudioFragmenterTests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_STANDALONE_APPLICATION=1)
    target_link_libraries(AudioFragmenterTests PRIVATE juce::juce_audio_utils juce::juce_recommended_config_flags)
    add_test(NAME AudioFragmenterTests COMMAND AudioFragmenterTests)
endif()
