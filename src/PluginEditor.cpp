#include "PluginEditor.h"

AudioFragmenterAudioProcessorEditor::AudioFragmenterAudioProcessorEditor(AudioFragmenterAudioProcessor& p) : AudioProcessorEditor(p), processor(p)
{
    setSize(420, 220);
    for (auto* s : { &length, &wet, &recent }) { s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 22); addAndMakeVisible(s); }
    length.textFromValueFunction = [](double v) { return juce::String(juce::roundToInt(v)) + " ms"; };
    wet.textFromValueFunction = [](double v) { return juce::String(juce::roundToInt(v * 100.0)) + "%"; };
    recent.textFromValueFunction = [](double v) { return juce::String(juce::roundToInt(v)); };
    fade.setClickingTogglesState(true);
    fade.onStateChange = [this]
    {
        fade.setButtonText(fade.getToggleState() ? "Fade: On" : "Fade: Off");
    };
    fade.setButtonText("Fade: On");
    addAndMakeVisible(fade);
    lengthLabel.setText("Fragment Length", juce::dontSendNotification); wetLabel.setText("Dry/Wet", juce::dontSendNotification); recentLabel.setText("Recent Slices N", juce::dontSendNotification);
    addAndMakeVisible(lengthLabel); addAndMakeVisible(wetLabel); addAndMakeVisible(recentLabel);
    lengthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "fragmentLengthMs", length);
    wetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "dryWet", wet);
    recentAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "recentSlices", recent);
    fadeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, "fadeEnabled", fade);
}
void AudioFragmenterAudioProcessorEditor::paint(juce::Graphics& g) { g.fillAll(juce::Colours::darkgrey); g.setColour(juce::Colours::white); g.setFont(18.0f); g.drawText("AudioFragmenter", 16, 12, 280, 24, juce::Justification::left); }
void AudioFragmenterAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().withTrimmedTop(42).reduced(12);
    const int column = area.getWidth() / 4;
    lengthLabel.setBounds(0, area.getY(), column, 24); length.setBounds(0, area.getY() + 24, column, area.getHeight() - 24);
    wetLabel.setBounds(column, area.getY(), column, 24); wet.setBounds(column, area.getY() + 24, column, area.getHeight() - 24);
    recentLabel.setBounds(column * 2, area.getY(), column, 24); recent.setBounds(column * 2, area.getY() + 24, column, area.getHeight() - 24);
    fade.setBounds(column * 3, area.getY() + 24, column, area.getHeight() - 24);
}
