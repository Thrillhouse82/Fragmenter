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
    juce::Slider length, syncDivision, wet, recent;
    juce::ComboBox lengthMode;
    juce::ToggleButton fade;
    juce::Label lengthLabel, syncDivisionLabel, wetLabel, recentLabel, lengthModeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthAttachment, syncDivisionAttachment, wetAttachment, recentAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lengthModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fadeAttachment;

    void updateLengthModeUi();
};
