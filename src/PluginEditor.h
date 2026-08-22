#pragma once
#include "PluginProcessor.h"

class AudioFragmenterAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioFragmenterAudioProcessorEditor(AudioFragmenterAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    AudioFragmenterAudioProcessor& processor;
    juce::Slider length, wet;
    juce::Label lengthLabel, wetLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthAttachment, wetAttachment;
};
